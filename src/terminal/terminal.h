/* Text terminal interface */

#ifndef terminal_h
#define terminal_h

struct terminal {
	unsigned short width;
	unsigned short height;
	int lines;
	void (*putacs)(const char *);
	void (*puttext)(const char *);
	int cursor_x, cursor_y;
	int x0, y0;
};

extern struct terminal terminal;

int init_terminal();

void setup_terminal();
void restore_terminal();

/* move the cursor to column x, row y */
void moveto(int x, int y);

void cleartoeol();
void clearscreen();

/* Select Graphic Rendition */

#define BOLD        0x100
#define UNDERLINE   0x200
#define BLINK       0x400
#define STANDOUT    0x800
#define FG_COLOR(x) ((x) | 8)
#define BG_COLOR(x) (((x) | 8) << 4)

#define ALTCHARSET	0x1000
#define VT100_GRAPHICS  0x2000

/* set SGR parameters and switch back from alternate character set.
 *
 * The flags ALTCHARSET and VT100_GRAPHICS prevents switching back to normal.
 */
void set_text_attr(int attr);

void hide_cursor();
void show_cursor();

void putacs_vt100(const char *s);

void puttext_ibm(const char *s);

/* put cp437 string using the current locale */
void puttext_unicode(const char *s);

/* pad with single spaces and centre on line of length w.
 *
 * return the number of characters taken from s
 */
int centre_text(const char *s, int w, int x, int y);

#endif
