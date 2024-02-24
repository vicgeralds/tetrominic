#include "tetmino.h"

unsigned short tetmino_shapes[7][4] = {
	{TETMINO_I, TETMINO_I2, TETMINO_I,  TETMINO_I2 << 1},
	{TETMINO_S, TETMINO_S2, TETMINO_S,  TETMINO_S2 << 1},
	{TETMINO_T, TETMINO_T2, TETMINO_T3, TETMINO_T4},
	{TETMINO_O, TETMINO_O,  TETMINO_O,  TETMINO_O},
	{TETMINO_L, TETMINO_L2, TETMINO_L3, TETMINO_L4},
	{TETMINO_Z, TETMINO_Z2, TETMINO_Z,  TETMINO_Z2 << 1},
	{TETMINO_J, TETMINO_J2, TETMINO_J3, TETMINO_J4}
};

struct tetmino *init_tetmino(struct tetmino *t, int piece, int row, int col, int delay)
{
	t->shape = tetmino_shapes[piece][0];
	t->piece = piece;
	t->row = row;
	t->col = col + ((tetmino_shapes[piece][1] & 0x1111) ? 1 : 0);
	t->falling = delay;
	t->lock_delay_move = 0;
	t->lock_delay_step = 0;
	t->climbed = 2;
	return t;
}

int tetmino_has_row(unsigned shape, int i)
{
	int shift = i * PIECE_WIDTH;
	return (shape >> shift) % (1 << PIECE_WIDTH);
}

/* get mask to test collision */
static unsigned get_blocks_box(const blocks_row *blocks, int col)
{
	unsigned box = 0, row_mask = (1 << PIECE_WIDTH) - 1;
	int i;
	for (i=0; i < PIECE_HEIGHT; i++)
		box |= ((blocks[i] >> col) & row_mask) << (i * PIECE_WIDTH);

	return box;
}

/* add or remove blocks */
static void xor_blocks_box(blocks_row *blocks, int col, unsigned shape)
{
	unsigned row_mask = (1 << PIECE_WIDTH) - 1;
	int i;
	for (i = 0; i < PIECE_HEIGHT; i++) {
		blocks[i] ^= (shape & row_mask) << col;
		shape >>= PIECE_WIDTH;
	}
}

void xor_tetmino(const struct tetmino *t, blocks_row *blocks)
{
	xor_blocks_box(blocks + t->row, t->col, t->shape);
}

int can_move_tetmino(const struct tetmino *t, const blocks_row *blocks, int d)
{
	return !(t->shape & get_blocks_box(blocks + t->row, t->col + d));
}

int drop_height(const struct tetmino *t, const blocks_row *blocks, int max)
{
	int h = 0;

	if (max > t->row)
		max = t->row;

	if (max > 0) {
		unsigned box = get_blocks_box(blocks += t->row - 1, t->col);
		unsigned row_mask = (1 << PIECE_WIDTH) - 1;

		while (!(t->shape & box)) {
			h++;
			if (h == max)
				break;
			box = (box << PIECE_WIDTH) |
			      ((blocks[-h] >> t->col) & row_mask);
		}
	}
	return h;
}

/* doesn't handle the I piece! */
static int floor_kick(struct tetmino *t, unsigned rotated, unsigned box)
{
	if ((rotated & box) >> (PIECE_HEIGHT-1) * PIECE_WIDTH &&
	    !(rotated & (box << PIECE_WIDTH)) && !t->climbed) {
		t->climbed = 1;
		t->row++;
		if (t->falling) {
			t->falling = 1;
		}
		return 1;
	}
	return 0;
}

static int wall_kick(struct tetmino *t, unsigned rotated, unsigned box,
		     const blocks_row *blocks)
{
	unsigned overlap = rotated & box,
		 mask1 = TETMINO_I2 >> 1,	/* left column */
		 mask3 = TETMINO_I2 << 1;	/* right column */
	int	n = 0;

	if (rotated == TETMINO_I)
		n = (t->shape == TETMINO_I2) ? -1 : 1;
	else {
		/* kick if exactly one of the columns overlaps */
		if (overlap == (overlap & mask1))
			n = 1;
		if (overlap == (overlap & mask3))
			n = -1;
	}

	if (n && !(rotated & get_blocks_box(blocks, t->col + n))) {
		t->col += n;
		return n;
	}
	return 0;
}

