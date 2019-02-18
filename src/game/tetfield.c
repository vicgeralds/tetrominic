#include "tetmino.h"
#include "tetfield.h"

void init_tetgrid(struct tetgrid *grid, int cols)
{
	int i;
	grid->cols = cols;
	grid->clearing = 0;
	grid->delay = 0;
	grid->blocks[0] = MAKE_FLOOR(cols);
	for (i=1; i < PLAYFIELD_HEIGHT; i++)
		grid->blocks[i] = MAKE_WALLS(cols);
}

static int is_empty_row(const struct tetgrid *grid, int row)
{
	/* floor has all bits set between the walls */
	return (grid->blocks[row] & grid->blocks[0]) == 0;
}

static void stop_retrying_action(struct tetfield *f)
{
	if (f->last_action != NO_ACTION) {
		f->last_action = NO_ACTION;
		f->timeout[RETRY_ACTION] += SPAWN_DELAY - AUTOREPEAT_FRAMES;
	}
}

void enter_tetfield(struct tetfield *f, int piece, int col)
{
	init_tetmino(&f->mino, piece, SPAWN_ROW + 1, col, SPAWN_DELAY + 1);
	f->state = TETFIELD_SPAWN;
	/* prevent last action from cancelling spawn delay */
	stop_retrying_action(f);
}

static enum action make_move(struct tetfield *f, enum action a)
{
	if (f->timeout[a] == 0 && control_tetmino(&f->mino, f->blocks, a)) {
		if (f->last_action == NO_ACTION || f->timeout[f->last_action] == 0) {
			f->last_action = a;
		}
		f->timeout[RETRY_ACTION] = AUTOREPEAT_FRAMES;
		f->timeout[a] = AUTOREPEAT_FRAMES;
		return a;
	}
	return NO_ACTION;
}

static void dec_timeout(struct tetfield *f)
{
	int i;
	for (i=0; i < END_ACTION; i++) {
		if (f->timeout[i])
			f->timeout[i]--;
	}
}

/* process moves before the piece is spawned */
static void update_prespawn(struct tetfield *f, enum action a)
{
	if (!f->timeout[RETRY_ACTION]) {
		switch (a) {
		case ROTATE_CW:
		case ROTATE_CCW:
			f->last_action = a;
			break;
		case MOVE_RIGHT:
		case MOVE_LEFT:
		case HARDDROP:
		case SOFTDROP:
			f->mino.falling = 1;
			f->timeout[a] = 1;
			break;
		default:
			break;
		}
	}
}

/* initial rotation */
static enum action spawn_orient(struct tetfield *f)
{
	unsigned unrotated = tetmino_shapes[f->mino.piece][0];
	if (f->mino.shape != unrotated) {
		if (drop_height(&f->mino, f->blocks, 1))
			return (f->mino.shape == tetmino_shapes[f->mino.piece][1]) ?
				ROTATE_CW : ROTATE_CCW;
		f->mino.shape = unrotated;
	}
	return NO_ACTION;
}

static enum action update_move(struct tetfield *f, enum action a)
{
	enum action moved = NO_ACTION;

	if (f->timeout[a]) {
		if (f->timeout[RETRY_ACTION]) {
			moved = make_move(f, f->last_action);
		}
		if (a != NO_ACTION) {
			f->last_action = a;
			f->timeout[RETRY_ACTION] = AUTOREPEAT_FRAMES;
		}
	} else if ((moved = make_move(f, a)) == NO_ACTION) {
		if (a != NO_ACTION) {
			f->last_action = a;
			f->timeout[RETRY_ACTION] = WALL_CHARGE_FRAMES;
		}
	}

	/* set timer to block actions for next tetromino during spawn delay */
	if (f->timeout[RETRY_ACTION] == 1) {
		stop_retrying_action(f);
	}

	return moved;
}

static int is_movable(const struct tetmino *t)
{
	return t->falling || (t->lock_delay_move && t->lock_delay_step);
}

/* check that there are no unfinished line clears at occupied rows */
static int is_lockable(const struct tetfield *f)
{
	const blocks_row *blocks = f->blocks + f->mino.row;
	int i;
	for (i=0; i < PIECE_HEIGHT; i++) {
		if (!(blocks[i] & LINE_CLEAR_MARK) && tetmino_has_row(f->mino.shape, i))
			return 0;
	}
	return 1;
}

