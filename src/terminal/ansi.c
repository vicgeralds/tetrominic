/* ANSI escape sequences */

#include <stdio.h>
#include <string.h>
#include "terminal.h"

#define ESC "\033"
/* Control Sequence Introducer */
#define CSI ESC "["

static int ansi_attr;

static void nlto(int y)
{
	int dy = y - terminal.cursor_y;
	if (dy > 0) {
		for (; dy > 0; dy--)
			putchar('\n');
		terminal.cursor_x = terminal.x0;
		terminal.cursor_y = y;
	}
}

/* heuristic to determine visible lines in use */
static void grow_lines(int y)
{
	int y0  = terminal.y0;
	int top = y - y0;
	int h   = terminal.height;
		
	if (top >= terminal.lines) {
		terminal.lines = top+1;
		nlto(y);
	}
	if (top + h < terminal.lines) {
		terminal.lines = h;
		/* Move cursor to upper left corner */
		fputs(CSI "H", stdout);	/* cursor home */
		terminal.cursor_x = terminal.x0;
		terminal.cursor_y = y0;
	}
}

void moveto(int x, int y)
{
	int dx, dy;

	if (y != terminal.cursor_y)
		grow_lines(y);

	dx = x - terminal.cursor_x;
	dy = y - terminal.cursor_y;
	terminal.cursor_x = x;
	terminal.cursor_y = y;

	if (dy) {
		if (dy < 0)
			printf(CSI "%dA", -dy);
		else
			printf(CSI "%dB", dy);
	}
	if (dx) {
		if (dx > 0)
			printf(CSI "%dC", dx);
		else
			printf(CSI "%dD", -dx);
	}
}

void cleartoeol()
{
	fputs(CSI "K", stdout);
}

void clearscreen()
{
	moveto(terminal.x0, terminal.y0);
	fputs(CSI "J", stdout);
	terminal.lines = 1;
}

void set_text_attr(int attr)
{
	char sgr[20] = CSI;
	char *p;
	int  toggle  = (attr &= (0xFFF | ansi_attr)) ^ ansi_attr;

	if (toggle & VT100_GRAPHICS) {
		/* Select default G0 character set */
		fputs(ESC "(B", stdout);
	}

	if (toggle & ALTCHARSET) {
		/* Select primary font */
		strcpy(sgr, CSI "10;");
	}

	p = sgr + strlen(sgr) - 1;

		/* any attributes off */
	if (toggle & ansi_attr & 0xF88) {
		*++p = '0';
		*++p = ';';
		toggle = attr;
	}
	ansi_attr = attr;

	if (toggle & BOLD)	{ *++p = '1'; *++p = ';'; }
	if (toggle & UNDERLINE) { *++p = '4'; *++p = ';'; }
	if (toggle & BLINK)	{ *++p = '5'; *++p = ';'; }
	if (toggle & STANDOUT)	{ *++p = '7'; *++p = ';'; }

		/* foreground color */
	if ((toggle & 0x0F) + (attr & 8) > 8) {
		*++p = '3';
		*++p = '0' + (attr & 7);
		*++p = ';';
	}
		/* background color */
	if ((toggle & 0xF0) + (attr & 0x80) > 0x80) {
		*++p = '4';
		*++p = '0' + ((attr >> 4) & 7);
		*++p = ';';
	}

	if (*p == ';') {
		*p = 'm';
		fputs(sgr, stdout);
	}
}

void hide_cursor()
{
	fputs(CSI "?25l", stdout);
}

void show_cursor()
{
	fputs(CSI "?25h", stdout);
}

void putacs_vt100(const char *s)
{
	if (!(ansi_attr & ALTCHARSET)) {
		/* Select the IBM PC alternate character set.
		 * This works in the linux console and makes it possible to
		 * switch to VT100 graphics even in utf8 mode.
		 */
		fputs(CSI "11m", stdout);
	}

	if ((ansi_attr | ALTCHARSET | VT100_GRAPHICS) != ansi_attr) {
		ansi_attr |= ALTCHARSET | VT100_GRAPHICS;
		/* Select VT100 graphics mapping */
		fputs(ESC "(0", stdout);
	}

	fputs(s, stdout);
}
