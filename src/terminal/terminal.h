/* (UNIX) Terminal interface */

#ifndef terminal_h
#define terminal_h
#include <stdio.h>

#define XTERM      1
#define LINUX_TERM 2
#define GNOME_TERM 4
#define CYGWIN     8

enum charenc {
	CHAR_ASCII,
	CHAR_ISO8859_1,
	CHAR_CP850,
	CHAR_CP437,
	CHAR_UTF8
};

/* how to do line drawing */
enum acs {
	ACS_ASCII,
	ACS_VT100,	/* switch to ACS */
	ACS_IBM,	/* use codepage graphics */
	ACS_UNICODE	/* it's popular */
};

struct terminal {
	unsigned short width;
	unsigned short height;
	unsigned short type;
	enum charenc charenc;
	enum acs     acs;
	const char  *acs_disabled;	/* set of disabled characters */
	FILE *out;
	char *wm_class[2];
	void (*hide_cursor)();
	void (*show_cursor)();
	int  (*has_focus)();
};

extern struct terminal terminal;

/* init terminal structure */
void init_terminal();

void gettermsize();
void gettermtype();

/* connect to X and get focus detection and more!
   terminal.
	wm_class[2]	e.g. {"xterm", "XTerm"}
	has_focus()	returns 0 if top-level window loses focus
*/
void xwindow_init();
void xwindow_exit();

/* terminal non-canonical mode */
void set_input_mode();
void restore_input_mode();

/* simple output wrappers */
void tputstr(const char *s);
int  tprintf(const char *format, ...);

/* use the alt char set (line drawing) */
void init_acs_caps();
void set_acs_table();
void tputacs(const char *s);

/* enable conversion to terminal.charenc */
void set_latin1_codepage();

/* put iso8859-1 string */
void tputtext(const char *s);

/* put unicode character (cast to unsigned char if < 0) */
void tputchar(int c);

/* Low-level character coding */

extern const char acs_vt100[];
extern const char acs_ascii[];
extern const char acs_cp437[];
extern const unsigned short acs_unicode[];

extern const char latin1_ascii[96];
extern const char latin1_cp850[96];
extern const char latin1_cp437[96];

/* example: set_char_table(acs_vt100, acs_ascii) */
void set_char_table(const char *set1, const char *set2);

/* example: set_acs_unicode_table(acs_vt100, acs_unicode) */
void set_acs_unicode_table(const char *set1, const unsigned short *set2);

/* set ascii or codepage conversion table for latin-1 supplement A0-FF */
void set_latin1_table(const char *table);

/* replace mappings to C0 control codes with safe ones */
void replace_codepage_c0();

/* convert using char table, to buf of size n.
   returns length of string stored in buf == strlen(buf) < n */
int convert_chars(const char *s, char *buf, int n);

/* typical use: uc_min=0xA0 */
int convert_chars_hi(const char *s, char *buf, int n, unsigned uc_min);

int acs_to_unicode(int c);

/* apply pure function f to chars in s1 -> s2 */
void str_map(int (*f)(int), const char *s1, char *s2);

/* all characters are 7-bit */
int isasciistr(const char *s);

/* Encode one character to UTF-8 and return the number of bytes used.
   Max 3 bytes -- from U+0000 to U+FFFF. */
int setc_utf8(char *s, unsigned uc);

#endif
