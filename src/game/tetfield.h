/* Tetromino playing field */

/* Time constants are in frames assuming that the game will be run at this
 * speed
 */
#define TETFIELD_FPS 60

/* auto repeat rate. This limits how fast an action can be repeated */
#define AUTOREPEAT_FRAMES 3

/* how many times a blocked move is retried */
#define WALL_CHARGE_FRAMES 4

/* time until appearance of next tetromino */
#define SPAWN_DELAY	25

/* a tetromino piece in a playfield */
struct tetfield {
	struct tetmino mino;
	struct tetgrid *grid;
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
};

/* init next tetromino */
void enter_tetfield(struct tetfield *, int piece, int col);

/* advance one frame.
   return 0 on lock condition */
int run_tetfield(struct tetfield *, enum action, struct changed *out);

/* lock tetromino into place and init line clear.
   return number of lines cleared */
int lock_tetfield(struct tetfield *);
