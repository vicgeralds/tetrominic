#include "tetmino.h"
#include "view.h"

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

/* convert bits to byte pixels (bit n to piece[n]).
	0 -> 0x00
	1 -> 0xFF
*/
static void make_piece(unsigned char *piece, unsigned bits)
{
	int n = PIECE_WIDTH * PIECE_HEIGHT;

	for (; n > 0; n--) {
		*piece = 0 - (bits & 1);
		piece++;
		bits >>= 1;
	}
}

void render_tetmino(unsigned char *bmp, int w, int h, const struct tetmino *t, int mask)
{
	struct copyrect r = cliprect(
		w, h, PIECE_WIDTH, PIECE_HEIGHT,
		t->col - LEFT_WALL_WIDTH, h - PIECE_HEIGHT - t->row
	);

	if (r.width > 0 && r.height > 0) {
		unsigned char piece[PIECE_WIDTH * PIECE_HEIGHT];

		make_piece(piece, t->shape);

		mask &= 0x88 | (0x11 * (t->piece + 1));

		for (; r.height > 0; r.height--) {
			int i;
			for (i=0; i < r.width; i++) {
				if (piece[r.si + i])
					bmp[r.di + i] = piece[r.si + i] & mask;
			}
			r.di += w;
			r.si += PIECE_WIDTH;
		}
	}
}
