#include <unistd.h>
#include <termios.h>
#include "terminal.h"

static struct termios saved_term_attr;

void set_input_mode()
{
	struct termios attr;
	tcgetattr(STDIN_FILENO, &saved_term_attr);
	tcgetattr(STDIN_FILENO, &attr);
	/* set noncanonical mode and disable echoing */
	attr.c_lflag &= ~(ICANON|ECHO|ECHONL);
	/* read with timeout (0.2 seconds) */
	attr.c_cc[VMIN] = 0;
	attr.c_cc[VTIME] = 2;
	tcsetattr(STDIN_FILENO, TCSADRAIN, &attr);
}

void restore_input_mode()
{
	tcsetattr(STDIN_FILENO, TCSANOW, &saved_term_attr);
}
