#include <stdio.h>
#include <stdarg.h>
#include "terminal.h"

/* select the IBM PC alternate character set.
 * This works in the linux console and makes it possible to switch to VT100
 * graphics even in utf8 mode.
 */
#define SELECT_ALT_FONT	     "\033[11m"
#define SELECT_PRIMARY_FONT  "\033[10m"

/* select VT100 graphics mapping */
#define SELECT_G0_VT100      "\033(0"
#define SELECT_G0_DEFAULT    "\033(B"

struct terminal terminal = {80, 25};

static void hide_cursor() { tputstr("\033[?25l"); }
static void show_cursor() { tputstr("\033[?25h"); }

static int yes() { return 1; }

void init_terminal()
{
	terminal.out = stdout;
	terminal.wm_class[0] = "";
	terminal.wm_class[1] = "";
	terminal.hide_cursor = hide_cursor;
	terminal.show_cursor = show_cursor;
	terminal.has_focus = yes;
}

void tputstr(const char *s)
{
	fputs(s, terminal.out);
}

int tprintf(const char *format, ...)
{
	va_list ap;
	int n;
	va_start(ap, format);
	n = vfprintf(terminal.out, format, ap);
	va_end(ap);
	return n;
}

void tputacs(const char *s)
{
	tputstr(SELECT_ALT_FONT SELECT_G0_VT100);
	tputstr(s);
	tputstr(SELECT_G0_DEFAULT SELECT_PRIMARY_FONT);
}
