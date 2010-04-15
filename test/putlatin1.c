#include <stdio.h>
#include <string.h>
#include "terminal.h"

int main(int argc, char **argv)
{
	init_terminal();
	terminal.charenc = CHAR_ISO8859_1;

	const char *names[8] = {0};
	names[CHAR_ISO8859_1] = "iso8859-1";
	names[CHAR_ASCII] = "ascii";
	names[CHAR_CP850] = "cp850";
	names[CHAR_CP437] = "cp437";
	names[CHAR_UTF8]  = "utf8";

	int i;
	if (argc > 1) {
		for (i=0; i<8; i++) {
			if (names[i] && !strcmp(argv[1], names[i]))
				terminal.charenc = i;
		}
	}
	printf("Printing Latin-1 Supplement characters using %s\n\n",
		names[terminal.charenc]);

	set_latin1_codepage();

	char line[40] = "";
	for (i=0; i<32; i++)
		line[i] = 0xa0+i;
	line[i] = '\n';
	tputtext(line);
	
	for (i=0; i<32; i++)
		line[i] = 0xc0+i;
	line[i] = '\n';
	line[i+1] = '\0';
	tputtext(line);

	for (i=0; i<32; i++)
		line[i] = 0xe0+i;
	line[i] = '\n';
	line[i+1] = '\0';
	tputtext(line);

	putchar('\n');
	return 0;
}
