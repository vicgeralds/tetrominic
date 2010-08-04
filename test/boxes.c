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

	drawbox(2,0, 10,5, "lquxhxtqu");
	drawbox(2,5, 10,4, "xhxxhxtqu");
	drawbox(2,9, 10,4, "xhxxhxtqu");
	drawbox(2,13,10,4, "xhxx xmqu");

	drawbox  (14,0, 40,3, NULL);
	drawframe(15,5, 23,5, "Hello world!");

	textgfx_end();

	return 0;
}
