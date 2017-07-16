#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "terminal.h"

void gettermtype()
{
	char *s = getenv("TERM");
	if (s) {
		if (!strncmp(s, "xterm", 5))
			terminal.type = XTERM;
		else if (!strcmp(s, "linux"))
			terminal.type = LINUX_TERM;
		else if (!strcmp(s, "cygwin"))
			terminal.type = CYGWIN;
	}
	/* Gnome terminal no longer sets COLORTERM=gnome-terminal */
	s = getenv("COLORTERM");
	if ((s && !strcmp(s, "gnome-terminal")) ||
	    ((s = getenv("VTE_VERSION")) && isdigit(*s)))
		terminal.type |= GNOME_TERM;
}
