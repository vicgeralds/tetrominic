
/* number of bits per item */
#define RANDSEQ_BITS 3

/* number of bytes used to store n items */
#define RANDSEQ_BYTES(n) (((n)+7)/8 * RANDSEQ_BITS)

/* generate random numbers between 1 and RANDSEQ_RANGE */
#define RANDSEQ_RANGE ((1 << RANDSEQ_BITS) - 1)

/* a word that can hold 8 items (RANDSEQ_BITS bytes) */
typedef long word_randseq;

int get_randseq(unsigned char *seq, int i);