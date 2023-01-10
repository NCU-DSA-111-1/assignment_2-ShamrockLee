#include <myshogi.h>

#include <errno.h>
#include <stdbool.h>
#include <signal.h>

void errprinter(const int fd_target_err, const pid_t pid_target,
		const char *const name)
{
	FILE *pf_target_err = fdopen(fd_target_err, "r");
	char buffer_errprinter[BUFFSIZE_ERRPRINTER];
	while (pid_target < 0 || (kill(pid_target, 0), errno != ESRCH)) {
		fscanf(pf_target_err, "%s", buffer_errprinter);
		fprintf(stderr, "%s stderr: %s", name, buffer_errprinter);
		while (fputc(fgetc(pf_target_err), stderr) != '\n')
			;
	}
	dup2(fd_target_err, STDERR_FILENO);
	fclose(pf_target_err);
}

#ifdef TESTEXEC_ERRPRINTER
int main(int argc, char *argv[])
{
	const pid_t pid_target = argc > 1 ? atoi(argv[1]) : -1;
	errprinter(STDIN_FILENO, pid_target, "test");
}
#endif /* TESTEXEC_ERRPRINTER */
