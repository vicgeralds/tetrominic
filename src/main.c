#include <langinfo.h>
#include <locale.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <time.h>
#include <errno.h>

#include "terminal/terminal.h"
#include "terminal/acstext.h"
#include "terminal/input.h"

#include "game/game.h"

static volatile sig_atomic_t signal_caught;

static void catch_signal(int sig)
{
	signal_caught = sig;
}

static int process_signal()
{
	int sig = signal_caught;

	signal_caught = 0;

	if (sig == SIGINT || sig == SIGTERM || sig == SIGTSTP) {
		restore_terminal();

		/* call default signal handler */
		signal(sig, SIG_DFL);
		raise(sig);
	} else if (sig) {
		/* resumed or resized */
		if (sig == SIGCONT) {
			signal(SIGTSTP, catch_signal);
		}
		setup_terminal();
	}
	return sig;
}

static int is_linux_console()
{
	char *s = getenv("TERM");

	return s && !strcmp(s, "linux");
}

static void detect_charset()
{
	if (!strcmp(nl_langinfo(CODESET), "UTF-8")) {
		terminal.puttext = puttext_unicode;
		terminal.putacs = putacs_text;
	} else if (is_linux_console()) {
		terminal.putacs = putacs_text;
	}
}

static void wait_one_frame()
{
	struct timeval timeout = {0};
	fd_set fds;

	FD_ZERO(&fds);
	FD_SET(STDIN_FILENO, &fds);

	timeout.tv_usec = GAME_FRAME_TIME;

	if (select(STDIN_FILENO + 1, &fds, NULL, NULL, &timeout) < 0) {
		if (errno != EINTR) {
			print_error("select");
			restore_terminal();
			exit(EXIT_FAILURE);
		}
	}
}

static void run_game_loop()
{
	struct terminal_input input = {{""}};
	struct game game;

	init_game(&game, 20);

	while (update_game(&game, input.current.s)) {
		wait_one_frame();
		process_signal();
		read_terminal_seq(&input);
		process_signal();
		if (terminal.lines == 1) {
			game.blocks.rendered = 0;
		}
	}
}

int main(int argc, char **argv)
{
	signal(SIGINT, catch_signal);
	signal(SIGTERM, catch_signal);
	signal(SIGCONT, catch_signal);
	signal(SIGTSTP, catch_signal);
#ifdef SIGWINCH
	signal(SIGWINCH, catch_signal);
#endif

	setlocale(LC_CTYPE, "");

	if (!init_terminal()) {
		return EXIT_FAILURE;
	}
	detect_charset();

	/* seed random number generator */
	srand(time(NULL));

	run_game_loop();

	terminal.y0 += terminal.lines;
	terminal.lines = 0;
	restore_terminal();

	return EXIT_SUCCESS;
}
