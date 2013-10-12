/* Bitmap manipulation */

#define PIECE_WIDTH  4
#define PIECE_HEIGHT 4

void setpiece(unsigned char *bmp, int w, int h,
	      const unsigned char *piece, int x, int y, int mask);

/* convert bits to byte pixels (bit n to piece[n]).
	0 -> 0x00
	1 -> 0xFF
*/
void make_piece(unsigned char *piece, unsigned bits);
