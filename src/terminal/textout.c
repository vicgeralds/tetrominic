#include <stdlib.h>
#include <wchar.h>
#include <stdio.h>
#include "terminal.h"

/* convert character from code page 437 to unicode */
static wint_t to_wide(int c)
{
	/* box drawing and block characters (U+25xx) */
	static unsigned char extended_b0[48] = {
		0x91, 0x92, 0x93, 0x02, 0x24, 0x61, 0x62, 0x56,
		0x55, 0x63, 0x51, 0x57, 0x5d, 0x5c, 0x5b, 0x10,
		0x14, 0x34, 0x2c, 0x1c, 0x00, 0x3c, 0x5e, 0x5f,
		0x5a, 0x54, 0x69, 0x66, 0x60, 0x50, 0x6c, 0x67,
		0x68, 0x64, 0x65, 0x59, 0x58, 0x52, 0x53, 0x6b,
		0x6a, 0x18, 0x0c, 0x88, 0x84, 0x8c, 0x90, 0x80
	};

	int codepoint = (unsigned char) c;

	if (codepoint < 128)
		return codepoint;

	if (codepoint >= 0xb0 && codepoint < 0xe0)
		return extended_b0[codepoint - 0xb0] | 0x2500;

	return WEOF;
}

/* convert string from code page 437 using the current locale */
static int convert_text(const char *s, char *buf, size_t n)
{
	int i = 0;
	size_t mblen = MB_CUR_MAX;

	while (n > mblen && s[i]) {
		wint_t c = to_wide(s[i]);

		n -= mblen;

		if (c == WEOF || (mblen = wcrtomb(buf, c, NULL)) == -1) {
			/* restore initial shift state */
			mblen = wcrtomb(buf, L'\0', NULL);
			buf[mblen] = '?';
		}

		i++;
		buf += mblen;
	}

	*buf = '\0';

	return i;
}

void puttext_unicode(const char *s)
{
	char buf[20];

	while (*s) {
		s += convert_text(s, buf, sizeof(buf));
		fputs(buf, stdout);
	}
}
