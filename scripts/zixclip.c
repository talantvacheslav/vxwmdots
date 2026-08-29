// gcc zixclip.c -o zixclip -lX11 -lXfixes -lsqlite3 -lcrypto

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xfixes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <time.h>
#include <sys/stat.h>
#include <openssl/evp.h>
#include <unistd.h>
#include <fcntl.h>
#include <pwd.h>

#define ZIXCLIP_VERSION "0.1.0"

static Display *dpy;
static Window root, requestor_win;
static Atom clipboard_atom, targets_atom, utf8_atom;
static Atom image_png_atom, image_jpeg_atom, image_bmp_atom, image_webp_atom;
static sqlite3 *db;

static const char* get_db_path(void) {
    static char path[512];
    const char *xdg_cache = getenv("XDG_CACHE_HOME");
    const char *home = getenv("HOME");
    
    if (!home) {
        struct passwd *pw = getpwuid(getuid());
        home = pw ? pw->pw_dir : ".";
    }
    
    if (xdg_cache) {
        snprintf(path, sizeof(path), "%s/zixclip/db.sqlite", xdg_cache);
    } else {
        snprintf(path, sizeof(path), "%s/.cache/zixclip/db.sqlite", home);
    }
    return path;
}

static const char* get_db_dir(void) {
    static char dir[512];
    const char *path = get_db_path();
    strncpy(dir, path, sizeof(dir) - 1);
    dir[sizeof(dir) - 1] = '\0';
    
    char *last_slash = strrchr(dir, '/');
    if (last_slash) *last_slash = '\0';
    
    return dir;
}

static void md5_hash(const unsigned char *data, size_t len, char *out) {
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int digest_len;
    
    EVP_DigestInit_ex(ctx, EVP_md5(), NULL);
    EVP_DigestUpdate(ctx, data, len);
    EVP_DigestFinal_ex(ctx, digest, &digest_len);
    EVP_MD_CTX_free(ctx);
    
    for (unsigned int i = 0; i < digest_len; i++) {
        sprintf(out + i * 2, "%02x", digest[i]);
    }
    out[digest_len * 2] = '\0';
}

static void init_db(void) {
    char *err = NULL;
    sqlite3_exec(db, 
        "CREATE TABLE IF NOT EXISTS clips ("
        "id INTEGER PRIMARY KEY,"
        "type TEXT,"
        "data BLOB,"
        "preview TEXT,"
        "hash TEXT UNIQUE,"
        "created INTEGER"
        ");"
        "CREATE INDEX IF NOT EXISTS idx_created ON clips(created DESC);"
        "CREATE INDEX IF NOT EXISTS idx_hash ON clips(hash);",
        NULL, NULL, &err);
    if (err) {
        fprintf(stderr, "DB init: %s\n", err);
        sqlite3_free(err);
    }
}

static unsigned char* get_selection(Atom selection, Atom target, size_t *len, Atom *actual_type) {
    Atom type;
    int format;
    unsigned long nitems, bytes_after;
    unsigned char *data = NULL;
    
    XConvertSelection(dpy, selection, target, XA_STRING, requestor_win, CurrentTime);
    XFlush(dpy);
    
    XEvent ev;
    while (1) {
        XNextEvent(dpy, &ev);
        if (ev.type == SelectionNotify) break;
    }
    
    if (ev.xselection.property == None) return NULL;
    
    XGetWindowProperty(dpy, requestor_win, ev.xselection.property,
        0, 0x1FFFFFFF, False, AnyPropertyType,
        &type, &format, &nitems, &bytes_after, &data);
    
    *len = nitems * (format / 8);
    *actual_type = type;
    return data;
}

static void save_clip(const char *type_str, const unsigned char *data, size_t len, const char *preview) {
    char hash[33];
    md5_hash(data, len, hash);
    
    sqlite3_stmt *check_stmt;
    sqlite3_prepare_v2(db, "SELECT id FROM clips WHERE hash = ? LIMIT 1", -1, &check_stmt, NULL);
    sqlite3_bind_text(check_stmt, 1, hash, -1, SQLITE_STATIC);
    
    int rc = sqlite3_step(check_stmt);
    if (rc == SQLITE_ROW) {
        int existing_id = sqlite3_column_int(check_stmt, 0);
        sqlite3_finalize(check_stmt);
        
        sqlite3_stmt *update_stmt;
        sqlite3_prepare_v2(db, "UPDATE clips SET created = ? WHERE id = ?", -1, &update_stmt, NULL);
        sqlite3_bind_int64(update_stmt, 1, time(NULL));
        sqlite3_bind_int(update_stmt, 2, existing_id);
        sqlite3_step(update_stmt);
        sqlite3_finalize(update_stmt);
        return;
    }
    sqlite3_finalize(check_stmt);
    
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db,
        "INSERT INTO clips (type, data, preview, hash, created) VALUES (?, ?, ?, ?, ?)",
        -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, type_str, -1, SQLITE_STATIC);
    sqlite3_bind_blob(stmt, 2, data, len, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, preview, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, hash, -1, SQLITE_STATIC);
    sqlite3_bind_int64(stmt, 5, time(NULL));
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

