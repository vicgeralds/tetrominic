#include "tetmino.h"
#include "tetgrid.h"

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

void update_removable_rows(struct tetgrid *grid, const struct tetmino *t)
{
	struct tetgrid grid2 = *grid;
	int row = update_line_clears(&grid2);

	/* find row that causes collision with tetmino */
	while (row > 0 && (shift_cleared_blocks(&grid2, row) || can_move_tetmino(t, grid2.blocks, 0))) {
		row = next_cleared_row(grid, row);
	}
	/* stop remaining empty rows from being cleared */
	while (row > 0) {
		if (is_empty_row(grid, row)) {
			grid->blocks[row] |= LINE_CLEAR_MARK;
			grid->clearing--;
		}
		row = next_cleared_row(grid, row);
	}
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
	/* find cleared row to start with */
	for (i=1; clearing > 0 && i < PLAYFIELD_HEIGHT; i++) {
		if (!(grid->blocks[i] & LINE_CLEAR_MARK)) {
			cleared = i;
			clearing--;
			grid->delay = BLOCK_CLEAR_DELAY;
		}
	}
	/* find empty rows to remove on next update */
	for (i = cleared + 1; i + 1 < PLAYFIELD_HEIGHT; i++) {
		if (is_empty_row(grid, i) && !is_empty_row(grid, i + 1)) {
			grid->blocks[i] ^= LINE_CLEAR_MARK;
			grid->clearing++;

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
