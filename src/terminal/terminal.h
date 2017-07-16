/* Text terminal interface */

#ifndef terminal_h
#define terminal_h

#include <stdio.h>

#define XTERM      1
#define LINUX_TERM 2
#define GNOME_TERM 4
#define CYGWIN     8

struct terminal {
	unsigned short width;
	unsigned short height;
	unsigned short type;
	FILE *out;
	char *wm_class[2];		/* window name and class */
	void (*hide_cursor)();
	void (*show_cursor)();
	int  (*has_focus)();
};

extern struct terminal terminal;

/* init terminal structure */
void init_terminal();

void gettermsize();
void gettermtype();

/* connect to X and get focus detection and window info:
	wm_class[2]	e.g. {"xterm", "XTerm"}
	has_focus()	returns 0 if top-level window loses focus
*/
void xwindow_init();
void xwindow_exit();

/* terminal non-canonical mode */
void set_input_mode();
void restore_input_mode();

/* simple output wrappers */
void tputstr(const char *s);
int  tprintf(const char *format, ...);

/* use the alt char set (line drawing) */
void tputacs(const char *s);

/* put cp437 string using the current locale */
void tputtext(const char *s);

#endif
