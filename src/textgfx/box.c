#include <stdlib.h>
#include <string.h>
#include "textgfx.h"
#include "terminal.h"

#define BUF_SIZE 40

void drawline(int x, int y, int n, const char *chars)
{
	char buf[BUF_SIZE];
	const int m = BUF_SIZE-2;
	int i;

	buf[0] = chars[0];
	for (i=1; i<m; i++)
		buf[i] = chars[1];
	buf[m] = '\0';

	moveto(x, y);
	if (n > 0) {
		for (; n >= BUF_SIZE; n -= m) {
			textgfx.putacs(buf);
			textgfx.x += m;
			buf[0] = chars[1];
		}
		buf[n-1] = chars[2];
		buf[n] = '\0';
		textgfx.putacs(buf);
		textgfx.x += n;
	}
}

void drawbox(int x, int y, int w, int h, const char *chars)
{
	int i;
	if (!chars)
		chars = "lqkx xmqj";
	drawline(x, y, w, chars);
	for (i=1; i<h-1; i++)
		drawline(x, y+i, w, chars+3);
	drawline(x, y+h-1, w, chars+6);
}

void drawframe(int x, int y, int w, int h, const char *title)
{
	int n = strlen(title);
	int m = w-n;
	normalattr();
	setattr(STANDOUT);
	drawline(x, y, m/2, "lq ");
	textgfx.puttext(title);
	drawline((textgfx.x += n), y, m-m/2, " qk");
	normalattr();
	drawbox(x, y+1, w, h-1, "x xx xmqj");
}

void clearbox(int x, int y, int w, int h)
{
	int i;
	normalattr();
	if (w > 0)
		drawbox(x, y, w, h, "         ");
	else {
		for (i=0; i<h; i++) {
			moveto(x, y+i);
			cleartoeol();
		}
	}
}
