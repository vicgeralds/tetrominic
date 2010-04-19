#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "terminal.h"

static Display *dpy;
static Window xterm_win;
static Window toplevel_win;

static Window find_toplevel(Window w)
{
	Window root, par, *children;
	unsigned n;
	if (XQueryTree(dpy, w, &root, &par, &children, &n)) {
		if (children)
			XFree(children);
		if (par != root)
			return find_toplevel(par);
	}
	return w;
}

static int has_focus()
{
	Window w;
	int revert;
	return XGetInputFocus(dpy, &w, &revert) &&
		(w == xterm_win || w == PointerRoot ||
		 w != None && find_toplevel(w) == toplevel_win);
}

static int yes_focus()
{
	if (has_focus())
		terminal.has_focus = has_focus;
	return 1;
}

static int error_handler(Display *d, XErrorEvent *e) { return 0; }

void xwindow_init()
{
	XClassHint wmclass;
	char *s = getenv("WINDOWID");
	if (s && sscanf(s, "%lu", &xterm_win)==1 && (dpy=XOpenDisplay(0))) {

		XSetErrorHandler(error_handler);

		toplevel_win = find_toplevel(xterm_win);
		terminal.has_focus = yes_focus;

		if (XGetClassHint(dpy, xterm_win, &wmclass)) {
			terminal.wm_class[0] = wmclass.res_name;
			terminal.wm_class[1] = wmclass.res_class;
		}
	}
}

void xwindow_exit()
{
	int i;
	if (dpy) {
		for (i=0; i<2; i++) {
			if (terminal.wm_class[i] && *terminal.wm_class[i]) {
				XFree(terminal.wm_class[i]);
				terminal.wm_class[i] = "";
			}
		}
		XCloseDisplay(dpy);
		dpy = NULL;
	}
}
