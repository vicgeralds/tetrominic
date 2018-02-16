/* Text-based graphics */

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
