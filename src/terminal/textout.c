#include "terminal.h"

void set_latin1_codepage()
{
	switch (terminal.charenc) {
	case CHAR_ASCII:
		set_latin1_table(latin1_ascii);
		break;
	case CHAR_CP850:
		set_latin1_table(latin1_cp850);
		break;
	case CHAR_CP437:
		set_latin1_table(latin1_cp437);
		break;
	default:
		return;
	}
	replace_codepage_c0();
}

static int convert_text(const char *s, char *buf, int n)
{
	if (terminal.charenc == CHAR_UTF8) {
		buf[setc_utf8(buf, (unsigned char) *s)] = '\0';
		return 1;
	}
	return convert_chars_hi(s, buf, n, 0xA0);
}

void tputtext(const char *s)
{
	char buf[80];

	if (terminal.charenc == CHAR_ISO8859_1)
		tputstr(s);
	else {
		while (*s) {
			s += convert_text(s, buf, sizeof(buf));
			tputstr(buf);
		}
	}
}

void tputchar(int c)
{
	char s[4] = "\xBF";	/* upside down question mark */

	if (c < 256)
		s[0] = c;
	else if (terminal.charenc == CHAR_UTF8 && c <= 0xffff) {
		setc_utf8(s, c);
		tputstr(s);
		return;
	}
	tputtext(s);
}
