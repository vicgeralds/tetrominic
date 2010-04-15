#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#if HAVE_GETTEXTINFO
#include <conio.h>
#elif UNIX
#include <unistd.h>
#include <sys/ioctl.h>
#endif
#include "terminal.h"

static int gettermsize_unix()
{
#if defined(TIOCGSIZE)
	struct ttysize tty;
	if (!ioctl(STDIN_FILENO, TIOCGSIZE, &tty)) {
		terminal.width  = tty.ts_cols;
		terminal.height = tty.ts_lines;
		return 1;
	}
#elif defined(TIOCGWINSZ)
	struct winsize win;
	if (!ioctl(STDIN_FILENO, TIOCGWINSZ, &win)) {
		terminal.width  = win.ws_col;
		terminal.height = win.ws_row;
		return 1;
	}
#endif
	return 0;
}

static int getenv_number(const char *name)
{
	char *s = getenv(name);
	return s ? atoi(s) : 0;
}

void gettermsize()
{
	int x;
#if HAVE_GETTEXTINFO
	struct text_info text;
	gettextinfo(&text);
	terminal.width  = text.screenwidth;
	terminal.height = text.screenheight;
#endif
	if (!gettermsize_unix()) {
		x = getenv_number("COLUMNS");
		if (x > 0)
			terminal.width = x;

		x = getenv_number("LINES");
		if (x > 0)
			terminal.height = x;
	}
}
