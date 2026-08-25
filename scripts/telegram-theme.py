#!/usr/bin/env python3
import json
import colorsys
import os
import shutil
import subprocess
import sys
import zipfile

WAL_JSON = os.path.expanduser("~/.cache/wal/colors.json")
TDATA = os.path.expanduser("~/.local/share/AyuGramDesktop/tdata")
THEME_FILE = os.path.join(TDATA, "colors.tdesktop-theme")
THEME_ZIP = os.path.join(TDATA, "pywal.tdesktop-theme")


def hx(c):
    c = c.lstrip("#")
    return tuple(int(c[i:i + 2], 16) for i in (0, 2, 4))


def to_hex(rgb):
    return "#%02x%02x%02x" % rgb


def lum(rgb):
    def f(v):
        v /= 255
        return v / 12.92 if v <= 0.03928 else ((v + 0.055) / 1.055) ** 2.4
    r, g, b = (f(v) for v in rgb)
    return 0.2126 * r + 0.7152 * g + 0.0722 * b


def contrast(a, b):
    l1, l2 = sorted((lum(a), lum(b)), reverse=True)
    return (l1 + 0.05) / (l2 + 0.05)


def mix(a, b, t):
    return tuple(round(a[i] + (b[i] - a[i]) * t) for i in range(3))


def lighten(c, t):
    return mix(c, (255, 255, 255), t)


def darken(c, t):
    return mix(c, (0, 0, 0), t)


def sat(c):
    _, s, _ = colorsys.rgb_to_hsv(*[v / 255 for v in c])
    return s


def readable(fg, bg, minimum=3.5):
    c = fg
    target = (255, 255, 255) if lum(bg) < 0.4 else (0, 0, 0)
    for t in (0.25, 0.45, 0.65, 0.85, 1.0):
        c = mix(fg, target, t)
        if contrast(c, bg) >= minimum:
            break
    return c


