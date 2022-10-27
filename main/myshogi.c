#include <errno.h>
#include <error.h>
#include <stdio.h>
#include <stdlib.h>
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

#define GNUSHOGI_COMMAND_STRING STRINGIFY(GNUSHOGI_COMMAND)

void errprinter(const int fd_target_err, const pid_t pid_target) {
	FILE *pf_target_err = fdopen(fd_target_err, "r");
	char buffer_errprinter[BUFFSIZE_ERRPRINTER];
	while (kill(pid_target, 0), errno != ESRCH) {
		fscanf(pf_target_err, "%s", buffer_errprinter);
		fprintf(stderr, "gnushogi stderr: %s", buffer_errprinter);
		while (fputc(fgetc(pf_target_err), stderr) != '\n');
	}
	dup2(pf_target_err, STDERR_FILENO);
	fclose(pf_target_err);
	close(fd_target_err);
}

void translate_board(FILE *pf_write, FILE *pf_read) {
	char *buffer[128];
	// Transfer the first line
	while (fputc(fgetc(pf_read), pf_write) != '\n');
	int min_rmn_b = 0, secmmin_rmn_b = 0, min_rmn_w = 0, secmmin_rmn_w = 0;
	fscanf(pf_read, "Black %i:%i  White %i:%i", &min_rmn_b, &secmmin_rmn_b, &min_rmn_w, &secmmin_rmn_w);
	fprintf(pf_write, "Time remaining: Black %i:%i, White %i:%i");
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
		}
	}
	int fd_gnushogi_in = -1, fd_gnushogi_out = -1, fd_gnushogi_err = -1;
	pid_t pid_gnushogi = popen3(GNUSHOGI_COMMAND_STRING, &fd_gnushogi_in, &fd_gnushogi_out, &fd_gnushogi_err);
	pid_t pid_errprinter = fork();
	if (pid_errprinter == -1) {
		perror("errprinter: failed to start.\n");
		return EXIT_FAILURE;
	} if (!pid_errprinter) {
		errprinter(fd_gnushogi_err, pid_gnushogi);
		return EXIT_SUCCESS;
	}
	return EXIT_SUCCESS;
};
