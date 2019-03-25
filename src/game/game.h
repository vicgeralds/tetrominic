#include "tetmino.h"
#include "tetfield.h"
#include "view.h"

/* microseconds per frame */
#define GAME_FRAME_TIME (1000000L / TETFIELD_FPS)

#define GAME_TETGRID_COLS 10
#define GAME_SPAWN_COL (LEFT_WALL_WIDTH + 3)
#define GAME_BLOCKS_HEIGHT (PLAYFIELD_HEIGHT - 1)
#define GAME_BLOCKS_CEILING (SPAWN_ROW)
#define GAME_BITMAP_SIZE (GAME_TETGRID_COLS * GAME_BLOCKS_HEIGHT * 2)

struct game {
	struct tetgrid tetgrid;
	struct tetfield tetfield;
	struct tetmino next_tetmino;
	struct tetmino_piece piece, next_piece;
	struct blocks blocks;
	unsigned char bitmap[GAME_BITMAP_SIZE];
};

void init_game(struct game *game, int gravity);

int update_game(struct game *game, const char *input);
