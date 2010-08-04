/* VITETRIS TEXTGFX LIBRARY
   Text display output and control */

#ifndef NUM_COLORS
#define NUM_COLORS 40
#endif

/* text tag bits */
#define TEXT_UCS2 1
#define TEXT_ACS  2

struct textgfx {
	int x, y;
	int x0, y0;
	int lines;
	void (*putstr)(const char *);
	void (*putacs)(const char *);
	void (*puttext)(const char *);
	void (*putchar)(int);
};

struct text {
	unsigned char tag, len;
	union {
		const char *c;
		const unsigned short *u;
	} str;
};

extern struct textgfx textgfx;
extern unsigned colors[NUM_COLORS];

void textgfx_init();
void textgfx_end();

void moveto(int x, int y);

void cleartoeol();
void clearscreen();

/* Select Graphic Rendition */

#define STANDOUT  7
#define BOLD      1
#define UNDERLINE 4

void setattr(int code);
void normalattr();

#define SGR_PARAMS(x,y,z) ((x) | (y)<<4 | (z)<<10)

void set_sgr_params(unsigned params);

/* set_sgr_params(colors[i]) */
void setcolor(int i);

/* ACS line drawing
 *
 *	+---+  lqqqk   drawline 5   "lqk"
 *
 *	+---+  lqqqk   drawbox  5x3 "lqkx xmqj" (default if NULL)
 *	|   |  x   x                           
 *	+---+  nqqqj
 */
void drawline (int x, int y, int n, const char *chars);
void drawbox  (int x, int y, int w, int h, const char *chars);
void drawframe(int x, int y, int w, int h, const char *title);
void clearbox (int x, int y, int w, int h);

/* put n characters using cycled string, offset i */
void put_pattern(const struct text *, int n, int i);

/* draw an image 8 columns wide
   one row per nibble (each bit is 2 chars)	8899...
   fill 1 bits with pattern			44556677
   (x,y) is bottom-left				00112233
*/
void draw_pattern_bits(const struct text *, unsigned bits, int x, int y);
