#include "tetmino.h"
#include "tetgrid.h"
#include "tetfield.h"

static void stop_retrying_action(struct tetfield *tf)
{
	if (tf->last_action == HARDDROP) {
		tf->last_action = NO_ACTION;
		tf->timeout[RETRY_ACTION] = 0;
		tf->timeout[HARDDROP] += SPAWN_DELAY - AUTOREPEAT_FRAMES;
	}
	if (tf->last_action != NO_ACTION) {
		tf->last_action = NO_ACTION;
		tf->timeout[RETRY_ACTION] += SPAWN_DELAY - AUTOREPEAT_FRAMES;
	}
}

void enter_tetfield(struct tetfield *tf, int piece, int col)
{
	int delay = SPAWN_DELAY;
	if (delay < tf->gravity)
		delay = tf->gravity;
	init_tetmino(&tf->mino, piece, SPAWN_ROW + 1, col, delay + 1);
	tf->state = TETFIELD_SPAWN;
	/* prevent last action from cancelling spawn delay */
	stop_retrying_action(tf);
}

static int is_retrying_action(struct tetfield *tf)
{
	enum action a = tf->last_action;
	return a != NO_ACTION && tf->timeout[RETRY_ACTION] > tf->timeout[a];
}

static enum action make_move(struct tetfield *tf, enum action a)
{
	if (tf->timeout[a] == 0 && control_tetmino(&tf->mino, tf->grid->blocks, a)) {
		if (!is_retrying_action(tf) || tf->timeout[tf->last_action] == 0) {
			tf->last_action = a;
		}
		tf->timeout[RETRY_ACTION] = AUTOREPEAT_FRAMES;
		tf->timeout[a] = AUTOREPEAT_FRAMES;
		return a;
	}
	return NO_ACTION;
}

static void dec_timeout(struct tetfield *tf)
{
	int i;
	for (i=0; i < END_ACTION; i++) {
		if (tf->timeout[i])
			tf->timeout[i]--;
	}
}

/* process moves before the piece is spawned */
static void update_prespawn(struct tetfield *tf, enum action a)
{
	if (!tf->timeout[RETRY_ACTION]) {
		switch (a) {
		case ROTATE_CW:
		case ROTATE_CCW:
			tf->last_action = a;
			break;
		case MOVE_RIGHT:
		case MOVE_LEFT:
		case HARDDROP:
		case SOFTDROP:
			if (!tf->timeout[a]) {
				tf->mino.falling = 1;
				tf->timeout[a] = 1;
			}
			break;
		default:
			break;
		}
	}
}

static enum action update_move(struct tetfield *tf, enum action a)
{
	enum action moved = NO_ACTION;

	if (tf->timeout[a]) {
		if (tf->timeout[RETRY_ACTION]) {
			moved = make_move(tf, tf->last_action);
		}
		if (a != NO_ACTION) {
			tf->last_action = a;
			tf->timeout[RETRY_ACTION] = AUTOREPEAT_FRAMES;
		}
	} else if ((moved = make_move(tf, a)) == NO_ACTION) {
		if (a != NO_ACTION) {
			tf->last_action = a;
			tf->timeout[RETRY_ACTION] = WALL_CHARGE_FRAMES;
		}
	}

	return moved;
}

static int is_movable(const struct tetmino *t)
{
	return t->falling || (t->lock_delay_move && t->lock_delay_step);
}

/* check that there are no unfinished line clears at occupied rows */
static int is_lockable(const struct tetfield *tf)
{
	const blocks_row *blocks = tf->grid->blocks + tf->mino.row;
	int i;
	for (i=0; i < PIECE_HEIGHT; i++) {
		if (!(blocks[i] & LINE_CLEAR_MARK) && tetmino_has_row(tf->mino.shape, i))
			return 0;
	}
	return 1;
}

int run_tetfield(struct tetfield *tf, enum action a, struct changed *out)
{
	struct tetgrid *grid = tf->grid;

	dec_timeout(tf);
	out->moved = NO_ACTION;
	out->dropped = 0;

	switch (tf->state) {
	case TETFIELD_SPAWN:
		update_prespawn(tf, a);
		if (tf->mino.falling > 1 && grid->clearing) {
			/* stop spawn timer during line clear animation */
			tf->mino.falling++;
		}
		out->dropped = update_tetmino(&tf->mino, grid->blocks, tf->gravity);
		if (!out->dropped) {
			if (is_movable(&tf->mino) || grid->clearing)
				return 1;
			tf->state = TETFIELD_TOP_OUT;
			return 0;
		}
		tf->timeout[RETRY_ACTION] = 1;
		tf->state = TETFIELD_MOVE;
		out->moved = update_move(tf, a);
		break;
	case TETFIELD_MOVE:
		out->moved = update_move(tf, a);
		out->dropped = update_tetmino(&tf->mino, grid->blocks, tf->gravity);
		break;
	case TETFIELD_PLACED:
		/* accepting no more actions */
		out->dropped = control_tetmino(&tf->mino, grid->blocks, HARDDROP);
		break;
	case TETFIELD_TOP_OUT:
		return 0;
	}

	/* set timer to block actions for next tetromino during spawn delay */
	if (tf->timeout[RETRY_ACTION] == 1) {
		stop_retrying_action(tf);
	}

	update_removable_rows(grid, &tf->mino);

	/* return 0 on lock condition */
	return is_movable(&tf->mino) || (tf->state = TETFIELD_PLACED, !is_lockable(tf));
}

static void levelup(struct tetfield *tf)
{
	if (tf->gravity > 1) {
		tf->gravity *= 0.9;
		tf->pieces = 0;
	}
}

static int get_score_for_lines(struct tetfield *tf, int num_lines_cleared)
{
	int score = tf->grid->cols * num_lines_cleared;

	while (num_lines_cleared > 2) {
		score *= 2;
		num_lines_cleared -= 2;
	}

	if (num_lines_cleared > 1) {
		score *= 1.5;
	}

	int gravity = tf->gravity;

	int level = (30 - gravity) * 10 / gravity;

	if (gravity < 5) {
		level = (10 - gravity + tf->pieces / 25) * 10;
	}

	if (level > 0) {
		score *= level;
	}

	return score;
}

int lock_tetfield(struct tetfield *tf)
{
	struct tetgrid *grid = tf->grid;
	blocks_row *blocks = grid->blocks;
	int num_lines_cleared = 0;
	int i;

	/* add blocks */
	xor_tetmino(&tf->mino, blocks);

	/* find completed lines */
	blocks += tf->mino.row;
	for (i=0; i < PIECE_HEIGHT; i++) {
		blocks_row b = blocks[i] + 1;
		if (!b) {
			num_lines_cleared++;
			/* init line clear */
			blocks[i] ^= LINE_CLEAR_MARK;
			grid->clearing++;
			grid->delay += BLOCK_CLEAR_DELAY;
		}
	}

	/* update score */
	tf->score += get_score_for_lines(tf, num_lines_cleared);

	/* update level */
	tf->pieces++;
	if (tf->pieces >= 25 && num_lines_cleared) {
		levelup(tf);
	}

	return num_lines_cleared;
}

