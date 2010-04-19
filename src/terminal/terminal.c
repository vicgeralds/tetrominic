#include <stdio.h>
#include <stdarg.h>
#include "terminal.h"

struct terminal terminal = {80, 25, 0, CHAR_ASCII, ACS_ASCII, "hoprs0"};

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
