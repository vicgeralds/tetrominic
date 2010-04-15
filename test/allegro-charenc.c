#include <string.h>
#include <allegro.h>
#include "terminal.h"

#ifndef DATA_PREFIX
#define DATA_PREFIX "../data/"
#endif

#define CP437_FONT "pc8x16.fnt"

static int black;
static int white;

static void puttext(const FONT *f, const char *s, int x, int y)
{
	textout_ex(screen, f, s, x, y, white, black);
}

static void test_unicode_acs(const FONT *f, int c, int c0, int x)
{
	char s[4] = "";
	s[0] = c;
	x += 8*(c-c0);
	puttext(f, s, x, 10);
	if (usetc(s, acs_to_unicode(c)) > 0)
		puttext(f, s, x, 12+text_height(f));
}

static int test_utf8_enc()
{
	char s1[4];
	char s2[4];
	int c, n;
	for (c=0; c <= 0xffff; c++) {
		n = usetc(s1, c);
		s1[n] = '\0';
		n = setc_utf8(s2, c);
		s2[n] = '\0';
		if (strcmp(s1, s2))
			return 0;
	}
	return 1;
}

static void test_unicode()
{
	FONT *f = font;
	DATAFILE *dat = load_datafile(DATA_PREFIX "font.dat");
	if (dat && dat->type == DAT_FONT)
		f = (FONT *) dat->dat;
	int c;

	puttext(font, "ACS to unicode:", 0, 0);
	set_acs_unicode_table(acs_vt100, acs_unicode);
	for (c=0x60; c < 0x7f; c++)
		test_unicode_acs(f, c, 0x60, 0);
	for (c='+'; c <= '0'; c++)
		test_unicode_acs(f, c, '+', 254);

	puttext(font, "testing UTF-8 encoder...", 0, 70);
	puttext(font, test_utf8_enc() ? "ok" : "failed!", 204, 70);

	if (dat)
		unload_datafile(dat);
}

static void test_8bit_acs(const FONT *f, int c, int c0, int x)
{
	char s[2] = "";
	s[0] = c;
	x += 8*(c-c0);
	puttext(f, s, x, 10);
	convert_chars(s, s, 2);
	puttext(f, s, x, 12+text_height(f));
}

static int add32(int x) { return x+32; }

static void test_8bit_latin1(const FONT *f)
{
	char s1[33] = "";
	char s2[33] = "";
	int y = 60;
	int i;
	for (i=0; i<32; i++)
		s1[i] = 0xa0+i;
	convert_chars(s1, s2, sizeof(s2));
	puttext(font, s1, 0, y);
	puttext(f, s2, 0, y+10);

	for (i=0; i<2; i++) {
		str_map(add32, s1, s1);
		convert_chars(s1, s2, sizeof(s2));
		y += 16+text_height(f);
		puttext(font, s1, 0, y);
		puttext(f, s2, 0, y+10);
	}
}

static void test_cp437()
{
	FONT *f = load_font(DATA_PREFIX CP437_FONT, NULL, NULL);
	if (!f) {
		puttext(font, "couldn't load " DATA_PREFIX CP437_FONT, 16, 16);
		return;
	}
	int c;

	puttext(font, "ACS to codepage 437:", 0, 0);
	set_char_table(acs_vt100, acs_cp437);
	for (c=0x60; c < 0x7f; c++)
		test_8bit_acs(f, c, 0x60, 0);
	for (c='+'; c <= '0'; c++)
		test_8bit_acs(f, c, '+', 254);

	puttext(font, "Latin-1 to CP437:", 0, 50);
	set_latin1_table(latin1_cp437);
	test_8bit_latin1(f);

	destroy_font(f);
}

static void test_ascii()
{
	int c;
	puttext(font, "ACS to ASCII:", 0, 0);
	set_char_table(acs_vt100, acs_ascii);
	for (c=0x60; c < 0x7f; c++)
		test_8bit_acs(font, c, 0x60, 0);
	for (c='+'; c <= '0'; c++)
		test_8bit_acs(font, c, '+', 254);

	puttext(font, "Latin-1 to ASCII:", 0, 50);
	set_latin1_table(latin1_ascii);
	test_8bit_latin1(font);
}

int main(int argc, char **argv)
{
	if (allegro_init() != 0)
		return 1;

	install_keyboard();

	if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, 320, 200, 0, 0) != 0) {
		if (set_gfx_mode(GFX_SAFE, 320, 200, 0, 0) != 0) {
			set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
			allegro_message("Unable to set any graphics mode:\n"
					"%s\n", allegro_error);
			return 1;
		}
	}

	black = makecol(0,0,0);
	white = makecol(255,255,255);

	clear_to_color(screen, black);
	test_unicode();
	readkey();

	set_uformat(U_ASCII);

	clear_to_color(screen, black);
	test_cp437();
	readkey();

	clear_to_color(screen, black);
	test_ascii();
	readkey();

	return 0;
}
END_OF_MAIN()
