#include <stdio.h>
#include "textgfx.h"
#include "terminal.h"

void textgfx_init()
{
	hide_cursor();
}

void textgfx_end()
{
	set_text_attr(0);
	moveto(textgfx.x0, textgfx.y0 + textgfx.lines);
	show_cursor();
}

void update_screen()
{
	fflush(stdout);
}
