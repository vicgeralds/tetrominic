/* Tetromino stacking */

#define PIECE_WIDTH  4
#define PIECE_HEIGHT 4

/* orientations */

#define TETMINO_I  0x00F0	/* I    #      O    */
#define TETMINO_I2 0x2222	/*      #           */
				/* #### #       ##  */
#define TETMINO_O  0x0066	/*      #       ##  */

#define TETMINO_J  0x0074	/* J                */
#define TETMINO_J2 0x0223	/*      #  #    ##  */
#define TETMINO_J3 0x0170	/* ###  #  ###  #   */
#define TETMINO_J4 0x0622	/*   # ##       #   */

#define TETMINO_L  0x0071	/* L                */
#define TETMINO_L2 0x0322	/*     ##    #  #   */
#define TETMINO_L3 0x0470	/* ###  #  ###  #   */
#define TETMINO_L4 0x0226	/* #    #       ##  */

#define TETMINO_S  0x0063	/* S       Z        */
#define TETMINO_S2 0x0132	/*     #        #   */
#define TETMINO_Z  0x0036	/*  ## ##  ##  ##   */
#define TETMINO_Z2 0x0231	/* ##   #   ## #    */

#define TETMINO_T  0x0072	/* T                */
#define TETMINO_T2 0x0232	/*      #   #   #   */
#define TETMINO_T3 0x0270	/* ### ##  ###  ##  */
#define TETMINO_T4 0x0262	/*  #   #       #   */

extern unsigned short tetmino_shapes[7][4];

/* occupied grid cells and walls */
typedef unsigned short blocks_row;

#define LEFT_WALL_WIDTH 3
#define MAKE_FLOOR(cols) ((((blocks_row) 1 << (cols)) - 1) << LEFT_WALL_WIDTH)
#define MAKE_WALLS(cols) ((blocks_row) (-1) ^ MAKE_FLOOR(cols))

/* timer presets */
#define LOCK_DELAY_MOVE 20
#define LOCK_DELAY_STEP 30
#define MIN_LOCK_DELAY 15

struct tetmino {
	unsigned shape;
	int piece;		/* 0-6 */
	int row, col;		/* position */
	int falling;		/* gravity timer */
	int lock_delay_move;	/* move reset timer */
	int lock_delay_step;	/* step reset timer */
	int climbed;		/* floor kick state */
};

struct tetmino *init_tetmino(struct tetmino *, int piece, int row, int col, int delay);

/* extract one row from shape, counting from bottom */
int tetmino_has_row(unsigned shape, int i);

/* toggle piece occupied grid cells */
void xor_tetmino(const struct tetmino *, blocks_row *);

/* check collision when moving right by d columns (or left if d<0) */
int can_move_tetmino(const struct tetmino *, const blocks_row *, int d);

/* get distance to the blocks below.
   return when height max is reached */
int drop_height(const struct tetmino *, const blocks_row *, int max);

enum action {
	NO_ACTION,
	ROTATE_CW,
	ROTATE_CCW,
	MOVE_RIGHT,
	MOVE_LEFT,
	HARDDROP,
	SOFTDROP,
	END_ACTION
};

/* process moves.
   return non-zero if position or orientation changed */
int control_tetmino(struct tetmino *, const blocks_row *, enum action);

/* process gravity.
   gravity (or softdrop speed) is number of frames to count down.
   return number of rows dropped */
int update_tetmino(struct tetmino *, const blocks_row *, int gravity);

/* prevent floating piece */
void unfloat_tetmino(struct tetmino *, const blocks_row *);
