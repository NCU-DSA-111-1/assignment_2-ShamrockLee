#include <errno.h>
#include <error.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include <optparse.h>
#include <popen3.h>

#ifndef STRINGIFY
#define STRINGIFY(X) #X
#endif

#ifndef GNUSHOGI_COMMAND
#define GNUSHOGI_COMMAND gnushogi
#endif

#ifndef BUFFSIZE_ERRPRINTER
#define BUFFSIZE_ERRPRINTER 1024ul
#endif

#ifndef BUFFSIZE_GNUSHOGI
#define BUFFSIZE_GNUSHOGI 1024ul
#endif

#ifndef BUFFSIZE_SAVE
#define BUFFSIZE_SAVE 1024u
#endif

#ifndef BOARD_LENGTH
#define BOARD_LENGTH 9
#endif

static const char *const a_roles[] = {
	"K王",
	"G金",
	"S銀",
	"N桂",
	"L香",
	"R飛",
	"B角",
	"P兵",
};

static const char *const a_roles_fast[20] = {
	NULL, "角", NULL, NULL, NULL, // ABCDE
	NULL, "金", NULL, NULL, NULL, // FGHIJ
	"王", "香", NULL, "桂", NULL, // KLMNO
	"步", NULL, "飛", "銀", NULL, // PQRST
};



#define GNUSHOGI_COMMAND_STRING STRINGIFY(GNUSHOGI_COMMAND)

void errprinter(const int fd_target_err, const pid_t pid_target, const char* const name) {
	FILE *pf_target_err = fdopen(fd_target_err, "r");
	char buffer_errprinter[BUFFSIZE_ERRPRINTER];
	while (kill(pid_target, 0), errno != ESRCH) {
		fscanf(pf_target_err, "%s", buffer_errprinter);
		fprintf(stderr, "%s stderr: %s", name, buffer_errprinter);
		while (fputc(fgetc(pf_target_err), stderr) != '\n');
	}
	dup2(pf_target_err, STDERR_FILENO);
	fclose(pf_target_err);
	close(fd_target_err);
}

void translate_board(FILE *pf_write, FILE *pf_read) {
	char *buffer[128];
	// Transfer the 1st line (blank line)
	while (fputc(fgetc(pf_read), pf_write) != '\n');
	// The 2nd line is the time left
	int min_rmn_b = 0, secmmin_rmn_b = 0, min_rmn_w = 0, secmmin_rmn_w = 0;
	fscanf(pf_read, "Black %i:%i  White %i:%i", &min_rmn_b, &secmmin_rmn_b, &min_rmn_w, &secmmin_rmn_w);
	fprintf(pf_write, "Time left: Black %i:%i, White %i:%i");
	while (fputc(fgetc(pf_read), pf_write) != '\n');
	// The 3rd line (blank line)
	while (fputc(fgetc(pf_read), pf_write) != '\n');
	// The square board starts from the 4th line to the 4 - 1 + BOARD_LENGTH line,
	// where there is always exactly one space between each alphabet representing the rows.
	// The white goes up and use upper case letters.
	for (int i = 0; i < BOARD_LENGTH; ++i) {
		for (int j = 0; j < BOARD_LENGTH; ++i) {
			fgetc(pf_read);
			const char role = fgetc(pf_read);
			if (role == '-') {
				fputs("  ", pf_write);
				continue;
			} else {
				const bool is_black = role >= 'a';
				const char *const role_zh = a_roles_fast[role - (is_black ? 'a' : 'A')];
				fputs(role_zh, pf_write);
			}
		}
		while (fputc(fgetc(pf_read), pf_write) != '\n');
	}
	// Following is a blank line
	while (fputc(fgetc(pf_read), pf_write) != '\n');
	// Then the pieces taken by the players. First black and then white.
	for (int i_player = 0; i_player < 1; ++i_player){
		fscanf(pf_read, i_player ? "white" : "black");
		char c_space = '\0';
		while ((c_space = fputc(fgetc(pf_read), pf_write)) == '\n') {
			// The number of pieces of this role (<= 8)
			fputc(fgetc(pf_read), pf_write);
			// The role of pieces (upper case)
			fputs(a_roles_fast[fgetc(pf_read)], pf_write);
		}
		while (c_space != '\n') c_space = fputc(fgetc(pf_read), pf_write);
	}
}

int main(int argc, char **argv) {
	struct optparse_long longopts[] = {
		{"help", 'h', OPTPARSE_NONE},
		{"new", 'n', OPTPARSE_NONE},
		{"save_as", 's', OPTPARSE_REQUIRED},
		{"view", 'l', OPTPARSE_REQUIRED},
		{"gnushogi-path", 'x', OPTPARSE_OPTIONAL},
		{NULL}
	};
	struct optparse options;
	optparse_init(&options, argv);
	int option;
	while ((option = optparse_long(&options, longopts, NULL)) != -1) {
		switch (option) {
			case 'h':
			printf(
				"Description:\n"
				"  myshogi -n -s FILENAME\n [-x path/to/gnushogi]\n"
				"  myshogi -l FILENAME\n [-x path/to/gnushogi]\n"
			);
			break;
			case 'n':
			break;
		}
	}
	fputs("Starting GNU Shogi ...\n", stderr);
	int fd_gnushogi_in = -1, fd_gnushogi_out = -1, fd_gnushogi_err = -1;
	pid_t pid_gnushogi = popen3(GNUSHOGI_COMMAND_STRING, &fd_gnushogi_in, &fd_gnushogi_out, &fd_gnushogi_err);
	pid_t pid_errprinter = fork();
	if (pid_errprinter == -1) {
		perror("errprinter: failed to start.\n");
		return EXIT_FAILURE;
	} if (!pid_errprinter) {
		errprinter(fd_gnushogi_err, pid_gnushogi, "gnushogi");
		return EXIT_SUCCESS;
	}
	FILE *pf_gnushogi_out = fdopen(fd_gnushogi_out, "r"), *pf_gnushogi_in = fdopen(fd_gnushogi_in, "w");
	// Wait till the first line being printed
	while (fputc(fgetc(pf_gnushogi_out), stdout) != '\n');
	// Both players will be human.
	// GNU Shogi can be an AI player, but to utilize that will inclease the complexity of this code base.
	fputs("force\n", pf_gnushogi_in);
	char buffer[BUFFSIZE_GNUSHOGI];
	char buffer_save[BUFFSIZE_SAVE];
	while (true) {
		puts("> ");
		char string_peek = '\0';
		while (isspace(string_peek = fgetc(stdin)) && string_peek != '\n');
		if (string_peek == '\n') continue;
		fungetc(string_peek, stdin);
		scanf("%s", buffer);
		while (fgetc(stdin) != '\n');
		if (!strcmp(buffer, "S") || !strcmp(buffer, "s")) {
			;
		} else if (buffer[0] = '0') {
			fputs("remove\n", pf_gnushogi_in);
		}
		char *str_to_translate[4];
		if (buffer[1] == "*") {
			str_to_translate[0] = buffer[0] < 'a' ? buffer[0] : buffer[0] - ('a' - 'A');
		}
		fputs(buffer, pf_gnushogi_in);
	}
	return EXIT_SUCCESS;
};
