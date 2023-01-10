#include <myshogi.h>

#include <ctype.h>
#include <errno.h>
#include <error.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define OPTPARSE_IMPLEMENTATION
#include <optparse.h>

#include <popen3.h>

#ifndef STRINGIFY
#define STRINGIFY(X) #X
#endif

#ifndef GNUSHOGI_COMMAND
#define GNUSHOGI_COMMAND gnushogi
#endif

#ifndef BUFFSIZE_GNUSHOGI
#define BUFFSIZE_GNUSHOGI 1024ul
#endif

#ifndef BUFFSIZE_SAVE
#define BUFFSIZE_SAVE 1024u
#endif

#define GNUSHOGI_COMMAND_STRING STRINGIFY(GNUSHOGI_COMMAND)

int main(int argc, char **argv)
{
	struct optparse_long longopts[] = {
		{ "help", 'h', OPTPARSE_NONE },
		{ "new", 'n', OPTPARSE_NONE },
		{ "save_as", 's', OPTPARSE_REQUIRED },
		{ "view", 'l', OPTPARSE_REQUIRED },
		{ "gnushogi-path", 'x', OPTPARSE_OPTIONAL },
		{ NULL }
	};
	struct optparse options;
	optparse_init(&options, argv);
	int option;
	while ((option = optparse_long(&options, longopts, NULL)) != -1) {
		switch (option) {
		case 'h':
			printf("Description:\n"
			       "  myshogi -n -s FILENAME\n [-x path/to/gnushogi]\n"
			       "  myshogi -l FILENAME\n [-x path/to/gnushogi]\n");
			break;
		case 'n':
			break;
		}
	}
	fputs("Starting GNU Shogi ...\n", stderr);
	int fd_gnushogi_in = -1, fd_gnushogi_out = -1, fd_gnushogi_err = -1;
	pid_t pid_gnushogi = popen3(GNUSHOGI_COMMAND_STRING, &fd_gnushogi_in,
				    &fd_gnushogi_out, &fd_gnushogi_err);
	pid_t pid_errprinter = fork();
	if (pid_errprinter == -1) {
		perror("errprinter: failed to start.\n");
		return EXIT_FAILURE;
	}
	if (!pid_errprinter) {
		errprinter(fd_gnushogi_err, pid_gnushogi, "gnushogi");
		return EXIT_SUCCESS;
	}
	FILE *pf_gnushogi_out = fdopen(fd_gnushogi_out, "r"),
	     *pf_gnushogi_in = fdopen(fd_gnushogi_in, "w");
	// Wait till the first line being printed
	while (fputc(fgetc(pf_gnushogi_out), stdout) != '\n')
		;
	// Both players will be human.
	// GNU Shogi can be an AI player, but to utilize that will inclease the complexity of this code base.
	fputs("force\n", pf_gnushogi_in);
	char buffer[BUFFSIZE_GNUSHOGI];
	char buffer_save[BUFFSIZE_SAVE];
	while (true) {
		puts("> ");
		char string_peek = '\0';
		while (isspace(string_peek = fgetc(stdin)) &&
		       string_peek != '\n')
			;
		if (string_peek == '\n')
			continue;
		ungetc(string_peek, stdin);
		scanf("%s", buffer);
		while (fgetc(stdin) != '\n')
			;
		if (!strcmp(buffer, "S") || !strcmp(buffer, "s")) {
			;
		} else if (buffer[0] == '0') {
			fputs("remove\n", pf_gnushogi_in);
		}
		char str_to_translate[4];
		if (buffer[1] == '*') {
			str_to_translate[0] = buffer[0] < 'a' ?
						      buffer[0] :
						      (char)(buffer[0] - ('a' - 'A'));
		}
		fputs(buffer, pf_gnushogi_in);
	}
	return EXIT_SUCCESS;
}
