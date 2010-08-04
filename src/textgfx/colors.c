#include "textgfx.h"

unsigned colors[NUM_COLORS] = {
	0,
	SGR_PARAMS(BOLD, 31, 41),
	SGR_PARAMS(BOLD, 32, 42),
	SGR_PARAMS(BOLD, 33, 43),
	SGR_PARAMS(BOLD, 34, 44),
	SGR_PARAMS(BOLD, 35, 45),
	SGR_PARAMS(BOLD, 36, 46),
	SGR_PARAMS(BOLD, 37, 47)
};

void setcolor(int i)
{
	set_sgr_params(colors[i]);
}
