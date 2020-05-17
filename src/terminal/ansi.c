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
		set_text_attr(0);
		for (; dy > 0; dy--)
			putchar('\n');
		terminal.cursor_x = terminal.x0;
		terminal.cursor_y = y;
	}
}

/* Update visible lines in use before moving the cursor to line y */
static void grow_lines(int y)
{
	int y0  = terminal.y0;
	int top = y - y0;

	if (top >= terminal.lines) {
		terminal.lines = top+1;
		/* Print newlines if needed */
		nlto(y);
	}
}

void moveto(int x, int y)
{
	int dx, dy;

	if (terminal.lines > terminal.height) {
		terminal.y0 += terminal.lines - terminal.height;
		terminal.lines = terminal.height;
	}

	if (y > terminal.cursor_y) {
		grow_lines(y);
	}

	if (y < terminal.y0) y = terminal.y0;

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
	static int saved_lines;

	if (terminal.y0 > 0 || terminal.lines == 1) {
		terminal.lines = terminal.y0 + (
			saved_lines > terminal.lines ? saved_lines : terminal.lines
		);
		terminal.y0 = 0;
	}
        if (terminal.lines >= terminal.height) {
		/* Move cursor to upper left corner */
		fputs(CSI "H", stdout);
		terminal.cursor_x = terminal.x0;
		terminal.cursor_y = terminal.lines - terminal.height;
	}
	moveto(terminal.x0, terminal.y0);
	fputs(CSI "J", stdout);
	saved_lines = terminal.lines;
	terminal.lines = 1;
}

void flush_output()
{
	if (terminal.cursor_x > terminal.x0) {
		if (terminal.cursor_y + 1 >= terminal.y0 + terminal.lines) {
			fputs(CSI "A", stdout);
			terminal.cursor_y--;
		}
		/* flush with a newline */
		nlto(terminal.cursor_y + 1);
	}

	fflush(stdout);
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

	/* any attributes off? */
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
	if ((ansi_attr | ALTCHARSET | VT100_GRAPHICS) != ansi_attr) {
		/* Select VT100 graphics mapping */
		puttext_ibm(ESC "(0");
		ansi_attr |= ALTCHARSET | VT100_GRAPHICS;
	}

	fputs(s, stdout);
}

void puttext_ibm(const char *s)
{
	if (!(ansi_attr & ALTCHARSET)) {
		/* Select the IBM PC alternate character set.
		 * This works in the linux console and makes it possible to
		 * switch to VT100 graphics even in utf8 mode.
		 */
		fputs(CSI "11m", stdout);
		ansi_attr |= ALTCHARSET;
	}

	fputs(s, stdout);
}
