#include <string.h>
#include "tetmino.h"
#include "view.h"

#define MIN(a,b)	((a) < (b) ? (a) : (b))

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

/* toggle 0x80 bits from bitmap before rendering piece */
static void unrender_piece(struct piece *p)
{
	const unsigned char *bmp = p->bitmap.bitmap;
	struct copyrect r = p->r;

	for (; r.height > 0; r.height--) {
		int i;
		for (i=0; i < r.width; i++) {
			p->piece[r.si + i] &= bmp[r.di + i] | 0x7f;
		}
		r.di += p->bitmap.width;
		r.si += PIECE_WIDTH;
	}
}

static void render_piece(const struct piece *p, int mask)
{
	unsigned char *bmp = p->bitmap.bitmap;
	struct copyrect r = p->r;

	for (; r.height > 0; r.height--) {
		int i;
		for (i=0; i < r.width; i++) {
			if (p->piece[r.si + i])
				bmp[r.di + i] = p->piece[r.si + i] & mask;
		}
		r.di += p->bitmap.width;
		r.si += PIECE_WIDTH;
	}
}

void render_tetmino_piece(struct piece *p, const struct tetmino *t, int mask)
{
	int w = p->bitmap.width;
	int h = p->bitmap.height;

	p->r = cliprect(
		w, h, PIECE_WIDTH, PIECE_HEIGHT,
		t->col - LEFT_WALL_WIDTH, h - (PIECE_HEIGHT - 1) - t->row
	);

	make_piece(p->piece, t->shape);

	unrender_piece(p);

	mask &= 0x88 | (0x11 * (t->piece + 1));

	render_piece(p, mask);
}

static void render_blocks(struct blocks *b)
{
	int w = b->piece.bitmap.width;
	int h = b->piece.bitmap.height;
	unsigned char *back = b->piece.bitmap.bitmap;
	unsigned char *front = b->piece.bitmap.bitmap + w * h;

	if (!b->rendered) {
		b->rendered = 1;
		memcpy(front, back, w * h);
		front = NULL;
	}
	drawtiles(front, back, w, h, b->x, b->y, &b->tiles);

	render_piece(&b->piece, 0x80);
}

void render_tetmino_blocks(struct blocks *b, const struct tetmino *t, int mask)
{
	render_tetmino_piece(&b->piece, t, mask);

	render_blocks(b);
}

void render_cleared_blocks(struct bitmap *bitmap, int row, blocks_row mask)
{
	unsigned char *bmp = bitmap->bitmap;
	int w = bitmap->width;
	int h = bitmap->height;
	int i = 0;

	if (mask) {
		for (; mask; mask >>= 1, i++) {
			if (mask & 1)
				bmp[w * (h - row) + i] = 0x80;
		}
	} else {
		memmove(bmp + w, bmp, w * (h - row));
		memset(bmp, 0, w);
		while (memchr(bmp + w, 0, w)) {
			bmp += w;
		}
		for (; i < w; i++) {
			if (!bmp[i]) {
				bmp[i] = 0x80;
			}
		}
	}
}
