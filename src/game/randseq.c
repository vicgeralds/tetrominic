#include <stdlib.h>
#include "randseq.h"

/* pack 8 values into RANDSEQ_BITS bytes */
static void pack_randseq(unsigned char *seq, const unsigned char *values)
{
	word_randseq bytes = values[0];
	int i;
	for (i=1; i < 8; i++) {
		bytes |= values[i] << (i * RANDSEQ_BITS);
	}
	seq[0] = bytes;
	for (i=1; i < RANDSEQ_BITS; i++) {
		seq[i] = (bytes >>= 8);
	}
}

static word_randseq unpack_randseq(const unsigned char *seq)
{
	word_randseq bytes = seq[0];
	int i;
	for (i=1; i < RANDSEQ_BITS; i++) {
		bytes |= seq[i] << (8 * i);
	}
	return bytes;
}

/* generate 8 random numbers */
static void random_fun(unsigned char *values)
{
	long randval = 0;
	int i = 0;

	while (i < 8) {
		if (randval > 0) {
			int x = randval % (1 << RANDSEQ_BITS);
			if (x > 0 && x <= RANDSEQ_RANGE) {
				values[i] = x;
				i++;
			}
			randval >>= RANDSEQ_BITS;
		} else {
			randval = random() >> (31 % RANDSEQ_BITS);
		}
	}
}

int get_randseq(unsigned char *seq, int i)
{
	unsigned u;
	seq += i/8 * RANDSEQ_BITS;
	if (*seq == 0) {
		unsigned char values[8];
		random_fun(values);
		pack_randseq(seq, values);
	}
	u = unpack_randseq(seq) >> ((i & 7) * RANDSEQ_BITS);
	return u % (1 << RANDSEQ_BITS);
}
