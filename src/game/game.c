#include <stdio.h>
#include <string.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <assert.h>
#endif

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

	game->blocks = blocks;

	memset(game->bitmap, 0, top_size);
	memset(game->bitmap + top_size, 0x80, w * h1);
}

static struct tetmino *spawn_piece(struct game *game)
{
	enter_tetfield(&game->tetfield, game->next_tetmino.piece, GAME_SPAWN_COL);
#ifdef __EMSCRIPTEN__
	EM_ASM({
		enterTetField($0, $1)
	}, game->next_tetmino.piece, game->tetfield.gravity);

	if (EM_ASM_INT({ return tetField })) {
		assert(game->tetfield.mino.col == LEFT_WALL_WIDTH + EM_ASM_INT({
			return tetField.piece.col
		}));
		assert(game->tetfield.mino.falling == EM_ASM_INT({
			return tetField.piece.falling
		}));
		assert(game->tetfield.mino.lock_delay_move == EM_ASM_INT({
			return tetField.piece.lockDelayMove
		}));
		assert(game->tetfield.mino.lock_delay_step == EM_ASM_INT({
			return tetField.piece.lockDelayStep
		}));
	}
#endif
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
	game->next_tetmino.piece = rand7();
	init_tetmino_piece(&game->next_piece, spawn_piece(game), 0x7f,
		init_tetmino_piece(&game->piece, &game->tetfield.mino, 0x77, NULL));
	init_game_blocks(game);
}

static enum action get_action(const char *input)
{
	if (!strcmp(input, "j") || !strcmp(input, "left")) {
		return MOVE_LEFT;
	}
	if (!strcmp(input, "k") || !strcmp(input, "up")) {
		return ROTATE_CW;
	}
	if (!strcmp(input, "i")) {
		return ROTATE_CCW;
	}
	if (!strcmp(input, "l") || !strcmp(input, "right")) {
		return MOVE_RIGHT;
	}
	if (!strcmp(input, "m") || !strcmp(input, "down")) {
		return SOFTDROP;
	}
	if (!strcmp(input, "space")) {
		return HARDDROP;
	}

	return NO_ACTION;
}

static int update_next_tetmino(struct game *game)
{
	struct tetfield *tf = &game->tetfield;

#ifdef __EMSCRIPTEN__
	assert(tf->state == EM_ASM_INT({
		return tetField.state
	}));
	assert((!tf->grid->clearing) == EM_ASM_INT({
		return !tetField.grid.isClearing
	}));
	assert(tf->mino.row == EM_ASM_INT({
		return tetField.piece.row
	}));
	assert(tf->mino.falling == EM_ASM_INT({
		return tetField.piece.falling
	}));
	assert(tf->mino.lock_delay_move == EM_ASM_INT({
		return tetField.piece.lockDelayMove
	}));
	assert(tf->mino.lock_delay_step == EM_ASM_INT({
		return tetField.piece.lockDelayStep
	}));
#endif
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

static void render_score(const struct tetfield *tf)
{
	moveto(terminal.x0, terminal.y0);
	set_text_attr(0);
	printf("Score: %lu\n", tf->score);
	printf("Speed: %d frames per row\n", tf->gravity);
	terminal.cursor_y++;
	terminal.cursor_y++;
}

int update_game(struct game *game, const char *input)
{
	struct tetgrid *grid = &game->tetgrid;
	struct tetfield *tf = &game->tetfield;
	struct blocks *b = &game->blocks;
	struct changed changed;
	int cleared = update_line_clears(grid);
#ifdef __EMSCRIPTEN__
	assert(cleared == EM_ASM_INT({
		return tetField.grid.updateLineClears()
	}));
#endif

	b->x = terminal.width / 2 - GAME_TETGRID_COLS;
	b->y = 1;

	if (terminal.lines == 1) {
		int attr = FG_COLOR(0) | BOLD;
		b->rendered = 0;
		draw_acs("x22", 1, b->x - 1, b->y, attr);
		draw_acs("x22", 1, b->x + GAME_TETGRID_COLS * 2, b->y, attr);
		draw_acs("lq19k", 22, b->x - 1, 0, attr);
		draw_acs("mq19j", 22, b->x - 1, b->y + GAME_BLOCKS_HEIGHT, attr);
		render_score(tf);
		render_tetmino_blocks(b, &game->next_piece);
	}

	if (cleared > 0) {
		int row = cleared;
		while (row > 0) {
			blocks_row mask = shift_cleared_blocks(grid, row);
			render_cleared_blocks(&b->bitmap, row, mask);
#ifdef __EMSCRIPTEN__
			assert(mask == EM_ASM_INT({
				return tetField.grid.shiftClearedBlocks($0)
			}, row));
			row = EM_ASM_INT({
				return tetField.grid.nextClearedRow($0)
			}, row);
#else
			row = next_cleared_row(grid, row);
#endif
		}
		render_score(tf);
	}
	if (!strcmp(input, "q")) {
		return 0;
	}
#ifdef __EMSCRIPTEN__
	EM_ASM({
		runTetField($0)
	}, get_action(input));
#endif
	if (!run_tetfield(tf, get_action(input), &changed)) {
#ifdef __EMSCRIPTEN__
		assert(tf->state == EM_ASM_INT({
			return tetField.state
		}));
#endif
		if (tf->state == TETFIELD_TOP_OUT) {
			return 0;
		}
		render_tetmino_piece(&b->bitmap, &game->piece.piece, &tf->mino, 0x7f);
#ifdef __EMSCRIPTEN__
		assert(EM_ASM_INT({
			return tetField.isLockable
		}));
		assert(changed.dropped == EM_ASM_INT({
			return changed.dropped
		}));
		EM_ASM({
			tetField.lock()
		});
#endif
		lock_tetfield(tf);
		spawn_piece(game);
#ifdef __EMSCRIPTEN__
		int col = EM_ASM_INT({
			return tetField.piece.col
		});
		assert(tf->mino.col == col + LEFT_WALL_WIDTH);
#endif
		render_tetmino_blocks(b, &game->piece);
	} else if (update_next_tetmino(game) || cleared > 0 || (changed.moved | changed.dropped)) {
		render_tetmino_blocks(b, &game->next_piece);
	}
	flush_output();
	return 1;
}
