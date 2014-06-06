#include <string.h>
#include "tetmino.h"
#include "tetfield.h"

void init_tetgrid(struct tetgrid *grid, int cols)
{
	int i;
	grid->cols = cols;
	grid->blocks[0] = MAKE_FLOOR(cols);
	for (i=1; i < PLAYFIELD_HEIGHT; i++)
		grid->blocks[i] = MAKE_WALLS(cols);
}

static int is_empty_row(const struct tetgrid *grid, int row)
{
	/* floor has all bits set between the walls */
	return (grid->blocks[row] & grid->blocks[0]) == 0;
}

void enter_tetfield(struct tetfield *f, int piece, int col)
{
	f->mino.shape = 0;
	f->mino.piece = piece;
	f->mino.row = SPAWN_ROW + 1;	/* drop 1 row to test top out */
	f->mino.col = col;
	f->mino.falling = 1;
	memset(f->timeout, SPAWN_DELAY + 1, END_ACTION);
}

static enum action make_move(struct tetfield *f, enum action a)
{
	if (f->timeout[a] == 0 && control_tetmino(&f->mino, f->grid->blocks, a)) {
		f->charge = NO_ACTION;
		f->timeout[NO_ACTION] = 0;
		f->timeout[a] = AUTOREPEAT_FRAMES;
		return a;
	}
	return NO_ACTION;
}

static void dec_timeout(struct tetfield *f)
{
	int line_clear_delayed = f->grid->clearing && !f->mino.shape;
	int i;
	for (i=0; i < END_ACTION; i++) {
		if (f->timeout[i])
			f->timeout[i]--;
		/* stop spawn timer during line clear animation */
		if (line_clear_delayed)
			break;
	}
}

static int update_gravity(struct tetfield *f)
{
	int gravity = f->gravity;
	/* set minimum frames until first drop */
	if (f->mino.row >= SPAWN_ROW && gravity < SPAWN_GRAVITY)
		gravity = SPAWN_GRAVITY;

	if (!f->mino.shape) {
		/* spawn delay */
		if (f->timeout[ROTATE_CW])
			return 0;
		f->mino.shape = tetmino_shapes[f->mino.piece][0];
		f->mino.falling = 0;

		/* discard drops during spawn delay */
		if (f->charge == HARDDROP || f->charge == SOFTDROP)
			f->charge = NO_ACTION;
	}
	return update_tetmino(&f->mino, f->grid->blocks, gravity);
}

static void update_move(struct tetfield *f, enum action a, struct changed *out)
{
	/* timeout[NO_ACTION] is number of times to retry charged action */
	if (f->timeout[NO_ACTION] == 0)
		f->charge = NO_ACTION;

	if (f->timeout[a]) {
		out->moved = make_move(f, f->charge);
		if (a != NO_ACTION) {
			f->charge = a;
			f->timeout[NO_ACTION] = AUTOREPEAT_FRAMES;
		}
	} else if ((out->moved = make_move(f, a)) == NO_ACTION) {
		f->charge = a;
		f->timeout[NO_ACTION] = WALL_CHARGE_FRAMES;
	}

	dec_timeout(f);
	out->dropped = update_gravity(f);
}

static int is_movable(const struct tetmino *t)
{
	return t->falling || (t->lock_delay_move && t->lock_delay_step);
}

/* check that there are no unfinished line clears at occupied rows */
static int is_lockable(const struct tetfield *f)
{
	const blocks_row *blocks = f->grid->blocks + f->mino.row;
	int i;
	for (i=0; i < PIECE_HEIGHT; i++) {
		if (!(blocks[i] & LINE_CLEAR_MARK) && tetmino_has_row(f->mino.shape, i))
			return 0;
	}
	return 1;
}

int run_tetfield(struct tetfield *f, enum action a, struct changed *out)
{
	/* accepting no more actions */
	if (!is_movable(&f->mino)) {
		out->moved = NO_ACTION;
		out->dropped = control_tetmino(&f->mino, f->grid->blocks, HARDDROP);
		return !is_lockable(f);
	}

	if (f->other) {
		xor_tetmino(f->other, f->grid->blocks);
		update_move(f, a, out);
		xor_tetmino(f->other, f->grid->blocks);
	} else {
		update_move(f, a, out);
	}
	unfloat_tetmino(&f->mino, f->grid->blocks);

	/* return 0 on lock condition */
	return is_movable(&f->mino) || !is_lockable(f);
}

int lock_tetfield(struct tetfield *f)
{
	blocks_row *blocks = f->grid->blocks;
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
			f->grid->clearing++;
			f->grid->delay += BLOCK_CLEAR_DELAY;
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

static void grow_cleared_blocks(struct tetgrid *grid, int row)
{
	blocks_row b = invert_blocks(grid, row);
	if (b)
		grid->blocks[row] &= ~((b << 1) | (b >> 1));
	else {
		/* odd or even width */
		b = (grid->cols & 1) ? 1 : 3;
		/* start clearing from middle */
		grid->blocks[row] ^= b << (grid->cols/2 +
		                           LEFT_WALL_WIDTH - 1);
	}
}

static int clear_blocks(struct tetgrid *grid)
{
	int cleared = 0;
	int clearing = grid->clearing;
	int i;
	for (i=1; clearing > 0 && i < PLAYFIELD_HEIGHT; i++) {
		if (!(grid->blocks[i] & LINE_CLEAR_MARK)) {
			cleared = i;
			clearing--;
			if (!is_empty_row(grid, i)) {
				grow_cleared_blocks(grid, i);
				grid->delay = BLOCK_CLEAR_DELAY;
			}
		}
	}
	return cleared;
}

int update_blocks(struct tetgrid *grid)
{
	if (grid->delay > 0) {
		grid->delay--;
		return 0;
	}
	return clear_blocks(grid);
}

blocks_row get_cleared_blocks(const struct tetgrid *grid, int row)
{
	return is_empty_row(grid, row) ?
		0 :
		invert_blocks(grid, row) >> LEFT_WALL_WIDTH;
}

int next_cleared_row(const struct tetgrid *grid, int row)
{
	do row--;
	while (grid->blocks[row] & LINE_CLEAR_MARK);
	/* row 0 always has the LINE_CLEAR_MARK bit reversed */
	return row;
}

/* check for collision with piece when removing row */
static int check_remove(const struct tetmino *piece,
			const struct tetgrid *grid, int row, int end)
{
	if (piece) {
		int stuck = has_blocks_above(piece, grid->blocks, row);
		if (stuck && stuck < end) {
			if (row + 1 < stuck || is_movable(piece))
				return stuck;
			else
				/* no movable rows and piece can't lock */
				return 0;
		}
	}
	return end;
}

int remove_cleared_row(struct tetgrid *grid, int row,
			const struct tetmino *piece1,
			const struct tetmino *piece2)
{
	/* determine range of movable blocks */
	int end;
	end = check_remove(piece1, grid, row, PLAYFIELD_HEIGHT);
	end = check_remove(piece2, grid, row, end);

	for (; row + 1 < end; row++)
		grid->blocks[row] = grid->blocks[row + 1];

	/* set to empty row */
	grid->blocks[row] = MAKE_WALLS(grid->cols);

	if (end < PLAYFIELD_HEIGHT && (end || next_cleared_row(grid, row) > 0))
		/* try to remove row later */
		grid->blocks[row] ^= LINE_CLEAR_MARK;
	else
		grid->clearing--;

	return row;
}
