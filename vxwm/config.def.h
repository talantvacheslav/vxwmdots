#pragma once
/* See LICENSE file for copyright and license details. */

/* appearance */
static const unsigned int borderpx  = 2;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 0;        /* 0 means bottom bar */
static const char *fonts[]          = { "JetBrainsMono Nerd Font:size=12" };
static const char dmenufont[]       = "JetBrainsMono Nerd Font:size=12";

/* colors */
static char normbgcolor[]           = "#222222";
static char normbordercolor[]       = "#444444";
static char normfgcolor[]           = "#bbbbbb";
static char selfgcolor[]            = "#eeeeee";
static char selbordercolor[]        = "#005577";
static char selbgcolor[]            = "#005577";
static char *colors[][3] = {
       /*               fg           bg           border   */
       [SchemeNorm] = { normfgcolor, normbgcolor, normbordercolor },
       [SchemeSel]  = { selfgcolor,  selbgcolor,  selbordercolor  },
};

#if GAPS
static const unsigned int gappx = 5;
#endif

#if BAR_HEIGHT
static const int user_bh = 0;
#endif

#if BAR_PADDING
static const int top_vertpad = 0;          /* top vertical padding of bar */ 
	static const int bottom_vertpad = 0;       /* bottom vertical padding of bar */
	static const int left_sidepad = 500;         /* left horizontal padding of bar */
	static const int right_sidepad = 500;        /* right horizontal padding of bar */

#endif

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7" };

#if OCCUPIED_TAGS_DECORATION
static const char *occupiedtags[] = { "1+", "2+", "3+", "4+", "5+", "6+", "7+"};
#endif

#if INFINITE_TAGS
#define MOVE_CANVAS_STEP 120 /* Defines how many pixel will be jumped when using movecanvas function */
#endif

#if INFINITE_TAGS && IT_SHOW_COORDINATES_IN_BAR
#define COORDINATES_DIVISOR 10 /* Defines by what number coordinates on the bar will be divided, can be used for making numbers smaller which makes navigation easier */
#endif

#if MOVE_RESIZE_WITH_KEYBOARD
#define MOVE_WITH_KEYBOARD_STEP 50 /* Defines by how many pixels windows will be resized with keyboard */
#define RESIZE_WITH_KEYBOARD_STEP 50 /* Defines by how many pixels windows will be resized with keyboard */
#endif

/* vxwm will execute this on startup (can be skipped with -ignoreautostart vxwm flag). */
#if AUTOSTART
static const char *const autostart[] = {
	"sh", "-c", "~/vxwmdots/scripts/zixclip",
	"sh", "-c", "~/vxwmdots/scripts/status.sh",
	"sh", "-c", "~/vxwmdots/scripts/spicetify-watcher.sh",
	"sh", "-c", "",
	
	NULL /* must end with NULL */
};
#endif
static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            1,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */
#if LOCK_MOVE_RESIZE_REFRESH_RATE
static const int refreshrate = 180;  /* refresh rate (per second) for client move/resize, set it to your monitor refresh rate or double of that*/
#endif //LOCK_MOVE_RESIZE_REFRESH_RATE
static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[]=",      tile },    /* first entry is default */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define ALTERNATE_MODKEY Mod1Mask

	
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ Mod1Mask,                     KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "rofi", "-show", "drun", NULL };

static const char *terminal[]  = { "kitty", "fish", NULL };
static const char *filemanager[]  = { "nemo", NULL };
static const char *browser[]  = { "firefox", NULL };
static const char *discord[]  = { "discord", NULL };
static const char *spotify[]  = { "spotify", NULL };
static const char *ayugram[]  = { "AyuGram", NULL };
static const char *clipboard[]  = { "sh", "-c", "~/vxwmdots/scripts/zixclip-rofi.sh", NULL };
static const char *screenshotcmd[]  = { "sh", "-c", "flameshot gui -r -p $HOME/screenshots/ | xclip -selection clipboard -t image/png", NULL };
static const char *rofiwallpaper[] = { "sh", "-c", "~/vxwmdots/scripts/rofi-wallpaper.sh"};
static const char *walr[] = {"sh", "-c", "wal -R"};

#define musicbuttons 1 
static const char *m_playpause[] = {"sh", "-c", "playerctl play-pause "};
static const char *m_next[] = {"sh", "-c", "playerctl next"};
static const char *m_prev[] = {"sh", "-c", "playerctl previous"};

