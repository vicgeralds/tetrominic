/* number of rows that can be occupied */
#define PLAYFIELD_HEIGHT 24
#define SPAWN_ROW (PLAYFIELD_HEIGHT - 4)

/* time between line clear animation steps */
#define BLOCK_CLEAR_DELAY 5

/* inverted bit used to mark blocks being cleared */
#define LINE_CLEAR_MARK 1

/* grid representing the set of occupied cells. It also has state for line
 * clear animation
 */
struct tetgrid {
	int cols;
	int clearing;	/* number of lines being cleared */
	int delay;	/* time until next update */
	blocks_row blocks[PLAYFIELD_HEIGHT];
};

/* initialize grid of width cols (excluding walls) */
void init_tetgrid(struct tetgrid *, int cols);

/* tick line clear timer.
   return row number > 0 when there are cleared blocks to process */
int update_line_clears(struct tetgrid *);

/* remove cleared blocks from row (two at a time).
 *
 * if the row is empty: remove it from the grid by moving all rows above it
 * down by 1.
 *
 * return blocks that have been cleared as set bits, with the leftmost visible
 * column in bit 0
 */
blocks_row shift_cleared_blocks(struct tetgrid *, int row);

/* get next row of cleared blocks.
   return row number > 0 until done */
int next_cleared_row(const struct tetgrid *, int row);
