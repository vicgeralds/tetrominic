#include <stdio.h>
#include <string.h>
#include "terminal.h"

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
		if (argc > 2 && !strcmp(argv[2], "cp850"))
			terminal.charenc = CHAR_CP850;
		else if (terminal.acs == ACS_UNICODE)
			terminal.charenc = CHAR_UTF8;
	}

	printf("Line drawing: %s\n\n", names[terminal.acs]);

	set_acs_table();

	char line[40] = "";
	for (i=0; i<31; i++)
		line[i] = 0x60+i;
	strcpy(line+i, " +,-. 0\n");
	tputstr(line);
	tputacs(line);

	putchar('\n');
	return 0;
}