static const Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_r,      spawn,          {.v = dmenucmd } },
	{ MODKEY,                       XK_t,      spawn,          {.v = terminal } },
	{ MODKEY,                       XK_e,      spawn,          {.v = filemanager } },
	{ MODKEY,                       XK_w,      spawn,          {.v = browser } },
	{ MODKEY,                       XK_d,      spawn,          {.v = discord } },
	{ MODKEY,                       XK_a,      spawn,          {.v = spotify } },
	{ MODKEY,                       XK_s,      spawn,          {.v = ayugram } },
	{ MODKEY,                       XK_v,      spawn,          {.v = clipboard } },
	{ MODKEY|ShiftMask,             XK_s,      spawn,          {.v = screenshotcmd } },
	{ MODKEY,			XK_m,      spawn,	   {.v = rofiwallpaper } },
	{ MODKEY|ShiftMask,		XK_m,      spawn,	   {.v = walr } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_o,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_0,      view,           {0} },
	{ MODKEY,                       XK_q,      killclient,     {0} },
	{ MODKEY,                       XK_Left,   focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_Right,  focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_Up,     focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_Down,   focusstack,     {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|Mod1Mask,              XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|Mod1Mask,              XK_period, tagmon,         {.i = +1 } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_e,      quit,           {0} },

#if musicbuttons
	{ MODKEY|ShiftMask,		XK_z,      spawn,	   {.v = m_prev } },
	{ MODKEY|ShiftMask,		XK_x,      spawn,	   {.v = m_playpause } },
	{ MODKEY|ShiftMask,		XK_c,      spawn,	   {.v = m_next } },
#endif
#if FULLSCREEN
	{ MODKEY,                       XK_f,      togglefullscr,  {0} },
#endif
#if XRDB
  { MODKEY,                       XK_F5,     xrdb,           {.v = NULL } },
#endif
#if ENHANCED_TOGGLE_FLOATING
	{ MODKEY|Mod1Mask,              XK_space,  enhancedtogglefloating, {0} },
#endif
#if GAPS
	{ MODKEY,                       XK_minus,  setgaps,        {.i = -1 } },
	{ MODKEY,                       XK_equal,  setgaps,        {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_equal,  setgaps,        {.i = 0  } },
#endif
#if INFINITE_TAGS
  { MODKEY,                       XK_Home,   homecanvas,       {0} }, // Return to x:0, y:0 position
  { MODKEY|ShiftMask,             XK_Left,   movecanvas,       {.i = 0} }, // Move your position to left
  { MODKEY|ShiftMask,             XK_Right,  movecanvas,       {.i = 1} }, // Move your position to right
  { MODKEY|ShiftMask,             XK_Up,     movecanvas,       {.i = 2} }, // Move your position up
  { MODKEY|ShiftMask,             XK_Down,   movecanvas,       {.i = 3} }, // Move your position down
  { MODKEY|ShiftMask,             XK_c,      centerwindow,     {0} },
  { MODKEY|ControlMask,           XK_z,      pinwindow,        {0} },
#endif
#if DIRECTIONAL_FOCUS
	{ ALTERNATE_MODKEY,             XK_Left,   focusdir,       {.i = 0 } }, // left
	{ ALTERNATE_MODKEY,             XK_Right,  focusdir,       {.i = 1 } }, // right
	{ ALTERNATE_MODKEY,             XK_Up,     focusdir,       {.i = 2 } }, // up
	{ ALTERNATE_MODKEY,             XK_Down,   focusdir,       {.i = 3 } }, // down
#endif
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
#if INFINITE_TAGS
 	  { ClkRootWin,           MODKEY|ShiftMask,         Button1,        movecanvasmouse,     {.f = 1.5 } }, 
	  { ClkClientWin,         MODKEY|ShiftMask,         Button1,        movecanvasmouse,     {.f = 1.5 } },
	  { ClkRootWin,           0,                        Button1,        movecanvasmouse,     {.f = 1.5 } },
	  /* .f = 1 is moving multiplier, for example if set to 0.5, canvas will move 2 times slower, if set to 2, canvas will move 2 times faster. 
	     If you want inverted canvas move then set the value to a negative value. */

#endif

	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = terminal } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

