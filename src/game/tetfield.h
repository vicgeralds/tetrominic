/* Tetromino playing field */

/* auto repeat rate, 60 Hz/frames */
#define AUTOREPEAT_FRAMES 3

/* how many times a blocked move is retried */
#define WALL_CHARGE_FRAMES 4

/* N.B. extra row needed for collision test after floor kick */
#define PLAYFIELD_HEIGHT 24
#define SPAWN_ROW (PLAYFIELD_HEIGHT - 5)

/* time until appearance of next tetromino */
#define SPAWN_DELAY	10
/* time until first drop */
#define SPAWN_GRAVITY	15

/* conservative maximum size of saved tetgrid */
#define TETGRID_STATE_SIZE (sizeof(blocks_row) * PLAYFIELD_HEIGHT)

/* grid of occupied cells */
struct tetgrid {
	int cols;
	blocks_row blocks[PLAYFIELD_HEIGHT];
};

/* a tetromino piece in a playfield */
struct tetfield {
	struct tetmino mino;
	struct tetgrid *grid;
	int gravity, dropped;
	enum action charge, moved;
	unsigned char timeout[END_ACTION + 1];
};

/* initialize grid of width cols (excluding walls) */
void init_tetgrid(struct tetgrid *, int cols);

/* save tetgrid to state as a null-terminated byte string,
   starting from given row index, copying at most TETGRID_STATE_SIZE bytes.
   return number of bytes copied */
int save_tetgrid(const struct tetgrid *, int row, unsigned char *state);

/* load tetgrid from state (initialize first) */
void load_tetgrid(struct tetgrid *, int row, const unsigned char *state);

/* init next tetromino */
void enter_tetfield(struct tetfield *, int piece, int col);

/* set members 'moved' and 'dropped' to indicate changed position or orientation.
   return 0 on lock condition */
int run_tetfield(struct tetfield *, enum action);
