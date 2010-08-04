#include <stdio.h>
#include <string.h>
#include "terminal.h"
#include "textgfx.h"

int main(int argc, char **argv)
{
	init_terminal();
	gettermtype();
	init_acs_caps();
	terminal.acs = ACS_VT100;

	const char *names[8] = {0};
	names[ACS_ASCII]   = "ascii";
	names[ACS_VT100]   = "vt100";
	names[ACS_IBM]     = "ibm";
	names[ACS_UNICODE] = "unicode";

	int i;
	if (argc > 1) {
		for (i=0; i<8; i++) {
			if (names[i] && !strcmp(argv[1], names[i]))
				terminal.acs = i;
		}
		if (terminal.acs == ACS_UNICODE)
			terminal.charenc = CHAR_UTF8;
	}

	printf("Line drawing: %s\n\n", names[terminal.acs]);

	set_acs_table();
	textgfx_init();

	struct text t;
	t.tag = TEXT_ACS;
	t.str.c = "ah h";
	t.len = strlen(t.str.c);
	moveto(0,4);
	setcolor(4);
	draw_pattern_bits(&t, 0x8c63, 0, 4);
	draw_pattern_bits(&t, 0xac63, 7, 4);

	unsigned short us[] = {0x2573, 'X', '~', 'x', 0x2715, 'n'};
	t.tag |= TEXT_UCS2;
	t.len = 6;
	t.str.u = us;
	moveto(3, 0);
	setcolor(1);
	put_pattern(&t, 50, 0);

	textgfx_end();

	return 0;
}
