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

#define BLOCKS_INIT(bitmap, width, height) { \
	BLOCKS_PIECE_INIT(bitmap, width, height), \
	BLOCKS_TILES_INIT \
}

#define BLOCKS_PIECE_INIT(bitmap, width, height) { \
	{ bitmap, width, height } \
}

#define BLOCKS_TILES_INIT { \
	{ ' ', 'a', 'a', 'a', 'a', 'a', 'a', 'a', \
	  '.', 'h', 'h', 'h', 'h', 'h', 'h', 'h' }, \
	{ BLOCKS_TILES_ATTRS, BLOCKS_TILES_ATTRS } \
}

#define BLOCKS_TILES_ATTRS \
	BG_COLOR(0) | FG_COLOR(0) | BOLD, \
	BLOCKS_TILES_ATTR(1), BLOCKS_TILES_ATTR(2), BLOCKS_TILES_ATTR(3), \
	BLOCKS_TILES_ATTR(4), BLOCKS_TILES_ATTR(5), BLOCKS_TILES_ATTR(6), \
	BLOCKS_TILES_ATTR(7)

#define BLOCKS_TILES_ATTR(x) BG_COLOR(x) | FG_COLOR(x) | BOLD | ALTCHARSET

void render_tetmino_piece(struct piece *, const struct tetmino *t, int mask);
void render_tetmino_blocks(struct blocks *, const struct tetmino *t, int mask);
void render_cleared_blocks(struct bitmap *, int row, blocks_row mask);