static int rotate(struct tetmino *t, const blocks_row *blocks, int n)
{
	const unsigned short *rot_states = tetmino_shapes[t->piece];
	unsigned box = get_blocks_box(blocks, t->col);
	int kick = 1;
	int i;

	/* try both vertical orientations of I, S and Z */
	if (rot_states[0] == t->shape &&
	    rot_states[2] == t->shape && (rot_states[n] & box)) {
		n += 2;
		kick = 0;
	}

	for (i=0; i<4; i++) {
		if (rot_states[i] == t->shape) {
			unsigned rotated = rot_states[(i+n) & 3];
			if (rotated & box) {
				if (!kick)
					return 0;
				/* J3, L3, T3 */
				if (i == 2 && floor_kick(t, rotated, box))
					;
				else if (!wall_kick(t, rotated, box, blocks))
					return 0;
			}
			t->shape = rotated;
			return rotated ^ TETMINO_O;
		}
	}
	return 0;
}

static int drop(struct tetmino *t, const blocks_row *blocks, int n)
{
	int h = drop_height(t, blocks, n);
	t->row -= h;
	t->climbed -= h;
	if (t->climbed < 0)
		t->climbed = 0;
	return h;
}

int control_tetmino(struct tetmino *t, const blocks_row *blocks, enum action a)
{
	int moved = 0, dropped = 0, lock = 0;

	switch (a) {
	case ROTATE_CW:
		moved = rotate(t, blocks + t->row, 1);
		break;
	case ROTATE_CCW:
		moved = rotate(t, blocks + t->row, 3);
		break;
	case MOVE_RIGHT:
		t->col += (moved = can_move_tetmino(t, blocks, 1));
		break;
	case MOVE_LEFT:
		t->col -= (moved = can_move_tetmino(t, blocks, -1));
		break;
	case HARDDROP:
		dropped = drop(t, blocks, t->row);
		lock = 1;
		break;
	case SOFTDROP:
		dropped = drop(t, blocks, 1);
		lock = !dropped;
	default:
		break;
	}
	if (lock) {
		t->falling = 0;
		t->lock_delay_move = 0;
		t->lock_delay_step = 0;
		return lock;
	}

	/* any successful shifting or rotation */
	if (moved) {
		t->lock_delay_move = LOCK_DELAY_MOVE + 1;

	}
	return moved | dropped;
}

static void init_lock_delay(struct tetmino *t, int gravity)
{
	/* shorter initial lock delay at slower speeds */
	int lock_delay = LOCK_DELAY_MOVE - gravity;
	t->lock_delay_step = LOCK_DELAY_STEP;
	if (lock_delay > t->lock_delay_move)
		t->lock_delay_move = lock_delay;
}

int update_tetmino(struct tetmino *t, const blocks_row *blocks, int gravity)
{
	unfloat_tetmino(t, blocks);
	if (!t->falling) {
		if (t->lock_delay_move > 0)
			t->lock_delay_move--;
		if (t->lock_delay_step > 0)
			t->lock_delay_step--;
		return 0;
	}

	if (t->falling > gravity)
		t->falling = gravity;

	/* count down frames until drop */
	t->falling--;
	if (t->falling > 0)
		return 0;

	if (drop(t, blocks, 1)) {
		t->falling = gravity;
		return 1;
	} else {
		t->falling = 0;
		init_lock_delay(t, gravity);
		return 0;
	}
}

void unfloat_tetmino(struct tetmino *t, const blocks_row *blocks)
{
	if (!t->falling) {
		/* floor kicked */
		drop(t, blocks, t->climbed);

		t->falling = drop_height(t, blocks, 1);
	}
}
