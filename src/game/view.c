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
	int i;

	for (i = PIECE_HEIGHT; i > 0; i--) {
		int j;
		for (j = 0; j < PIECE_WIDTH; j++) {
			piece[(i-1) * PIECE_WIDTH + j] = 0 - (bits & 1);
			bits >>= 1;
		}
	}
}

static void render_piece(struct bitmap *bitmap, struct piece *p, int mask)
{
	unsigned char *bmp = bitmap->bitmap;
	struct copyrect r = p->r;

	for (; r.height > 0; r.height--) {
		int i;
		for (i=0; i < r.width; i++) {
			int block = p->piece[r.si + i];
			/* save background */
			p->piece[r.si + i] = bmp[r.di + i];
			if (block)
				bmp[r.di + i] = block & mask;
		}
		r.di += bitmap->width;
		r.si += PIECE_WIDTH;
	}
}

void render_tetmino_piece(struct bitmap *bitmap, struct piece *p, const struct tetmino *t, int mask)
{
	int w = bitmap->width;
	int h = bitmap->height;

	p->r = cliprect(
		w, h, PIECE_WIDTH, PIECE_HEIGHT,
		t->col - LEFT_WALL_WIDTH, h - (PIECE_HEIGHT - 1) - t->row
	);

	make_piece(p->piece, t->shape);

	mask &= 0x88 | (0x11 * (t->piece + 1));

	render_piece(bitmap, p, mask);
}

struct tetmino_piece *init_tetmino_piece(struct tetmino_piece *tp,
	const struct tetmino *t, int mask, struct tetmino_piece *succ)
{
	tp->tetmino = t;
	tp->mask = mask;
	tp->succ = succ;
	return tp;
}

static struct piece *render_tetmino_pieces(struct bitmap *bitmap, struct tetmino_piece *tp)
{
	struct piece *piece = NULL;

	while (tp) {
		render_tetmino_piece(bitmap, &tp->piece, tp->tetmino, tp->mask);
		tp->piece.prev = piece;
		piece = &tp->piece;
		tp = tp->succ;
	}

	return piece;
}

static void remove_piece(struct bitmap *bitmap, struct piece *p)
{
	unsigned char *bmp = bitmap->bitmap;

	while (p) {
		struct copyrect r = p->r;

		for (; r.height > 0; r.height--) {
			memcpy(bmp + r.di, p->piece + r.si, r.width);
			r.di += bitmap->width;
			r.si += PIECE_WIDTH;
		}

		p = p->prev;
	}
}

static void render_blocks(struct blocks *b, struct piece *p)
{
	int w = b->bitmap.width;
	int h = b->bitmap.height;
	unsigned char *back = b->bitmap.bitmap;
	unsigned char *front = b->bitmap.bitmap + w * h;

	if (!b->rendered) {
		b->rendered = 1;
		memcpy(front, back, w * h);
		front = NULL;
	}
	drawtiles(front, back, w, h, b->x, b->y, &b->tiles);

	remove_piece(&b->bitmap, p);
}

void render_tetmino_blocks(struct blocks *b, struct tetmino_piece *tp)
{
	render_blocks(b, render_tetmino_pieces(&b->bitmap, tp));
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
