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

	blocks.piece.bitmap.bitmap = game->bitmap;
	blocks.x = terminal.width / 2 - w;

	game->blocks = blocks;

	memset(game->bitmap, 0, top_size);
	memset(game->bitmap + top_size, 0x80, w * h1);
}

void init_game(struct game *game, int gravity)
{
	struct tetfield tetfield = { { 0 } };
	tetfield.blocks = game->tetgrid.blocks;
	tetfield.gravity = gravity;

	init_tetgrid(&game->tetgrid, GAME_TETGRID_COLS);
	game->tetfield = tetfield;
	enter_tetfield(&game->tetfield, rand7(), LEFT_WALL_WIDTH + 3);
	game->next = rand7();
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
	int row;

	if (!strcmp(input, "q")) {
		return 0;
	}
	if (!run_tetfield(tf, grid, get_action(input), &changed)) {
		if (tf->state == TETFIELD_TOP_OUT) {
			return 0;
		}
		render_tetmino_piece(&b->piece, &tf->mino, 0x7f);
		lock_tetfield(tf, grid);
		enter_tetfield(tf, game->next, LEFT_WALL_WIDTH + 3);
		game->next = rand7();
	}
	row = update_line_clears(grid);
       	if (row > 0 || (changed.moved | changed.dropped)) {
		while (row > 0) {
			blocks_row mask = shift_cleared_blocks(grid, row);
			render_cleared_blocks(&b->piece.bitmap, row, mask);
			row = next_cleared_row(grid, row);
		}
		render_tetmino_blocks(b, &tf->mino, 0x77);
		fflush(stdout);
	}
	return 1;
}
