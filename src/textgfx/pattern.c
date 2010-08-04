#include "textgfx.h"

#define BUF_SIZE 40

void put_pattern(const struct text *t, int n, int i)
{
	const unsigned short *us = t->str.u;
	char buf[BUF_SIZE];
	int j = 0;
	if (!t->len || n <= 0)
		return;
	i %= t->len;
	if (t->tag & TEXT_UCS2) {
		if (us[i] >= 0x100) {
			textgfx.putchar(us[i++]);
			textgfx.x++;
			n--;
		} else {
			for (; us[i] < 0x100 && j<n && j<BUF_SIZE-1; j++) {
				buf[j] = us[i];
				i = (i+1) % t->len;
			}
		}
	} else {
		for (; j<n && j<BUF_SIZE-1; j++) {
			buf[j] = t->str.c[i];
			i = (i+1) % t->len;
		}
	}
	buf[j] = '\0';
	if (t->tag & TEXT_ACS)
		textgfx.putacs(buf);
	else
		textgfx.puttext(buf);
	textgfx.x += j;
	put_pattern(t, n-j, i);
}

void draw_pattern_bits(const struct text *t, unsigned bits, int x, int y)
{
	int n;
	while (bits > 0x0f) {
		draw_pattern_bits(t, bits & 0x0f, x, y--);
		bits >>= 4;
	}
	while (bits) {
		while (!(bits & 1)) {
			x += 2;
			bits >>= 1;
		}
		n = 2;
		while ((bits >>= 1) & 1)
			n += 2;
		moveto(x, y);
		put_pattern(t, n, x);
		x = textgfx.x;
	}
}
