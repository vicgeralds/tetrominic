/* ANSI escape sequences */

#include "textgfx.h"
#include "terminal.h"

static int ansi_attr;

static void nlto(int y)
{
	int dy = y - textgfx.y;
	if (dy > 0) {
		for (; dy > 0; dy--)
			tputstr("\n");
		textgfx.x = textgfx.x0;
		textgfx.y = y;
	}
}

/* heuristic to determine visible lines in use */
static void grow_lines(int y)
{
	int y0  = textgfx.y0;
	int top = y - y0;
	int h   = terminal.height;
		
	if (top >= textgfx.lines) {
		textgfx.lines = top+1;
		nlto(y);
	}
	if (top + h < textgfx.lines) {
		textgfx.lines = h;
		tputstr("\033[H");	/* cursor home */
		textgfx.x = textgfx.x0;
		textgfx.y = y0;
	}
}

void moveto(int x, int y)
{
	int dx, dy;

	if (y != textgfx.y)
		grow_lines(y);

	dx = x - textgfx.x;
	dy = y - textgfx.y;
	textgfx.x = x;
	textgfx.y = y;

	if (dy) {
		if (dy < 0)
			tprintf("\033[%dA", -dy);
		else
			tprintf("\033[%dB", dy);
	}
	if (dx) {
		if (dx > 0)
			tprintf("\033[%dC", dx);
		else
			tprintf("\033[%dD", -dx);
	}
}

void cleartoeol()
{
	tputstr("\033[K");
}

void clearscreen()
{
	moveto(textgfx.x0, textgfx.y0);
	tputstr("\033[J");
	textgfx.lines = 1;
}

void set_text_attr(int attr)
{
	char sgr[20] = "\033[";
	char *p      = sgr + 1;
	int  toggle  = attr ^ ansi_attr;

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
		tputstr(sgr);
	}
}

void normalattr()
{
	ansi_attr = 0;
	tputstr("\033[m");
}
