#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#include "terminal.h"

struct terminal terminal = {
	80,	/* width */
	25,	/* height */
	1,	/* lines */
	putacs_vt100,
	puttext_ibm
};

static struct termios saved_term_attr;

static int set_input_mode()
{
	struct termios attr = saved_term_attr;

	if (attr.c_lflag == 0) {
		return 0;
	}

	/* set noncanonical mode and disable echoing */
	attr.c_lflag &= ~(ICANON|ECHO);
	/* read with timeout (0.2 seconds) */
	attr.c_cc[VMIN] = 0;
	attr.c_cc[VTIME] = 2;

	return tcsetattr(STDIN_FILENO, TCSANOW, &attr) == 0;
}

static int restore_input_mode()
{
	if (saved_term_attr.c_lflag == 0) {
		return 0;
	}

	return tcsetattr(STDIN_FILENO, TCSANOW, &saved_term_attr) == 0;
}

static void get_terminal_size()
{
#if defined(TIOCGSIZE)
	struct ttysize tty;
	if (!ioctl(STDOUT_FILENO, TIOCGSIZE, &tty)) {
		terminal.width  = tty.ts_cols;
		terminal.height = tty.ts_lines;
	}
#elif defined(TIOCGWINSZ)
	struct winsize win;
	if (!ioctl(STDOUT_FILENO, TIOCGWINSZ, &win)) {
		terminal.width  = win.ws_col;
		terminal.height = win.ws_row;
	}
#endif
}

int init_terminal()
{
	if (!isatty(STDOUT_FILENO)) {
		return 0;
	}
	tcgetattr(STDIN_FILENO, &saved_term_attr);
	setup_terminal();
	return 1;
}

void setup_terminal()
{
	if (set_input_mode()) {
		hide_cursor();
	}
	get_terminal_size();
	set_text_attr(0);
	clearscreen();
}

void restore_terminal()
{
	set_text_attr(0);
	clearscreen();
	if (restore_input_mode()) {
		show_cursor();
	}
	fflush(stdout);
}
