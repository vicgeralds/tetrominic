/* Bitmap manipulation */

#include "../terminal/textgfx.h"

struct bitmap {
	unsigned char *bitmap;
	int width;	/* width in columns */
	int height;	/* height in rows */
};

struct copyrect {
	int width, height;
	int di, si;	/* dest and source bitmap positions */
};

struct piece {
	struct bitmap bitmap;
	struct copyrect r;
	unsigned char piece[PIECE_WIDTH * PIECE_HEIGHT];
};

struct blocks {
	struct piece piece;
	struct tg_tiles tiles;
	int x, y;
	int rendered;
};

void render_tetmino_piece(struct piece *, const struct tetmino *t, int mask);
void render_tetmino_blocks(struct blocks *, const struct tetmino *t, int mask);
void render_cleared_blocks(struct bitmap *, int row, blocks_row mask);
