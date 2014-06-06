/* Tetromino playing field */

/* auto repeat rate, 60 Hz/frames */
#define AUTOREPEAT_FRAMES 3

/* how many times a blocked move is retried */
#define WALL_CHARGE_FRAMES 4

/* NOTE: extra row needed for collision test after floor kick */
#define PLAYFIELD_HEIGHT 24
#define SPAWN_ROW (PLAYFIELD_HEIGHT - 5)

/* time until appearance of next tetromino */
#define SPAWN_DELAY	10
/* time until first drop */
#define SPAWN_GRAVITY	15

/* time between line clear animation steps */
#define BLOCK_CLEAR_DELAY 5

/* inverted bit used to mark blocks being cleared */
#define LINE_CLEAR_MARK 1

/* grid of occupied cells */
struct tetgrid {
	int cols;
	int clearing;	/* number of lines being cleared */
	int delay;
	blocks_row blocks[PLAYFIELD_HEIGHT];
};

/* a tetromino piece in a playfield */
struct tetfield {
	struct tetmino mino;
	struct tetgrid *grid;
	const struct tetmino *other;	/* other piece sharing the grid */
	int gravity;
	enum action charge;
	unsigned char timeout[END_ACTION + 1];
};

/* indicate what has changed since the last frame */
struct changed {
	enum action moved;	/* changed position or orientation */
	int dropped;		/* number of rows auto-dropped */
};

/* initialize grid of width cols (excluding walls) */
void init_tetgrid(struct tetgrid *, int cols);

/* init next tetromino */
void enter_tetfield(struct tetfield *, int piece, int col);

/* advance one frame.
   return 0 on lock condition */
int run_tetfield(struct tetfield *, enum action, struct changed *out);

/* lock tetromino into place and init line clear.
   return number of lines cleared */
int lock_tetfield(struct tetfield *);

/* clear blocks and process line clear gravity.
   return row number of cleared blocks */
int update_blocks(struct tetgrid *);

/* return blocks that have been cleared as set bits,
   with the leftmost visible column in bit 0.
   return 0 when the row is empty */
blocks_row get_cleared_blocks(const struct tetgrid *, int row);

/* get next row of cleared blocks.
   return row number > 0 until done */
int next_cleared_row(const struct tetgrid *, int row);

/* remove one row from the grid, moving rows above it down by 1.
   piece1 and piece2 are checked for collisions if they are not null.
   return row number of empty row above the range of moved rows */
int remove_cleared_row(struct tetgrid *, int row,
			const struct tetmino *piece1,
			const struct tetmino *piece2);