def main():
    if len(sys.argv) > 1:
        subprocess.run(["wal", "-i", sys.argv[1]], check=True)

    d = json.load(open(WAL_JSON))
    bg = hx(d["special"]["background"])
    fg = hx(d["special"]["foreground"])
    cols = [hx(d["colors"][f"color{i}"]) for i in range(8)]
    dark = lum(bg) < 0.35

    step = 0.10 if dark else 0.08
    surf1 = lighten(bg, step)
    surf2 = lighten(bg, step * 2)
    surf3 = lighten(bg, step * 3)

    accent_raw = max(cols[1:7], key=sat)
    accent = readable(accent_raw, bg, 3.0)
    on_accent = fg if contrast(fg, accent) >= 3.0 else (
        (0, 0, 0) if lum(accent) > 0.3 else (255, 255, 255))

    subtext = mix(fg, bg, 0.35)
    faint = mix(fg, bg, 0.55)

    msg_in = surf1
    msg_out = mix(accent, bg, 0.55 if dark else 0.45)
    if contrast(fg, msg_out) < 3.0:
        msg_out = darken(msg_out, 0.35) if dark else lighten(msg_out, 0.35)

    err = readable(cols[1], bg, 3.0)
    ok = readable(cols[2], bg, 3.0)

    overlay = "#000000b4" if dark else "#ffffffd8"
    overlay_text = "#ffffff" if dark else "#000000"

    btn = darken(bg, 0.15)
    if contrast(btn, fg) < 3.0:
        btn = darken(bg, 0.25)
    btn_hover = mix(btn, fg, 0.12)

    nicks = []
    pool = sorted(cols[1:8], key=sat, reverse=True)
    for c in pool:
        r = readable(c, bg, 3.0)
        if all(contrast(r, n) > 1.4 for n in nicks):
            nicks.append(r)
        if len(nicks) == 6:
            break
    while len(nicks) < 6:
        t = 0.3 + 0.12 * len(nicks)
        base = lighten(accent_raw, t) if not dark else darken(accent_raw, t * 0.5)
        nicks.append(readable(base, bg, 3.0))
    nick_hex = [to_hex(n) for n in nicks]

    C = {
        "bg": to_hex(bg), "surf1": to_hex(surf1), "surf2": to_hex(surf2),
        "surf3": to_hex(surf3), "fg": to_hex(fg), "subtext": to_hex(subtext),
        "faint": to_hex(faint), "accent": to_hex(accent),
        "on_accent": to_hex(on_accent), "err": to_hex(err), "ok": to_hex(ok),
        "msg_in": to_hex(msg_in), "msg_out": to_hex(msg_out),
        "overlay": overlay, "overlay_text": overlay_text,
        "nick1": nick_hex[0], "nick2": nick_hex[1], "nick3": nick_hex[2],
        "nick4": nick_hex[3], "nick5": nick_hex[4], "nick6": nick_hex[5],
        "btn": to_hex(btn), "btn_hover": to_hex(btn_hover),
    }

    theme = f"""// THEME EDITOR SERVICE INFO START
// ID: 0
// ACCESS: 0
// THEME EDITOR SERVICE INFO END
uiBg:{C['bg']};
uiMsg:{C['msg_in']};
uiBlock:{C['surf1']};
uiBlockActive:{C['surf2']};
uiText:{C['fg']};
uiTextSilver:{C['subtext']};
uiIcons:{C['subtext']};
uiLines:{C['faint']};
uiAccent:{C['accent']};
uiAccentText:{C['on_accent']};
uiError:{C['err']};
uiSuccess:{C['ok']};
uiOverlay:{C['overlay']};
uiOverlayText:{C['overlay_text']};
uiAlpha:#ffffff00;
windowBg:uiBg;
windowFg:uiText;
windowSubTextFg:uiTextSilver;
windowBoldFg:uiText;
windowBgOver:uiBlock;
windowBgRipple:uiBlock;
windowFgOver:uiText;
windowSubTextFgOver:uiText;
windowBgActive:{C['btn']};
windowFgActive:uiText;
windowActiveTextFg:uiAccent;
shadowFg:uiLines;
windowShadowFg:#00000074;
windowShadowFgFallback:uiLines;
slideFadeOutBg:uiBg;
slideFadeOutShadowFg:uiLines;
imageBg:uiBlock;
imageBgTransparent:uiBlock;
photoEditorBg:uiOverlay;
photoEditorItemBaseHandleFg:uiOverlayText;
photoCropFadeBg:uiOverlay;
photoCropPointFg:uiOverlayText;
topBarBg:uiBg;
spellUnderline:uiError;
layerBg:uiOverlay;
outdatedBg:uiError;
outdateSoonBg:uiError;
outdatedFg:uiAccentText;
boxDividerBg:uiBlock;
boxDividerFg:uiLines;
activeButtonBg:uiAccent;
activeButtonFg:uiAccentText;
activeButtonBgOver:{C['surf3']};
activeButtonBgRipple:uiAccent;
activeButtonFgOver:uiAccentText;
activeButtonSecondaryFg:uiAccentText;
activeButtonSecondaryFgOver:uiAccentText;
lightButtonBg:uiBlock;
lightButtonFg:uiText;
lightButtonBgOver:uiBlockActive;
lightButtonBgRipple:uiBlockActive;
lightButtonFgOver:uiText;
attentionButtonFg:uiError;
attentionButtonFgOver:uiOverlayText;
attentionButtonBgOver:uiError;
attentionButtonBgRipple:uiError;
msgInBg:uiMsg;
msgOutBg:{C['msg_out']};
msgInShadow:uiMsg;
msgOutShadow:{C['msg_out']};
msgInBgSelected:{C['surf3']};
msgOutBgSelected:uiBlockActive;
msgInShadowSelected:uiAccent;
msgOutShadowSelected:uiAccent;
historyTextInFg:uiText;
historyTextInFgSelected:uiText;
historyTextOutFg:uiText;
historyTextOutFgSelected:uiText;
historyLinkInFg:uiAccent;
historyLinkInFgSelected:uiAccent;
historyLinkOutFg:uiAccent;
historyLinkOutFgSelected:uiAccent;
msgServiceBg:{C['btn']};
msgServiceBgSelected:{C['btn_hover']};
msgServiceFg:uiText;
msgInServiceFg:uiAccent;
msgOutServiceFg:uiAccent;
msgInDateFg:uiTextSilver;
msgOutDateFg:uiTextSilver;
msgSelectOverlay:{C['accent']}40;
msgInReplyBarColor:uiAccent;
msgInReplyBarSelColor:uiAccent;
msgOutReplyBarColor:uiAccent;
msgOutReplyBarSelColor:uiAccent;
msgImgReplyBarColor:uiAccent;
dialogsBg:uiBg;
dialogsBgOver:uiBlock;
dialogsBgActive:uiBlockActive;
dialogsRippleBg:uiBlock;
dialogsRippleBgActive:uiBlockActive;
dialogsUnreadBg:uiAccent;
dialogsUnreadFg:uiAccentText;
dialogsUnreadBgOver:uiBlockActive;
dialogsUnreadFgOver:uiText;
dialogsUnreadBgMuted:uiLines;
dialogsUnreadBgActive:uiAccent;
dialogsUnreadFgActive:uiAccentText;
dialogsMenuIconFg:uiIcons;
dialogsMenuIconFgOver:uiText;
dialogsDateFg:uiTextSilver;
dialogsNameFg:uiText;
dialogsNameFgOver:uiText;
dialogsNameFgActive:uiText;
dialogsTextFg:uiTextSilver;
dialogsTextFgOver:uiText;
dialogsTextFgActive:uiText;
dialogsTextFgService:uiAccent;
dialogsChatIconFg:uiIcons;
dialogsChatIconFgOver:uiText;
dialogsSentIconFg:uiAccent;
dialogsDraftFg:uiError;
dialogsArchiveFg:uiTextSilver;
dialogsOnlineBadgeFg:uiSuccess;
dialogsVerifiedIconBg:uiAccent;
dialogsVerifiedIconFg:uiAccentText;
sideBarBg:uiBlock;
sideBarTextFg:uiText;
sideBarIconFg:uiIcons;
sideBarBgRipple:uiBlockActive;
sideBarBadgeBg:uiAccent;
sideBarBadgeFg:uiAccentText;
titleBg:uiBlock;
titleFg:uiTextSilver;
titleBgActive:uiBlock;
titleFgActive:uiText;
titleButtonFg:uiIcons;
titleButtonBgOver:uiBlockActive;
titleButtonFgOver:uiText;
titleButtonCloseBgOver:uiError;
titleButtonCloseFgOver:uiOverlayText;
menuBg:uiBlock;
menuBgOver:uiBlockActive;
menuBgRipple:uiBlockActive;
menuIconFg:uiIcons;
menuIconFgOver:uiText;
menuFgDisabled:uiLines;
menuSeparatorFg:uiLines;
emojiPanBg:uiBg;
emojiPanCategories:uiBg;
emojiIconFg:uiIcons;
emojiIconFgActive:uiAccent;
tooltipBg:uiBlock;
tooltipFg:uiText;
tooltipBorderFg:uiLines;
toastBg:uiOverlay;
toastFg:uiOverlayText;
importantTooltipBg:uiOverlay;
importantTooltipFg:uiOverlayText;
scrollBg:transparent;
historyScrollBg:transparent;
scrollBgOver:uiLines;
scrollBarBg:uiBlockActive;
historyScrollBarBg:uiBlockActive;
scrollBarBgOver:uiAccent;
mediaPlayerBg:uiBg;
mediaPlayerActiveFg:uiAccent;
mediaPlayerInactiveFg:uiLines;
msgFileInBg:uiBlockActive;
msgFileOutBg:uiBlockActive;
callBg:uiBg;
callArrowFg:uiSuccess;
callArrowMissedFg:uiError;
callAnswerBg:uiSuccess;
callHangupBg:uiError;
callNameFg:uiText;
callStatusFg:uiTextSilver;
historyOutIconFg:uiAccent;
historyOutIconFgSelected:uiAccent;
historySendingOutIconFg:uiLines;
historySendingInIconFg:uiLines;
historyPeer1NameFg:{C['nick1']};
historyPeer2NameFg:{C['nick2']};
historyPeer3NameFg:{C['nick3']};
historyPeer4NameFg:{C['nick4']};
historyPeer5NameFg:{C['nick5']};
historyPeer6NameFg:{C['nick6']};
historyPeer7NameFg:{C['nick1']};
historyPeer8NameFg:{C['nick2']};
historyPeer1NameFgSelected:uiText;
historyPeer2NameFgSelected:uiText;
historyPeer3NameFgSelected:uiText;
historyPeer4NameFgSelected:uiText;
historyPeer5NameFgSelected:uiText;
historyPeer6NameFgSelected:uiText;
historyPeer7NameFgSelected:uiText;
historyPeer8NameFgSelected:uiText;
historyComposeAreaBg:uiMsg;
historyComposeAreaFg:uiText;
historyComposeIconFg:uiIcons;
historyComposeButtonBg:{C['btn']};
historyComposeButtonFg:uiText;
historyComposeButtonBgOver:{C['btn_hover']};
historyComposeButtonBgRipple:{C['btn_hover']};
historyComposeIconFgOver:uiText;
historySendIconFg:uiAccent;
historySendIconFgOver:uiText;
historyPinnedBg:uiMsg;
historyTextFg:uiText;
fieldHelperFg:uiSubtext;
msgBotKbBg:{C['btn']};
msgBotKbIconFg:uiText;
msgBotKbRippleBg:{C['faint']}55;
msgBotKbOverBgAdd:{C['surf3']};
dialogsUnreadCounterBg:uiAccent;
dialogsUnreadCounterFg:uiAccentText;
sidebarBg:uiMsg;
sidebarBgOver:uiBlockActive;
sidebarIconFg:uiIcons;
sidebarTextFg:uiText;
sidebarActiveBg:uiAccent;
sidebarActiveFg:uiAccentText;
defaultBg:uiBg;
defaultPatternOpacity:30;
"""

    os.makedirs(os.path.dirname(THEME_FILE), exist_ok=True)
    with open(THEME_FILE, "w") as f:
        f.write(theme)

    wallpaper = d.get("wallpaper")
    if wallpaper and os.path.isfile(wallpaper):
        with zipfile.ZipFile(THEME_ZIP, "w", zipfile.ZIP_STORED) as z:
            z.writestr("colors.tdesktop-theme", theme)
            z.write(wallpaper, "background.jpg")
        print("success", THEME_ZIP)
    else:
        print("wallpaper not foud ", THEME_ZIP)


if __name__ == "__main__":
    main()