int run_tetfield(struct tetfield *f, struct tetgrid *grid,
		      enum action a, struct changed *out)
{
	dec_timeout(f);
	out->moved = NO_ACTION;
	out->dropped = 0;
	out->displaced = 0;

	switch (f->state) {
	case TETFIELD_SPAWN:
		update_prespawn(f, a);
		if (f->mino.falling > 1) {
			/* stop spawn timer during line clear animation */
			if (grid->clearing) return 1;

			f->mino.falling--;
			return 1;
		}
		out->dropped = update_tetmino(&f->mino, f->blocks, f->gravity);
		if (!out->dropped) {
			f->state = TETFIELD_TOP_OUT;
			return 0;
		}
		f->timeout[RETRY_ACTION] = 1;
		f->state = TETFIELD_MOVE;
		out->moved = update_move(f, a);
		break;
	case TETFIELD_MOVE:
		out->moved = update_move(f, a);
		out->dropped = update_tetmino(&f->mino, f->blocks, f->gravity);
		break;
	case TETFIELD_PLACED:
		/* accepting no more actions */
		out->dropped = control_tetmino(&f->mino, f->blocks, HARDDROP);
		break;
	case TETFIELD_TOP_OUT:
		return 0;
	}

	/* resolve collision upwards */
	while (!can_move_tetmino(&f->mino, f->blocks, 0)) {
		if (f->mino.row >= SPAWN_ROW) {
			f->state = TETFIELD_TOP_OUT;
			return 0;
		}
		f->mino.row++;
		out->displaced = 1;
	}
	unfloat_tetmino(&f->mino, grid->blocks);

	/* return 0 on lock condition */
	return is_movable(&f->mino) || (f->state = TETFIELD_PLACED, !is_lockable(f));
}

int lock_tetfield(struct tetfield *f, struct tetgrid *grid)
{
	blocks_row *blocks = grid->blocks;
	int num_lines_cleared = 0;
	int i;

	/* add blocks */
	xor_tetmino(&f->mino, blocks);

	/* find completed lines */
	blocks += f->mino.row;
	for (i=0; i < PIECE_HEIGHT; i++) {
		blocks_row b = blocks[i] + 1;
		if (!b) {
			num_lines_cleared++;
			/* init line clear */
			blocks[i] ^= LINE_CLEAR_MARK;
			grid->clearing++;
			grid->delay += BLOCK_CLEAR_DELAY;
		}
	}
	return num_lines_cleared;
}

/* return empty columns as set bits */
static blocks_row invert_blocks(const struct tetgrid *grid, int row)
{
	blocks_row b = -1;
	return b ^ (grid->blocks[row] | LINE_CLEAR_MARK);
}

static blocks_row grow_cleared_blocks(struct tetgrid *grid, int row)
{
	blocks_row b = invert_blocks(grid, row);
	if (b)
		grid->blocks[row] &= ~((b << 1) | (b >> 1));
	else {
		/* odd or even width */
		b = (grid->cols & 1) ? 1 : 3;
		/* start clearing from middle */
		grid->blocks[row] ^= b << (grid->cols/2 + LEFT_WALL_WIDTH - 1);
	}

	return (b ^ invert_blocks(grid, row)) >> LEFT_WALL_WIDTH;
}

int update_line_clears(struct tetgrid *grid)
{
	int cleared = 0;
	int clearing = grid->clearing;
	int i;

	if (grid->delay > 0) {
		grid->delay--;
		return 0;
	}
	for (i=1; clearing > 0 && i < PLAYFIELD_HEIGHT; i++) {
		if (!(grid->blocks[i] & LINE_CLEAR_MARK)) {
			cleared = i;
			clearing--;
			if (!is_empty_row(grid, i)) {
				grid->delay = BLOCK_CLEAR_DELAY;
			}
		}
	}
	return cleared;
}

blocks_row shift_cleared_blocks(struct tetgrid *grid, int row)
{
	if (is_empty_row(grid, row)) {
		/* remove cleared row */
		for (; row + 1 < PLAYFIELD_HEIGHT; row++) {
			grid->blocks[row] = grid->blocks[row + 1];
		}
		grid->clearing--;
		return 0;
	}
	return grow_cleared_blocks(grid, row);
}

int next_cleared_row(const struct tetgrid *grid, int row)
{
	do row--;
	while (grid->blocks[row] & LINE_CLEAR_MARK);
	/* row 0 always has the LINE_CLEAR_MARK bit reversed */
	return row;
}
