#include <string.h>
#include "acstext.h"
#include "terminal.h"

/* Alternate character set mapping to IBM graphics */

static int acs_to_text(int c)
{
	switch (c) {
	case 'a': return TEXT_MEDIUM_SHADE;
	case 'h': return TEXT_LIGHT_SHADE;
	case 'j': return TEXT_LOWER_RIGHT_CORNER;
	case 'k': return TEXT_UPPER_RIGHT_CORNER;
	case 'l': return TEXT_UPPER_LEFT_CORNER;
	case 'm': return TEXT_LOWER_LEFT_CORNER;
	case 'n': return TEXT_CROSS;
	case 'q': return TEXT_HORIZONTAL_LINE;
	case 't': return TEXT_LEFT_T;
	case 'u': return TEXT_RIGHT_T;
	case 'v': return TEXT_BOTTOM_T;
	case 'w': return TEXT_TOP_T;
	case 'x': return TEXT_VERTICAL_LINE;
	}
	return c;
}

static int convert_chars(const char *s, char *buf, size_t n)
{
	int i = 0;

	for (; s[i] && i < n - 1; i++) {
		buf[i] = acs_to_text(s[i]);
	}

	buf[i] = '\0';

	return i;
}

void putacs_text(const char *s)
{
	char buf[20];

	while (*s) {
		s += convert_chars(s, buf, sizeof(buf));
		terminal.puttext(buf);
	}
}
