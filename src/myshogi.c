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

#ifndef stringify_directly
#define stringify_directly(X) #X
#endif

#ifndef stringify_expanded
#define stringify_expanded(X) stringify_directly(X)
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

#define GNUSHOGI_COMMAND_STRING stringify_expanded(GNUSHOGI_COMMAND)

int start_new_game(FILE *pf_gnushogi_in, FILE *pf_gnushogi_out,
		   const char *path_game_file)
{
	char buffer[BUFFSIZE_GNUSHOGI];
	char buffer_save[BUFFSIZE_SAVE];
	struct linked_list ll_steps;
	ll_init(&ll_steps);
	size_t n_steps_taken = 0;
	while (true) {
		show_board(pf_gnushogi_in, pf_gnushogi_out, stdout);
		puts("> ");
		if (absorb_space(stdin) == '\n')
			continue;
		scanf("%s", buffer);
		absorb_till_lf(stdin);
		if (!strcmp(buffer, "S") || !strcmp(buffer, "s")) {
			fprintf(stderr, "Saving the game to %s\n",
				path_game_file);
			ll_reverse(&ll_steps);
			char *buffer_save_end = buffer_save;
			bool is_buffer_save_full = false;
			FILE *pf_save = fopen(path_game_file, "w");
			while (ll_steps.p_head) {
				struct node_pair_step *p_current_pair =
					container_of_type(struct node_ll,
							  ll_steps.p_head,
							  struct node_pair_step,
							  node);
				if (BUFFSIZE_SAVE -
					    (size_t)(buffer_save_end - buffer_save) <=
				    strlen(p_current_pair->str_step)) {
					// Write out
					fwrite(buffer_save,
					       buffer_save_end - buffer_save, 1,
					       pf_save);
					buffer_save_end = buffer_save;
				}
				strcpy(buffer_save_end,
				       p_current_pair->str_step);
				buffer_save_end +=
					strlen(p_current_pair->str_step);
			}
			if (buffer_save_end != buffer_save)
				fwrite(buffer_save,
				       buffer_save_end - buffer_save, 1,
				       pf_save);
			break;
		}
		if (buffer[0] == '0') {
			if (n_steps_taken < 2) {
				fprintf(stderr,
					"The total steps taken is %ld, which is less than 2. No round to remove.\n",
					n_steps_taken);
				continue;
			}
			fputs("remove\n", pf_gnushogi_in);
			free(container_of_type(struct node_ll,
					       ll_pop_head(&ll_steps),
					       struct node_pair_step, node));
			free(container_of_type(struct node_ll,
					       ll_pop_head(&ll_steps),
					       struct node_pair_step, node));
			n_steps_taken -= 2;
			continue;
		}
		fputs(buffer, pf_gnushogi_in);
		scan_gnushogi_step_from_reply(buffer, pf_gnushogi_out);
		if (strcmp(buffer, "Illegal")) {
			printf("Illegal step. Please try again.\n");
			continue;
		}
		++n_steps_taken;
		printf("%ld: %s\n", n_steps_taken, buffer);
		{
			struct node_pair_step *p_pair =
				(struct node_pair_step *)malloc(
					sizeof(struct node_pair_step));
			strcpy(buffer, p_pair->str_step);
			node_ll_init(&p_pair->node);
			ll_prepend(&ll_steps, &p_pair->node);
		}
	}
	return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
	enum enum_myshogi e_mode = MODE_NEW_GAME;
	const char *path_game_file = "";
	const char *gnushogi_command = GNUSHOGI_COMMAND_STRING;
	struct optparse_long longopts[] = {
		{ "help", 'h', OPTPARSE_NONE },
		{ "new", 'n', OPTPARSE_NONE },
		{ "save_as", 's', OPTPARSE_REQUIRED },
		{ "view", 'l', OPTPARSE_REQUIRED },
		{ "gnushogi-path", 'x', OPTPARSE_REQUIRED },
		{ NULL }
	};
	struct optparse options;
	(void)argc;
	optparse_init(&options, argv);
	int option;
	while ((option = optparse_long(&options, longopts, NULL)) != -1) {
		switch (option) {
		case 'h':
			puts("Description:\n"
			       "  myshogi -n -s FILENAME [-x path/to/gnushogi]\n"
			       "  myshogi -l FILENAME [-x path/to/gnushogi]\n");
			return EXIT_SUCCESS;
			break;
		case 'n':
			break;
		case 'l':
			e_mode = MODE_VIEW_GAME_FILE;
			path_game_file = options.optarg;
			break;
		case 's':
			e_mode = MODE_NEW_GAME;
			path_game_file = options.optarg;
			break;
		case 'x':
			gnushogi_command = options.optarg;
			break;
		}
	}
	if (!path_game_file[0]) {
		switch (e_mode) {
		case MODE_NEW_GAME:
			fprintf(stderr,
				"Expect -s PATH_GAME_FILE (non empty) when specifying -n\n");
			break;
		case MODE_VIEW_GAME_FILE:
			fprintf(stderr,
				"Expects -l PATH_GAME_FILE (non empty)\n");
			break;
		}
	}
	if (e_mode == MODE_NEW_GAME && !path_game_file[0]) {
		fprintf(stderr, "Expect -s PATH_GAME_FILE when specifying -n\n");
	}
	fputs("Starting GNU Shogi ...\n", stderr);
	int fd_gnushogi_in = -1, fd_gnushogi_out = -1, fd_gnushogi_err = -1;
	pid_t pid_gnushogi = popen3(gnushogi_command, &fd_gnushogi_in,
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
	{
		char c;
		while ((c = fgetc(pf_gnushogi_out)) != '\n')
			fputc(c, stderr);
		fputs(" started\n", stderr);
	}
	// Both players will be human.
	// GNU Shogi can be an AI player, but to utilize that will inclease the complexity of this code base.
	fputs("force\n", pf_gnushogi_in);
	int retval = EXIT_SUCCESS;
	switch (e_mode) {
	case MODE_NEW_GAME:
		retval = start_new_game(pf_gnushogi_in, pf_gnushogi_out,
					path_game_file);
		break;
	case MODE_VIEW_GAME_FILE:
		fprintf(stderr, "Not implemented yet.\n");
		return EXIT_FAILURE;
		break;
	}
	kill(pid_gnushogi, SIGTERM);
	// kill(pid_errprinter, SIGTERM);
	return retval;
}
