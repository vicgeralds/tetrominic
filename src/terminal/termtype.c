#include <stdlib.h>
#include <string.h>
#include "terminal.h"

void gettermtype()
{
	char *s = getenv("TERM");
	if (s) {
		if (!strncmp(s, "xterm", 5))
			terminal.type = XTERM;
		else if (!strcmp(s, "linux")) {
			terminal.type = LINUX_TERM;
			terminal.acs_disabled = "oprs";
		}
		else if (!strcmp(s, "cygwin"))
			terminal.type = CYGWIN;
	}
	s = getenv("COLORTERM");
	if (s && !strcmp(s, "gnome-terminal"))
		terminal.type |= GNOME_TERM;
}
