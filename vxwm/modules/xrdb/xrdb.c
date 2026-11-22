void
loadxrdb()
{
  Display *display;
  char * resm;
  XrmDatabase xrdb;
  char *type;
  XrmValue value;

  display = XOpenDisplay(NULL);

  if (display != NULL) {
    resm = XResourceManagerString(display);

    if (resm != NULL) {
      xrdb = XrmGetStringDatabase(resm);

      if (xrdb != NULL) {
	XRDB_LOAD_COLOR("dwm.color5",  normbordercolor);
	XRDB_LOAD_COLOR("dwm.color1",  selbordercolor);
	XRDB_LOAD_COLOR("dwm.color0",  normbgcolor);
	XRDB_LOAD_COLOR("dwm.color7",  normfgcolor);
	XRDB_LOAD_COLOR("dwm.color0",  selfgcolor);
	XRDB_LOAD_COLOR("dwm.color1",  selbgcolor);
      }
    }
  }

  XCloseDisplay(display);
}

void
xrdb(const Arg *arg)
{
  loadxrdb();
  int i;
  for (i = 0; i < LENGTH(colors); i++)
                scheme[i] = drw_scm_create(drw, colors[i], 3);
  focus(NULL);
  arrange(NULL);
}

