#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#include "terminal.h"

struct terminal terminal = {
	80,	/* width */
       	25, 	/* height */
	1,	/* lines */
	putacs_vt100,
	puttext_unicode
};

static struct termios saved_term_attr;

static int set_input_mode()
{
	if (tcgetattr(STDIN_FILENO, &saved_term_attr) == 0) {
		struct termios attr = saved_term_attr;

		/* set noncanonical mode and disable echoing */
		attr.c_lflag &= ~(ICANON|ECHO);
		/* read with timeout (0.2 seconds) */
		attr.c_cc[VMIN] = 0;
		attr.c_cc[VTIME] = 2;

		return tcsetattr(STDIN_FILENO, TCSADRAIN, &attr) == 0;
	}

	return 0;
}

static void restore_input_mode()
{
	tcsetattr(STDIN_FILENO, TCSANOW, &saved_term_attr);
}

static void restore_terminal()
{
	set_text_attr(0);
	moveto(terminal.x0, terminal.y0 + terminal.lines);
	show_cursor();
	restore_input_mode();
}

int init_terminal()
{
	get_terminal_size();
	if (set_input_mode()) {
		hide_cursor();
		return atexit(restore_terminal) == 0;
	}
	return 0;
}

void get_terminal_size()
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
