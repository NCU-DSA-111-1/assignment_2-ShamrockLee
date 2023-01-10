#ifndef __MYSHOGI_H__
#define __MYSHOGI_H__

#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include "shogi_piecename2kanji_table.h"

#ifndef BUFFSIZE_ERRPRINTER
#define BUFFSIZE_ERRPRINTER 1024ul
#endif

char absorb_till_lf(FILE *pf_read)
{
	while (true) {
		const char c = fgetc(pf_read);
		if (!c || c == '\n')
			return c;
	}
}

char pass_till_lf(FILE *pf_write, FILE *pf_read)
{
	// fputs("(passing)", pf_write);
	while (true) {
		const char c = fgetc(pf_read);
		fputc(c, pf_write);
		if (!c || c == '\n')
			return c;
	}
}

void errprinter(const int fd_target_err, const pid_t pid_target,
		const char *const name);

#ifndef BOARD_LENGTH
#define BOARD_LENGTH 9
#endif

void translate_board(FILE *pf_write, FILE *pf_read);

#endif /* __MYSHOGI_H__ */
