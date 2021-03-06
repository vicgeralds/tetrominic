#include <string.h>
#include "terminal.h"

int centre_text(const char *s, int w, int x, int y)
{
	char buf[80] = " ";
	int len = strcspn(strncat(buf, s, 72), "\n\t");
		/* append at most 72 characters, until newline or tab */

	buf[len++] = ' ';
	buf[len] = '\0';

	if (w - len > 0)
		x += (w - len)/2;

	moveto(x, y);
	terminal.puttext(buf);
	terminal.cursor_x += len;

	return len - 2;
}
