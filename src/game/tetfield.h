/* Tetromino playing field */

/* Time constants are in frames assuming that the game will be run at this
 * speed
 */
#define TETFIELD_FPS 60

/* auto repeat rate. This limits how fast an action can be repeated */
#define AUTOREPEAT_FRAMES 3

/* how many times a blocked move is retried */
#define WALL_CHARGE_FRAMES 4

/* number of rows that can be occupied */
#define PLAYFIELD_HEIGHT 24
#define SPAWN_ROW (PLAYFIELD_HEIGHT - 4)

/* time until appearance of next tetromino */
#define SPAWN_DELAY	25

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

/* a tetromino piece in a playfield */
struct tetfield {
	struct tetmino mino;
	const blocks_row *blocks;
	int gravity;
	enum {
		TETFIELD_SPAWN,
		TETFIELD_MOVE,
		TETFIELD_PLACED,
		TETFIELD_TOP_OUT
	} state;
	enum action last_action;
	unsigned char timeout[END_ACTION + 1];
};

/* timeout[NO_ACTION] is number of times to retry last action */
#define RETRY_ACTION	NO_ACTION

/* indicate what has changed since the last frame */
struct changed {
	enum action moved;	/* changed position or orientation */
	int dropped;		/* number of rows auto-dropped */
	int displaced;		/* collision resolved */
};

/* initialize grid of width cols (excluding walls) */
void init_tetgrid(struct tetgrid *, int cols);

/* init next tetromino */
void enter_tetfield(struct tetfield *, int piece, int col);

/* advance one frame.
   return 0 on lock condition */
int run_tetfield(struct tetfield *, struct tetgrid *, enum action, struct changed *out);

/* lock tetromino into place and init line clear.
   return number of lines cleared */
int lock_tetfield(struct tetfield *, struct tetgrid *);

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
