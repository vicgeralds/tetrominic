#include <stdio.h>
#include <string.h>
#include "game.h"
#include "random.h"
#include "../terminal/terminal.h"

static void init_game_blocks(struct game *game)
{
	const int w = GAME_TETGRID_COLS;
	const int h1 = GAME_BLOCKS_CEILING;
	const int h2 = GAME_BLOCKS_HEIGHT;
	struct blocks blocks = BLOCKS_INIT(NULL, w, h2);
	const int top_size = w * (h2 - h1);

	blocks.bitmap.bitmap = game->bitmap;
	blocks.x = terminal.width / 2 - w;

	game->blocks = blocks;

	memset(game->bitmap, 0, top_size);
	memset(game->bitmap + top_size, 0x80, w * h1);
}

static struct tetmino *spawn_piece(struct game *game)
{
	enter_tetfield(&game->tetfield, game->next_tetmino.piece, GAME_SPAWN_COL);
	return init_tetmino(&game->next_tetmino,
		rand7(),
		game->tetfield.mino.row + PIECE_HEIGHT,
		GAME_SPAWN_COL,
		game->tetfield.gravity);
}

void init_game(struct game *game, int gravity)
{
	struct tetfield tetfield = { { 0 } };
	tetfield.grid = &game->tetgrid;
	tetfield.gravity = gravity;

	init_tetgrid(&game->tetgrid, GAME_TETGRID_COLS);
	game->tetfield = tetfield;
	init_tetmino_piece(&game->next_piece, spawn_piece(game), 0x7f,
		init_tetmino_piece(&game->piece, &game->tetfield.mino, 0x77, NULL));
	init_game_blocks(game);
}

static enum action get_action(const char *input)
{
	if (!strcmp(input, "j") || !strcmp(input, "\033[D")) {
		return MOVE_LEFT;
	}
	if (!strcmp(input, "k") || !strcmp(input, "\033[A")) {
		return ROTATE_CW;
	}
	if (!strcmp(input, "i")) {
		return ROTATE_CCW;
	}
	if (!strcmp(input, "l") || !strcmp(input, "\033[C")) {
		return MOVE_RIGHT;
	}
	if (!strcmp(input, "m") || !strcmp(input, "\033[B")) {
		return SOFTDROP;
	}
	if (!strcmp(input, " ")) {
		return HARDDROP;
	}

	return NO_ACTION;
}

static int update_next_tetmino(struct game *game)
{
	struct tetfield *tf = &game->tetfield;

	if (tf->state == TETFIELD_SPAWN) {
		return 0;
	}
	if (game->next_tetmino.row > SPAWN_ROW + 1) {
		game->next_tetmino.falling--;
		if (game->next_tetmino.falling > 0) {
			return 0;
		}
		game->next_tetmino.row--;
		game->next_tetmino.falling = tf->gravity;
		return 1;
	}

	return 0;
}

int update_game(struct game *game, const char *input)
{
	struct tetgrid *grid = &game->tetgrid;
	struct tetfield *tf = &game->tetfield;
	struct blocks *b = &game->blocks;
	struct changed changed;
	int cleared = update_line_clears(grid);

	if (cleared > 0) {
		int row = cleared;
		while (row > 0) {
			blocks_row mask = shift_cleared_blocks(grid, row);
			render_cleared_blocks(&b->bitmap, row, mask);
			row = next_cleared_row(grid, row);
		}
	}
	if (!strcmp(input, "q")) {
		return 0;
	}
	if (!run_tetfield(tf, get_action(input), &changed)) {
		if (tf->state == TETFIELD_TOP_OUT) {
			return 0;
		}
		render_tetmino_piece(&b->bitmap, &game->piece.piece, &tf->mino, 0x7f);
		lock_tetfield(tf);
		spawn_piece(game);
		render_tetmino_blocks(b, &game->piece);
	} else if (update_next_tetmino(game) || cleared > 0 ||
			(changed.moved | changed.dropped | changed.displaced)) {
		render_tetmino_blocks(b, &game->next_piece);
	}
	fflush(stdout);
	return 1;
}
