/* Terminal input handling */

#include <stddef.h>

#define ESC '\033'
#define EOT '\004'

/* buffer for storing escape sequences */
struct terminal_seq_buffer {
	char s[16];
};

/* current: available input
 * next: a buffer to continue reading from, storing remaining input or an
 *       incomplete escape sequence.
 */
struct terminal_input {
	struct terminal_seq_buffer current, next;
	size_t next_length;
};

/*
 * Read an escape sequence or a single byte.
 *
 * Returns: length of string stored in current
 */
size_t read_terminal_seq(struct terminal_input *);
