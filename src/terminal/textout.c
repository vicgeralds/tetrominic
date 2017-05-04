#include <stdlib.h>
#include <wchar.h>
#include "terminal.h"

static wchar_t cp437_unicode[128] = L""
	"\u00c7\u00fc\u00e9\u00e2\u00e4\u00e0\u00e5\u00e7"
	"\u00ea\u00eb\u00e8\u00ef\u00ee\u00ec\u00c4\u00c5"
	"\u00c9\u00e6\u00c6\u00f4\u00f6\u00f2\u00fb\u00f9"
	"\u00ff\u00d6\u00dc\u00a2\u00a3\u00a5\u20a7\u0192"
	"\u00e1\u00ed\u00f3\u00fa\u00f1\u00d1\u00aa\u00ba"
	"\u00bf\u2310\u00ac\u00bd\u00bc\u00a1\u00ab\u00bb"
	"\u2591\u2592\u2593\u2502\u2524\u2561\u2562\u2556"
	"\u2555\u2563\u2551\u2557\u255d\u255c\u255b\u2510"
	"\u2514\u2534\u252c\u251c\u2500\u253c\u255e\u255f"
	"\u255a\u2554\u2569\u2566\u2560\u2550\u256c\u2567"
	"\u2568\u2564\u2565\u2559\u2558\u2552\u2553\u256b"
	"\u256a\u2518\u250c\u2588\u2584\u258c\u2590\u2580"
	"\u03b1\u00df\u0393\u03c0\u03a3\u03c3\u00b5\u03c4"
	"\u03a6\u0398\u03a9\u03b4\u221e\u03c6\u03b5\u2229"
	"\u2261\u00b1\u2265\u2264\u2320\u2321\u00f7\u2248"
	"\u00b0\u2219\u00b7\u221a\u207f\u00b2\u25a0\u00a0";

/* convert character from code page 437 to a wide character */
static wint_t towc(int c)
{
	if ((unsigned char) c < 128)
		return btowc(c);
	if ((unsigned char) c - 128 < 128)
		return cp437_unicode[(unsigned char) c - 128];

	return WEOF;
}

/* convert string from code page 437 using the current locale */
static int convert_text(const char *s, char *buf, size_t n)
{
	int i = 0;
	size_t mblen = MB_CUR_MAX;

	while (n > mblen && s[i]) {
		wint_t c = towc(s[i]);

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

void tputtext(const char *s)
{
	char buf[20];

	while (*s) {
		s += convert_text(s, buf, sizeof(buf));
		tputstr(buf);
	}
}
