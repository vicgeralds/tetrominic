/* Text-based graphics */

#define textgfx terminal

/* must be called to get actual output */
void update_screen();

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

/* code page 437 graphic characters (IBM graphics) */
enum {
	TEXT_MEDIUM_SHADE          = '\xB1',    /* a */
	TEXT_LIGHT_SHADE           = '\xB0',    /* h */
	TEXT_LOWER_RIGHT_CORNER    = '\xD9',    /* j */
	TEXT_UPPER_RIGHT_CORNER    = '\xBF',    /* k */
	TEXT_UPPER_LEFT_CORNER     = '\xDA',    /* l */
	TEXT_LOWER_LEFT_CORNER     = '\xC0',    /* m */
	TEXT_CROSS                 = '\xC5',    /* n */
	TEXT_HORIZONTAL_LINE       = '\xC4',    /* q */
	TEXT_LEFT_T                = '\xC3',    /* t */
	TEXT_RIGHT_T               = '\xB4',    /* u */
	TEXT_BOTTOM_T              = '\xC1',    /* v */
	TEXT_TOP_T                 = '\xC2',    /* w */
	TEXT_VERTICAL_LINE         = '\xB3'     /* x */
};

/* convert to IBM graphics and output using textgfx.puttext() */
void putacs_text(const char *s);
