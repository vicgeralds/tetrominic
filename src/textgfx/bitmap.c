/* Bitmap manipulation */

#include "bitmap.h"

#define MIN(a,b)	((a) < (b) ? (a) : (b))

struct copyrect {
	int width, height;
	int di, si;	/* dest and source bitmap positions */
};

/* get intersection */
static struct copyrect cliprect(int dw, int dh, int sw, int sh,
				int x, int y)
{
	struct copyrect r;
	int row_pos = dw * y;

	r.width  = MIN(sw, dw - x);
	r.height = MIN(sh, dh - y);
	r.di = x + row_pos;
	r.si = 0;

	if (x < 0) {
		r.width += x;
		r.di -= x;
		r.si -= x;
	}
	if (y < 0) {
		r.height += y;
		r.di -= row_pos;
		r.si -= sw * y;
	}

	return r;
}

void setpiece(unsigned char *bmp, int w, int h,
	      const unsigned char *piece, int x, int y, int mask)
{
	struct copyrect r = cliprect(w, h, PIECE_WIDTH, PIECE_HEIGHT, x, y);

	if (r.width > 0 && r.height > 0) {
		bmp += r.di;
		piece += r.si;

		for (; r.height > 0; r.height--) {
			int i;
			for (i=0; i < r.width; i++) {
				if (piece[i])
					bmp[i] = piece[i] & mask;
			}
			bmp += w;
			piece += PIECE_WIDTH;
		}
	}
}

void make_piece(unsigned char *piece, unsigned bits)
{
	int n = PIECE_WIDTH * PIECE_HEIGHT;

	for (; n > 0; n--) {
		*piece = 0 - (bits & 1);
		piece++;
		bits >>= 1;
	}
}
