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

void drawacs(const char *s, int w, struct tg_buffer *line)
{
	int wrapcount = w;

	while (*s && *s != ')') {
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
				drawacs(s, w, line);

			s = match_paren(s);
			wrapcount = w;		/* wrapping state is lost */
		}

		for (; repeat >= 0; repeat--) {
			drawchar(c, ALTCHARSET | line->attr, line);
			wrapcount--;
			/* wrapcount < 0 if w is zero */
			if (wrapcount == 0) {
				flush_tg(line);
				line->y++;
				line->x -= w;
				wrapcount = w;
			}
		}
	}
}

static void draw_tile(int x, Tiles tiles, struct tg_buffer *line)
{
	int n1 = (x & 0xF0) >> 4,
	    n2 = (x & 0x0F);
	drawchar(tiles->chars[n1], tiles->attrs[n1], line);
	drawchar(tiles->chars[n2], tiles->attrs[n2], line);
}

void drawtiles(unsigned char *front, const unsigned char *back,
	       int w, int h, int x, int y, Tiles tiles)
{
	struct tg_buffer line;
	line.y = y;
	line.len = 0;

	for (; h > 0; h--) {
		const unsigned char *nextRow = back + w;
		line.x = x;

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
