#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include "input.h"
#include "terminal.h"

/* Get the length of an escape sequence/control sequence */
static size_t get_seq_len(const char *s, size_t buflen)
{
	size_t i = 0;

	if (buflen == 0) {
		return 0;
	}

	if (s[0]) {
		i = 1;
	}

	if (s[0] == ESC) {
		int final_byte_start = 0x30;

		/* CSI - Control Sequence Introducer */
		if (s[1] == '[') {
			final_byte_start = 0x40;
			i = 2;
			/* parameter bytes */
			while (s[i] >= 0x30 && s[i] < 0x40) {
				i++;
			}
		}

		/* SS3 - Single Shift G3 */
		if (s[1] == 'O') {
			i = 2;
		} else {
			/* intermediate bytes */
			while (s[i] >= 0x20 && s[i] < 0x30) {
				i++;
			}
		}

		/* final byte */
		if (s[i] >= final_byte_start && s[i] < 0x7f) {
			i++;
			return i;
		}

		if (i == buflen) {
			return 0;
		}

		/* not a valid escape sequence */
	}

	if (!s[i]) {
		/* count terminating null byte */
		return i + 1;
	}

	return i;
}

/* Truncate an incomplete sequence to max bytes */
static size_t truncate_seq(char *buf, size_t len, size_t max)
{
	if (len > max) {
		/* keep the last byte in case it's an intermediate */
		buf[max - 1] = buf[len - 1];
		return max;
	}

	return len;
}

static int check_stdin_ready()
{
	fd_set fds;
	struct timeval timeout;

	FD_ZERO(&fds);
	FD_SET(STDIN_FILENO, &fds);

	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	return select(STDIN_FILENO + 1, &fds, NULL, NULL, &timeout);
}

static size_t read_into_buffer(struct terminal_input *inp, size_t len)
{
	char *buf = inp->next.s + len;
	size_t nbytes =  sizeof(inp->next.s) - len;
	int ret = check_stdin_ready();

	memset(buf, '\0', nbytes);

	if (ret > 0) {
		ret = read(STDIN_FILENO, buf, nbytes - 1);
		if (ret == 0) {
			buf[0] = EOT;
			return 1;
		}
	} else if (ret == 0 && len > 0) {
		/* read even if stdin isn't ready if there's an incomplete
		 * escape sequence in the buffer
		 */
		ret = read(STDIN_FILENO, buf, nbytes - 1);
		if (ret == 0) {
			/* append a null byte to the input buffer */
			return 1;
		}
	}

	if (ret >= 0) {
		return ret;
	}

	if (errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK) {
		print_error("Error reading from stdin");
		restore_terminal();
		exit(EXIT_FAILURE);
	}

	return 0;
}

size_t read_terminal_seq(struct terminal_input *inp)
{
	char *buf = inp->next.s;
	size_t len = get_seq_len(buf, inp->next_length);

	if (len == 0) {
		/* make sure buffer is not full before reading */
		inp->next_length = truncate_seq(buf, inp->next_length, 8);

		len = read_into_buffer(inp, inp->next_length);
		if (len > 0) {
			inp->next_length += len;
			len = get_seq_len(buf, inp->next_length);
		}
	}

	if (len > 0) {
		inp->current = inp->next;
		if (len < inp->next_length) {
			inp->next_length -= len;
		} else {
			inp->next_length = 0;
		}
		/* move remaining input to next buffer */
		memcpy(buf, inp->current.s + len, inp->next_length + 1);
	}
	inp->current.s[len] = '\0';

	return len;
}

const char *read_terminal_keypress(struct terminal_input *inp)
{
	size_t len = read_terminal_seq(inp);
	const char *s = inp->current.s;

	if (!strcmp(s, " ")) {
		return "space";
	}

	if (len == 3 && s[0] == ESC && s[1] == '[') {
		switch (s[2]) {
		case 'A': return "up";
		case 'B': return "down";
		case 'C': return "right";
		case 'D': return "left";
		}
	}

	return s;
}
