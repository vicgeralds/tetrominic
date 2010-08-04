#include "textgfx.h"
#include "terminal.h"

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

void moveto(int x, int y)
{
	const int y0 = textgfx.y0,
	       lines = textgfx.lines,
	           h = terminal.height;
	int dx, dy;

	if (y-y0+1 > lines) {
		textgfx.lines = y-y0+1;
		nlto(y);
	} else if (y-y0+h < lines) {
		textgfx.lines = h;
		y = y0;
	}

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

void setattr(int code)
{
	tprintf("\033[%dm", code);
}

void normalattr()
{
	tputstr("\033[m");
}

void set_sgr_params(unsigned params)
{
	int x = params & 0x0f,
	    y = (params >> 4) & 0x3f,
	    z = (params >> 10);

	if (z)
		tprintf("\033[%d;%d;%dm", x, y, z);
	else if (y)
		tprintf("\033[%d;%dm", x, y);
	else
		setattr(x);
}
