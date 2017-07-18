#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#include "terminal.h"

void gettermsize()
{
#if defined(TIOCGSIZE)
	struct ttysize tty;
	if (!ioctl(STDIN_FILENO, TIOCGSIZE, &tty)) {
		terminal.width  = tty.ts_cols;
		terminal.height = tty.ts_lines;
	}
#elif defined(TIOCGWINSZ)
	struct winsize win;
	if (!ioctl(STDIN_FILENO, TIOCGWINSZ, &win)) {
		terminal.width  = win.ws_col;
		terminal.height = win.ws_row;
	}
#endif
}
