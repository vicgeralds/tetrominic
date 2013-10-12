#include "tetmino.h"
#include "tetfield.h"

void init_tetfield(struct tetfield *f, int cols)
{
	int i;
	f->blocks[0] = MAKE_FLOOR(cols);
	for (i=1; i < PLAYFIELD_HEIGHT; i++)
		f->blocks[i] = MAKE_WALLS(cols);
}

static enum action make_move(struct tetfield *f, enum action a)
{
	if (f->timeout[a] == 0 && control_tetmino(&f->mino, f->blocks, a)) {
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
	f->dropped = update_tetmino(&f->mino, f->blocks, f->gravity);

	/* return 0 on lock condition */
	return (f->mino.falling | f->mino.lock_delay_move
	                        | f->mino.lock_delay_step);
}