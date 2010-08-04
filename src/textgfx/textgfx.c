#include "textgfx.h"
#include "terminal.h"

struct textgfx textgfx = {0,0,0,0,1};

void textgfx_init()
{
	textgfx.putstr = tputstr;
	textgfx.putacs = tputacs;
	textgfx.puttext = tputtext;
	textgfx.putchar = tputchar;
	terminal.hide_cursor();
}

void textgfx_end()
{
	normalattr();
	moveto(0, textgfx.y0 + textgfx.lines);
	terminal.show_cursor();
}
