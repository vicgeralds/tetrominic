/* Tetromino stacking */

#define PIECE_WIDTH  4
#define PIECE_HEIGHT 4

/* orientations */

#define TETMINO_I  0x0F00	/* I    #      O    */
#define TETMINO_I2 0x2222	/*      #           */
				/* #### #       ##  */
#define TETMINO_O  0x6600	/*      #       ##  */

#define TETMINO_J  0x4700	/* J                */
#define TETMINO_J2 0x3220	/*      #  #    ##  */
#define TETMINO_J3 0x0710	/* ###  #  ###  #   */
#define TETMINO_J4 0x2260	/*   # ##       #   */

#define TETMINO_L  0x1700	/* L                */
#define TETMINO_L2 0x2230	/*     ##    #  #   */
#define TETMINO_L3 0x0740	/* ###  #  ###  #   */
#define TETMINO_L4 0x6220	/* #    #       ##  */

#define TETMINO_S  0x3600	/* S       Z        */
#define TETMINO_S2 0x2310	/*     #        #   */
#define TETMINO_Z  0x6300	/*  ## ##  ##  ##   */
#define TETMINO_Z2 0x1320	/* ##   #   ## #    */

#define TETMINO_T  0x2700	/* T                */
#define TETMINO_T2 0x2320	/*      #   #   #   */
#define TETMINO_T3 0x0720	/* ### ##  ###  ##  */
#define TETMINO_T4 0x2620	/*  #   #       #   */

extern unsigned short tetmino_shapes[7][4];

/* occupied grid cells and walls */
typedef unsigned short blocks_row;

#define LEFT_WALL_WIDTH 3
#define MAKE_FLOOR(cols) ((((blocks_row) 1 << (cols)) - 1) << LEFT_WALL_WIDTH)
#define MAKE_WALLS(cols) ((blocks_row) (-1) ^ MAKE_FLOOR(cols))

/* timer presets */
#define LOCK_DELAY_MOVE 10
#define LOCK_DELAY_STEP 30

struct tetmino {
	unsigned shape;
	int piece;		/* 0-6 */
	int row, col;		/* position */
	int falling;		/* gravity timer */
	int lock_delay_move;	/* move reset timer */
	int lock_delay_step;	/* step reset timer */
	int floor_reached;	/* floor kick state */
};

/* toggle piece occupied grid cells */
void xor_tetmino(const struct tetmino *, blocks_row *);

/* get distance to the blocks below.
   return when height max is reached */
int drop_height(const struct tetmino *, const blocks_row *, int max);

/* check collision moving blocks down 1 row from above */
int has_blocks_above(const struct tetmino *, const blocks_row *);

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
