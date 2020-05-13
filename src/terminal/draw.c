#include "textgfx.h"
#include "terminal.h"

typedef const struct tg_tiles *Tiles;

void drawchar(int c, int attr, struct tg_buffer *line)
{
	if (attr != line->attr || line->len >= sizeof(line->buf)-1) {
		flush_tg(line);
		line->attr = attr;
	}
	line->buf[line->len++] = c;
}

void flush_tg(struct tg_buffer *line)
{
	int len  = line->len;
	int attr = line->attr;

	if (len > 0) {
		line->buf[len] = '\0';

		moveto(line->x, line->y);
		if (attr & ALTCHARSET) {
			set_text_attr(attr | VT100_GRAPHICS);
			terminal.putacs(line->buf);
		} else {
			set_text_attr(attr | ALTCHARSET);
			terminal.puttext(line->buf);
		}
		line->x = (terminal.cursor_x += len);
	}
	line->len = 0;
}

/* move past matching right parenthesis */
static const char *match_paren(const char *s)
{
	int count = 1;

	for (; *s && count > 0; s++) {
		if (*s == '(') count++;
		if (*s == ')') count--;
	}
	return s;
}

void drawacs(const char *s, int w, struct tg_buffer *line, int x0)
{
	while (*s && *s != ')' && w > 0) {
		int c = *s++;
		int repeat = 0;

		/* convert positive number */
		if (*s > '0' && *s <= '9') {
			do {
				repeat = *s - '0' + 10*repeat;
				s++;
			} while (*s >= '0' && *s <= '9');
		}

		if (c == '(') {
			for (; repeat >= 0; repeat--)
				drawacs(s, w, line, x0);

			s = match_paren(s);
		}

		for (; repeat >= 0; repeat--) {
			if (line->x + line->len >= x0 + w) {
				flush_tg(line);
				line->y++;
				line->x = x0;
			}
			drawchar(c, ALTCHARSET | line->attr, line);
		}
	}
}

void draw_acs(const char *s, int w, int x, int y, int attr)
{
	struct tg_buffer line;
	line.x = x;
	line.y = y;
	line.len = 0;
	line.attr = attr;
	drawacs(s, w, &line, x);
	flush_tg(&line);
}

static void draw_tile(int nibbles, Tiles tiles, struct tg_buffer *line)
{
	int n1 = (nibbles & 0xF0) >> 4,
	    n2 = (nibbles & 0x0F);
	drawchar(tiles->chars[n1], tiles->attrs[n1], line);
	drawchar(tiles->chars[n2], tiles->attrs[n2], line);
}

void drawtiles(unsigned char *front, const unsigned char *back,
	       int w, int h, int x, int y, Tiles tiles)
{
	struct tg_buffer line;
	line.y = y;
	line.len = 0;
	line.attr = 0;

	for (; h > 0; h--) {
		const unsigned char *nextRow = back + w;
		line.x = x;

		if (line.y < terminal.y0) {
			if (front) front += w;
			back = nextRow;
		}

		while (back < nextRow) {
			if (front) {
				/* advance cursor position when unchanged */
				if (*front == *back) {
					flush_tg(&line);
					do {
						line.x += 2;
						front++; back++;
					} while (back < nextRow &&
						 *front == *back);
					continue;
					/* end iteration */
				}
				*front = *back;
				front++;
			}
			draw_tile(*back, tiles, &line);
			back++;
		}
		flush_tg(&line);
		line.y++;
	}
}
