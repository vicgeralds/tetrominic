#include <stdio.h>
#include "terminal.h"

void set_latin1_codepage()
{
	const char *s = latin1_ascii;
	switch (terminal.charenc) {
	case CHAR_ASCII:
		break;
	case CHAR_CP850:
		s = latin1_cp850;
		break;
	case CHAR_CP437:
		s = latin1_cp437;
		break;
	default:
		return;
	}
	set_latin1_table(s);
	replace_codepage_c0();
}

static void puttext_utf8(const char *s)
{
	char buf[80];
	char *p;
	while (*s) {
		p = buf;
		while (*s && p < buf+sizeof(buf)-2) {
			p += setc_utf8(p, (unsigned char) *s);
			s++;
		}
		*p = '\0';
		tputstr(buf);
	}
}

static void puttext(const char *s)
{
	char buf[80];
	while (*s) {
		s += convert_chars(s, buf, sizeof(buf));
		tputstr(buf);
	}
}

void tputtext(const char *s)
{
	if (terminal.charenc == CHAR_ISO8859_1 || isasciistr(s))
		tputstr(s);
	else if (terminal.charenc == CHAR_UTF8)
		puttext_utf8(s);
	else
		puttext(s);
}

void tputchar(int c)
{
	char s[4] = "\xBF";
	if (c < 256)
		s[0] = c;
	else if (terminal.charenc == CHAR_UTF8 && c <= 0xffff) {
		setc_utf8(s, c);
		tputstr(s);
		return;
	}
	tputtext(s);
}
