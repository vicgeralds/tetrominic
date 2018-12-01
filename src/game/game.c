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

static struct tetmino *init_game_tetmino(struct game *game)
{
	return init_tetmino(&game->piece.data.tetmino,
		rand7(), SPAWN_ROW + 2, GAME_SPAWN_COL, 0);

}

void init_game(struct game *game, int gravity)
{
	struct tetfield tetfield = { { 0 } };
	tetfield.blocks = game->tetgrid.blocks;
	tetfield.gravity = gravity;

	init_tetgrid(&game->tetgrid, GAME_TETGRID_COLS);
	game->tetfield = tetfield;
	enter_tetfield(&game->tetfield, rand7(), GAME_SPAWN_COL);
	init_tetmino_piece(&game->next_piece,
		init_game_tetmino(game), 0x7f,
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
	if (!run_tetfield(tf, grid, get_action(input), &changed)) {
		if (tf->state == TETFIELD_TOP_OUT) {
			return 0;
		}
		render_tetmino_piece(&b->bitmap, &game->next_piece.data.piece, &tf->mino, 0x7f);
		lock_tetfield(tf, grid);
		enter_tetfield(tf, game->piece.data.tetmino.piece, GAME_SPAWN_COL);
		render_tetmino_blocks(b, &game->piece);
		init_game_tetmino(game);
	} else if (cleared > 0 || (changed.moved | changed.dropped | changed.displaced)) {
		struct tetmino next = game->piece.data.tetmino;
		int row = tf->mino.row - tf->mino.climbed + PIECE_HEIGHT;
		if (row > next.row) {
			game->piece.data.tetmino.row = row;
		}
		render_tetmino_blocks(b, &game->next_piece);
		game->piece.data.tetmino = next;
	}
	fflush(stdout);
	return 1;
}
