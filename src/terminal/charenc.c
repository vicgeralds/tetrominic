#include <string.h>
#include "terminal.h"

/* ACS LINE DRAWING (alternate character set)

   VT100 symbols and others

	a  medium shade         	x  vertical line            
	h  light shade          	y  less than or equal
	j  lower right corner   	z  greater than or equal
	k  upper right corner   	{  pi
	l  upper left corner    	|  not equal
	m  lower left corner    	}  pound sterling
	n  cross                	~  mid dot (bullet)
	o  scan line 1          	`  diamond
	p  scan line 3          	f  degree symbol
	q  horizontal line      	g  plus-minus
	r  scan line 7          	+  right arrow
	s  scan line 9          	,  left arrow
	t  left T               	-  up arrow
	u  right T              	.  down arrow
	v  bottom T             	0  solid block
	w  top T						*/

const char acs_vt100[] = "ahjklmnopqrstuvwxyz{|}~`fg+,-.0";
const char acs_ascii[] = "# +++++^---_++++|?????.+^?><^v#";

/* codepage 437 (IBM graphics) */
const char acs_cp437[] = "\xB1\xB0\xD9\xBF\xDA\xC0\xC5\x5E"
			 "\xC4\xC4\xC4\x5F\xC3\xB4\xC1\xC2"
			 "\xB3\xF3\xF2\xE3\xA8\x9C\xFA\x04"
			 "\xF8\xF1\x1A\x1B\x18\x19\xDB";
/* unicode box drawing */
const unsigned short acs_unicode[] = {
	0x2592, 0x2591, 0x2518, 0x2510, 0x250C, 0x2514, 0x253C, 0x00AF,
	0x2500, 0x2500, 0x2500, 0x005F, 0x251C, 0x2524, 0x2534, 0x252C,
	0x2502, 0x2264, 0x2265, 0x03C0, 0x2260, 0x00A3, 0x00B7, 0x25C6,
	0x00B0, 0x00B1, 0x2192, 0x2190, 0x2191, 0x2193, 0x2588
};

/* Latin-1 Supplement A0-FF */

const char latin1_ascii[96] =
	" !????|?:?a\"~-?^^?23'u?.?1o\"????"
	"AAAAAAACEEEEIIIIDNOOOOOxOUUUUY??"
	"aaaaaaaceeeeiiiidnooooo/ouuuuy?y";

const char latin1_cp850[96] =
	"\xFF\xAD\xBD\x9C\xCF\xBE\xDD\xF5\xF9\xB8\xA6\xAE\xAA\xF0\xA9\xEE"
	"\xF8\xF1\xFD\xFC\xEF\xE6\xF4\xFA\xF7\xFB\xA7\xAF\xAC\xAB\xF3\xA8"
	"\xB7\xB5\xB6\xC7\x8E\x8F\x92\x80\xD4\x90\xD2\xD3\xDE\xD6\xD7\xD8"
	"\xD1\xA5\xE3\xE0\xE2\xE5\x99\x9E\x9D\xEB\xE9\xEA\x9A\xED\xE8\xE1"
	"\x85\xA0\x83\xC6\x84\x86\x91\x87\x8A\x82\x88\x89\x8D\xA1\x8C\x8B"
	"\xD0\xA4\x95\xA2\x93\xE4\x94\xF6\x9B\x97\xA3\x96\x81\xEC\xE7\x98";

const char latin1_cp437[96] =
	"\xFF\xAD\x9B\x9C\xA8\x9D\x7C\x15\x3A\xA8\xA6\xAE\xAA\x2D\xA8\x5E"
	"\xF8\xF1\xFD\x33\x27\xE6\x14\xFA\xA8\x31\xA7\xAF\xAC\xAB\xA8\xA8"
	"\x41\x41\x41\x41\x8E\x8F\x92\x80\x45\x90\x45\x45\x49\x49\x49\x49"
	"\x44\xA5\x4F\x4F\x4F\x4F\x99\x78\x4F\x55\x55\x55\x9A\x59\xA8\xE1"
	"\x85\xA0\x83\x61\x84\x86\x91\x87\x8A\x82\x88\x89\x8D\xA1\x8C\x8B"
	"\xEB\xA4\x95\xA2\x93\x6F\x94\xF6\xED\x97\xA3\x96\x81\x79\xA8\x98";

static union {
	char chars[256];
	unsigned short uchars[128];
} table;

void set_char_table(const char *set1, const char *set2)
{
	int i, j;
	for (i=0; (j= (unsigned char) set1[i]); i++)
		table.chars[j] = set2[i];
}

void set_acs_unicode_table(const char *set1, const unsigned short *set2)
{
	int i, j;
	for (i=0; (j= (unsigned char) set1[i]); i++) {
		if (j < 128)
			table.uchars[j] = set2[i];
	}
}

void set_latin1_table(const char *s)
{
	memcpy(table.chars+0xA0, s, 96);
}

void replace_codepage_c0()
{
	int i, c;
	for (i=32; i<256; i++) {
		c = (unsigned char) table.chars[i];
		if (c > 0 && c < 32)
			table.chars[i] = '^';
	}
}

int convert_chars(const char *s, char *buf, int n)
{
	return convert_chars_hi(s, buf, n, 0);
}

int convert_chars_hi(const char *s, char *buf, int n, unsigned uc_min)
{
	char *p    = buf;
	char *nulp = buf + n - 1;
	int i;
	for (; *s && p < nulp; s++, p++) {
		i = (unsigned char) *s;
		if (i < uc_min)
			*p = *s;
		else {
			if (!table.chars[i])
				table.chars[i] = *s;
			*p = table.chars[i];
		}
	}
	*p = '\0';
	return p - buf;
}

int acs_to_unicode(int c)
{
	return c > 0 && c < 128 && table.uchars[c] ? table.uchars[c] : c;

}

/* Encode UTF-8 */

static unsigned shift_utf8(char *s, unsigned uc)
{
	*s = 0x80 | (uc & 0x3f);
	return uc >> 6;
}

int setc_utf8(char *s, unsigned uc)
{
	unsigned ones = 0xc0;
	int n = 2;

	if (uc < 0x80) {
		s[0] = uc;
		return 1;
	}
	if (uc >= 0x800) {
		ones = 0xe0;
		n = 3;
		/*
		if (uc > 0xffff) {
			ones = 0xf0;
			n = 4;
			uc = shift_utf8(s+3, uc);
		}
		*/
		uc = shift_utf8(s+2, uc);
	}
	s[0] = ones | shift_utf8(s+1, uc);
	return n;
}