static void check_targets(void) {
    Atom type;
    int format;
    unsigned long nitems, bytes_after;
    Atom *atoms = NULL;
    unsigned char *data = NULL;
    
    XConvertSelection(dpy, clipboard_atom, targets_atom, XA_STRING, requestor_win, CurrentTime);
    XFlush(dpy);
    
    XEvent ev;
    while (1) {
        XNextEvent(dpy, &ev);
        if (ev.type == SelectionNotify) break;
    }
    
    if (ev.xselection.property == None) return;
    
    XGetWindowProperty(dpy, requestor_win, ev.xselection.property,
        0, 0x1FFFFFFF, False, XA_ATOM,
        &type, &format, &nitems, &bytes_after, &data);
    
    atoms = (Atom*)data;
    int has_text = 0, has_png = 0, has_jpeg = 0, has_bmp = 0, has_webp = 0;
    Atom chosen_target = None;
    
    for (unsigned long i = 0; i < nitems; i++) {
        if (atoms[i] == image_png_atom) has_png = 1;
        if (atoms[i] == image_jpeg_atom) has_jpeg = 1;
        if (atoms[i] == image_bmp_atom) has_bmp = 1;
        if (atoms[i] == image_webp_atom) has_webp = 1;
        if (atoms[i] == utf8_atom) has_text = 1;
    }
    
    if (has_png) chosen_target = image_png_atom;
    else if (has_jpeg) chosen_target = image_jpeg_atom;
    else if (has_webp) chosen_target = image_webp_atom;
    else if (has_bmp) chosen_target = image_bmp_atom;
    else if (has_text) chosen_target = utf8_atom;
    
    if (chosen_target == None) {
        if (data) XFree(data);
        return;
    }
    
    size_t len;
    Atom actual;
    char *type_str = XGetAtomName(dpy, chosen_target);
    
    unsigned char *content = get_selection(clipboard_atom, chosen_target, &len, &actual);
    if (content && len > 0) {
        char preview[256];
        if (chosen_target == utf8_atom) {
            snprintf(preview, sizeof(preview), "%.*s", (int)(len > 200 ? 200 : len), content);
            for (int i = 0; preview[i]; i++) {
                if (preview[i] == '\n' || preview[i] == '\r' || preview[i] == '\t') {
                    preview[i] = ' ';
                }
            }
        } else {
            snprintf(preview, sizeof(preview), "[image %s %zu bytes]",
                type_str ? type_str : "?", len);
        }
        
        save_clip(type_str ? type_str : "unknown", content, len, preview);
        XFree(content);
    }
    
    if (type_str) XFree(type_str);
    if (data) XFree(data);
}

static void run_daemon(void) {
    int event_base, error_base;
    if (!XFixesQueryExtension(dpy, &event_base, &error_base)) {
        fprintf(stderr, "XFixes not available\n");
        return;
    }
    
    XFixesSelectSelectionInput(dpy, root, clipboard_atom,
        XFixesSetSelectionOwnerNotifyMask);
    
    while (1) {
        XEvent ev;
        XNextEvent(dpy, &ev);
        
        if (ev.type == SelectionNotify) continue;
        
        if (ev.type == event_base + XFixesSelectionNotify) {
            XFixesSelectionNotifyEvent *se = (XFixesSelectionNotifyEvent*)&ev;
            if (se->owner != None && se->owner != root && se->owner != requestor_win) {
                check_targets();
            }
        }
    }
}

