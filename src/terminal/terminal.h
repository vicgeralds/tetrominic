/* Text terminal interface */

#ifndef terminal_h
#define terminal_h

#include <stdio.h>

struct terminal {
	unsigned short width;
	unsigned short height;
	FILE *out;
	void (*hide_cursor)();
	void (*show_cursor)();
	int  (*has_focus)();
};

extern struct terminal terminal;

/* init terminal structure */
void init_terminal();

/* update terminal.width and terminal.height */
void gettermsize();

/* connect to X and get focus detection.
   terminal.has_focus() returns 0 if top-level window loses focus
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
