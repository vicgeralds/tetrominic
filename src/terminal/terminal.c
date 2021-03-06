#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <errno.h>
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
#ifdef __EMSCRIPTEN__
	return EM_ASM_INT(
		if (process.stdin.isTTY) process.stdin.setRawMode(true);
		return +process.stdin.isTTY || 0;
	);
#else
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
#endif
}

static int restore_input_mode()
{
#ifdef __EMSCRIPTEN__
	return EM_ASM_INT(
		if (process.stdin.isTTY) process.stdin.setRawMode(false);
		return +process.stdin.isTTY || 0;
	);
#else
	if (saved_term_attr.c_lflag == 0) {
		return 0;
	}

	return tcsetattr(STDIN_FILENO, TCSANOW, &saved_term_attr) == 0;
#endif
}

static void get_terminal_size(int fd)
{
#if defined(TIOCGSIZE)
	struct ttysize tty;
	if (!ioctl(fd, TIOCGSIZE, &tty)) {
		terminal.width  = tty.ts_cols;
		terminal.height = tty.ts_lines;
	}
#elif defined(TIOCGWINSZ)
	struct winsize win;
	if (!ioctl(fd, TIOCGWINSZ, &win)) {
		terminal.width  = win.ws_col;
		terminal.height = win.ws_row;
	}
#endif
}

int init_terminal()
{
#ifdef __EMSCRIPTEN__
	if (!EM_ASM_INT(return +process.stdout.isTTY || 0)) {
		return 0;
	}
#else
	if (!isatty(STDOUT_FILENO)) {
		return 0;
	}
	tcgetattr(STDIN_FILENO, &saved_term_attr);
#endif
	setup_terminal();
	return 1;
}

void setup_terminal()
{
	int fd = STDOUT_FILENO;
	if (set_input_mode()) {
		hide_cursor();
		fd = STDIN_FILENO;
	}
#ifdef __EMSCRIPTEN__
	terminal.width = EM_ASM_INT(return process.stdout.columns);
	terminal.height = EM_ASM_INT(return process.stdout.rows);
#else
	get_terminal_size(fd);
#endif
	set_text_attr(0);
	clearscreen();
}

void restore_terminal()
{
	set_text_attr(0);
	if (terminal.lines != 1 || terminal.x0 != terminal.cursor_x) {
		moveto(terminal.x0, terminal.y0 + terminal.lines);
	}
	terminal.cursor_y = 0;
	terminal.y0 = 0;
	clearscreen();
	if (restore_input_mode()) {
		show_cursor();
	}
	flush_output();
}

void print_error(const char *s)
{
	int saved_errno = errno;
	set_text_attr(0);
	clearscreen();
	flush_output();
	errno = saved_errno;
	perror(s);
}
