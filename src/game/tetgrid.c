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