static int do_list(void) {
    if (sqlite3_open(get_db_path(), &db) != SQLITE_OK) {
        fprintf(stderr, "Cannot open DB: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db,
        "SELECT id, type, preview FROM clips ORDER BY created DESC",
        -1, &stmt, NULL);
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char *type = (const char*)sqlite3_column_text(stmt, 1);
        const char *preview = (const char*)sqlite3_column_text(stmt, 2);
        
        if (strncmp(type, "image", 5) == 0) {
            printf("%d\t[%s] %s\n", id, type, preview ? preview : "");
        } else {
            printf("%d\t%s\n", id, preview ? preview : "");
        }
    }
    
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
}

static int do_select(int id) {
    if (sqlite3_open(get_db_path(), &db) != SQLITE_OK) {
        fprintf(stderr, "Cannot open DB: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db,
        "SELECT type, data FROM clips WHERE id = ?",
        -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, id);
    
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        fprintf(stderr, "Clip not found: %d\n", id);
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return 1;
    }
    
    const char *type = (const char*)sqlite3_column_text(stmt, 0);
    const void *data = sqlite3_column_blob(stmt, 1);
    int len = sqlite3_column_bytes(stmt, 1);
    
    char tmpfile[] = "/tmp/zixclip-XXXXXX";
    int fd = mkstemp(tmpfile);
    if (fd < 0) {
        perror("mkstemp");
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return 1;
    }
    
    if (write(fd, data, len) != len) {
        perror("write");
        close(fd);
        unlink(tmpfile);
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return 1;
    }
    close(fd);
    
    char cmd[512];
    if (strcmp(type, "text") == 0 || strncmp(type, "text/", 5) == 0) {
        snprintf(cmd, sizeof(cmd), "cat %s | xclip -selection clipboard", tmpfile);
    } else {
        snprintf(cmd, sizeof(cmd), "xclip -selection clipboard -t %s -i %s", type, tmpfile);
    }
    
    int rc = system(cmd);
    unlink(tmpfile);
    
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return rc;
}

static int do_daemon(void) {
    const char *db_dir = get_db_dir();
    mkdir(db_dir, 0755);
    
    dpy = XOpenDisplay(NULL);
    if (!dpy) {
        fprintf(stderr, "Cannot open display\n");
        return 1;
    }
    
    root = XDefaultRootWindow(dpy);
    if (root == 0 || root == None) {
        fprintf(stderr, "Invalid root window\n");
        XCloseDisplay(dpy);
        return 1;
    }
    
    XSetWindowAttributes attrs;
    attrs.override_redirect = True;
    attrs.event_mask = PropertyChangeMask;
    requestor_win = XCreateWindow(dpy, root, -10, -10, 1, 1, 0,
        CopyFromParent, InputOutput, CopyFromParent,
        CWOverrideRedirect | CWEventMask, &attrs);
    
    clipboard_atom = XInternAtom(dpy, "CLIPBOARD", False);
    targets_atom = XInternAtom(dpy, "TARGETS", False);
    utf8_atom = XInternAtom(dpy, "UTF8_STRING", False);
    image_png_atom = XInternAtom(dpy, "image/png", False);
    image_jpeg_atom = XInternAtom(dpy, "image/jpeg", False);
    image_bmp_atom = XInternAtom(dpy, "image/bmp", False);
    image_webp_atom = XInternAtom(dpy, "image/webp", False);
    
    if (sqlite3_open(get_db_path(), &db) != SQLITE_OK) {
        fprintf(stderr, "Cannot open DB: %s\n", sqlite3_errmsg(db));
        XCloseDisplay(dpy);
        return 1;
    }
    init_db();
    
    run_daemon();
    
    sqlite3_close(db);
    XCloseDisplay(dpy);
    return 0;
}

static void usage(const char *prog) {
    fprintf(stderr, "Usage: %s [daemon|list|select <id>]\n", prog);
    fprintf(stderr, "  daemon    Run clipboard monitor (default)\n");
    fprintf(stderr, "  list      List clipboard history\n");
    fprintf(stderr, "  select    Copy item <id> to clipboard\n");
}

int main(int argc, char **argv) {
    if (argc > 1) {
        if (strcmp(argv[1], "list") == 0) {
            return do_list();
        }
        if (strcmp(argv[1], "select") == 0) {
            if (argc < 3) {
                usage(argv[0]);
                return 1;
            }
            return do_select(atoi(argv[2]));
        }
        if (strcmp(argv[1], "daemon") == 0) {
            return do_daemon();
        }
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
            usage(argv[0]);
            return 0;
        }
    }
    
    return do_daemon();
}
