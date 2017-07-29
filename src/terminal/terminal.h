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

/* init terminal structure */
void init_terminal();

/* update terminal.width and terminal.height */
void gettermsize();

/* terminal non-canonical mode */
void set_input_mode();
void restore_input_mode();

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

/* put cp437 string using the current locale */
void tputtext(const char *s);

#endif
