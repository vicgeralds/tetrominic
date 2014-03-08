#include <string.h>
#include "tetmino.h"
#include "tetfield.h"

void init_tetgrid(struct tetgrid *grid, int cols)
{
	int i;
	grid->blocks[0] = MAKE_FLOOR(cols);
	for (i=1; i < PLAYFIELD_HEIGHT; i++)
		grid->blocks[i] = MAKE_WALLS(cols);
}

void enter_tetfield(struct tetfield *f, int piece, int col)
{
	f->mino.shape = 0;
	f->mino.piece = piece;
	f->mino.row = SPAWN_ROW + 1;	/* drop 1 row to test top out */
	f->mino.col = col;
	f->mino.falling = 1;
	memset(f->timeout, SPAWN_DELAY, END_ACTION);
}

static enum action make_move(struct tetfield *f, enum action a)
{
	if (f->timeout[a] == 0 && control_tetmino(&f->mino, f->grid->blocks, a)) {
		f->charge = NO_ACTION;
		f->timeout[NO_ACTION] = 0;
		f->timeout[a] = AUTOREPEAT_FRAMES;
		return a;
	}
	return NO_ACTION;
}

static void dec_timeout(struct tetfield *f)
{
	int i;
	for (i=0; i < END_ACTION; i++) {
		if (f->timeout[i])
			f->timeout[i]--;
	}
}

static int update_gravity(struct tetfield *f)
{
	int gravity = f->gravity;
	/* set minimum frames until first drop */
	if (f->mino.row >= SPAWN_ROW && gravity < SPAWN_GRAVITY)
		gravity = SPAWN_GRAVITY;

	if (!f->mino.shape) {
		/* spawn delay */
		if (f->timeout[ROTATE_CW])
			return 0;
		f->mino.shape = tetmino_shapes[f->mino.piece][0];
		f->mino.falling = 0;

		/* discard drops during spawn delay */
		if (f->charge == HARDDROP || f->charge == SOFTDROP)
			f->charge = NO_ACTION;
	}
	return update_tetmino(&f->mino, f->grid->blocks, gravity);
}

int run_tetfield(struct tetfield *f, enum action a)
{
	/* timeout[NO_ACTION] is number of times to retry charged action */
	if (f->timeout[NO_ACTION] == 0)
		f->charge = NO_ACTION;

	if (f->timeout[a]) {
		f->moved = make_move(f, f->charge);
		if (a != NO_ACTION) {
			f->charge = a;
			f->timeout[NO_ACTION] = AUTOREPEAT_FRAMES;
		}
	} else if ((f->moved = make_move(f, a)) == NO_ACTION) {
		f->charge = a;
		f->timeout[NO_ACTION] = WALL_CHARGE_FRAMES;
	}

	dec_timeout(f);
	f->dropped = update_gravity(f);

	/* return 0 on lock condition */
	return (f->mino.falling | f->mino.lock_delay_move
	                        | f->mino.lock_delay_step);
}
