#include <string.h>
#include "textgfx.h"

/* Alternate character set mapping to IBM graphics */

static const char acs_vt100[] = "`a+,-.fgh0jklmnopqrstuvwxyz{|}~";

static const char acs_cp437[] = {
	TEXT_DIAMOND,
	TEXT_MEDIUM_SHADE,
	TEXT_RIGHT_ARROW,
	TEXT_LEFT_ARROW,
	TEXT_UP_ARROW,
	TEXT_DOWN_ARROW,
	TEXT_DEGREE_SYMBOL,
	TEXT_PLUS_MINUS,
	TEXT_LIGHT_SHADE,
	TEXT_SOLID_BLOCK,
	TEXT_LOWER_RIGHT_CORNER,
	TEXT_UPPER_RIGHT_CORNER,
	TEXT_UPPER_LEFT_CORNER,
	TEXT_LOWER_LEFT_CORNER,
	TEXT_CROSS,
	TEXT_SCAN_LINE_1,
	TEXT_SCAN_LINE_3,
	TEXT_HORIZONTAL_LINE,
	TEXT_SCAN_LINE_7,
	TEXT_SCAN_LINE_9,
	TEXT_LEFT_T,
	TEXT_RIGHT_T,
	TEXT_BOTTOM_T,
	TEXT_TOP_T,
	TEXT_VERTICAL_LINE,
	TEXT_LESS_THAN_OR_EQUAL,
	TEXT_GREATER_THAN_OR_EQUAL,
	TEXT_PI,
	TEXT_NOT_EQUAL,
	TEXT_POUND_STERLING,
	TEXT_MID_DOT,
	'\0'
};

static int acs_to_text(int c)
{
	const char *p = acs_vt100 + (c & 31);

	if (c != *p) {
		p = strchr(acs_vt100, c);
	}

	return p ? acs_cp437[p - acs_vt100] : c;
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
		textgfx.puttext(buf);
	}
}
