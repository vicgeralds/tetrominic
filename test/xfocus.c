#include <stdio.h>
#include <unistd.h>
#include "terminal.h"

int main()
{
	init_terminal();
	xwindow_init();
	if (terminal.wm_class[0])
		printf("%s, %s\n", terminal.wm_class[0],
				   terminal.wm_class[1]);
	while (terminal.has_focus())
		sleep(1);
	puts("Lost focus");
	xwindow_exit();
	return 0;
}
