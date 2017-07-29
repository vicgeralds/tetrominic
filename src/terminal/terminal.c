#include "terminal.h"

struct terminal terminal = {
	80,	/* width */
       	25, 	/* height */
	1,	/* lines */
	putacs_vt100,
	tputtext
};

void init_terminal()
{
}
