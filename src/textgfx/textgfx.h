/* Text-based graphics */

struct textgfx {
	int x, y;
	int x0, y0;	/* top-left origin */
	int lines;
	void (*putstr)(const char *);
	void (*putacs)(const char *);
	void (*puttext)(const char *);
};

extern struct textgfx textgfx;

void textgfx_init();
void textgfx_end();

/* must be called to get actual output */
void update_screen();

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

/* only used with drawchar */
#define ALTCHARSET	  0x1000
#define DONT_CHANGE_ATTR  0x2000

void set_text_attr(int attr);
void normalattr();

struct tg_buffer {
	int x, y;	/* cursor position where line begins */
	int len;	/* buffer string length */
	int attr;
	char buf[32];
};

/* add one character to the buffer */
void drawchar(int c, int attr, struct tg_buffer *);

/* flush buffer and update cursor position */
void flush_tg(struct tg_buffer *);

/* add line drawing characters from s.
   wrap after w characters (don't wrap if w is zero).
   repeat char if followed by a positive number (decimal digits)
*/
void drawacs(const char *s, int w, struct tg_buffer *);

/* a tileset using one character per nibble */
struct tg_tiles {
	char chars[16];
	unsigned short attrs[16];
};

/* draw an image (w*h bytes) using a tileset.
   if front != NULL only changes are printed and back is copied to front */
void drawtiles(unsigned char *front, const unsigned char *back,
	       int w, int h, int x, int y, const struct tg_tiles *);

/* pad with spaces (one space before and one after)
   and centre on line of length w.
   return the number of characters taken from s */
int centre_text(const char *s, int w, int x, int y);
