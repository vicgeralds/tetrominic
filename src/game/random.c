#include <stdlib.h>

int rand7()
{
	int x;
	do {
	    x = rand() / ((RAND_MAX + 1U) / 7);
	} while (x >= 7);

	return x;
}
