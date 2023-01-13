#include <myshogi.h>

#include <stdbool.h>

void translate_board(FILE *pf_write, FILE *pf_read)
{
	fputs("translate_board called\n", stderr);
	char *buffer[128];
	// Transfer the 1st line (blank line)
	pass_till_lf(pf_write, pf_read);
	// fputs("One line passed.", stderr);
	// The 2nd line is the time left
	int min_rmn_b = 0, secmmin_rmn_b = 0, min_rmn_w = 0, secmmin_rmn_w = 0;
	// fprintf(stderr, "The following char is \"%c\" (%d)\n", peek_char(pf_read), peek_char(pf_read));
	fscanf(pf_read, "Black %d:%d  White %d:%d", &min_rmn_b, &secmmin_rmn_b,
	       &min_rmn_w, &secmmin_rmn_w);
	fprintf(pf_write, "Time left: Black %d:%02d, White %d:%02d", min_rmn_b,
		secmmin_rmn_b, min_rmn_w, secmmin_rmn_w);
	pass_till_lf(pf_write, pf_read);
	// The 3rd line (blank line)
	pass_till_lf(pf_write, pf_read);
	// The square board starts from the 4th line to the 4 - 1 + BOARD_LENGTH line,
	// where there is always exactly one space between each alphabet representing the rows.
	// The white goes up and use upper case letters.
	for (int i = 0; i < BOARD_LENGTH; ++i) {
		for (int j = 0; j < BOARD_LENGTH; ++j) {
			const char pref = fgetc(pf_read);
			const char role = fgetc(pf_read);
			if (role == '-') {
				// fprintf(stderr, "role is blank\n");
				fputs("  ", pf_write);
				continue;
			} else {
				const bool is_promoted = pref == '+';
				const bool is_black = role >= 'a';
				const int i_role_fast =
					role - (is_black ? 'a' : 'A');
				// fprintf(stderr, "role: %c, i_role_fast: %d\n", role, i_role_fast);
				if (i_role_fast < 0) {
					fprintf(stderr,
						"i_role_fast (%d) must not be negative.\n",
						i_role_fast);
				}
				if (i_role_fast >=
				    N_SHOGI_PIECENAMES_UNPRO_FAST) {
					fprintf(stderr,
						"i_role_fast (%d) must not be larger than 40.\n",
						i_role_fast);
				}
				const char *const role_zh =
					a_shogi_piecenames_fast[is_promoted]
							       [i_role_fast];
				if (!role_zh) {
					fprintf(stderr,
						"role_zh must not be NULL.\n");
				}
				// fprintf(stderr, "role_zh is now %s.\n", role_zh);
				// Color the text of black pieces with color blue (12) and white pieces with color red (9)
				fprintf(pf_write, COLORED_TEXT("%d", "%s"),
					is_black ? 12 : 9, role_zh);
			}
		}
		pass_till_lf(pf_write, pf_read);
	}
	// Following is a blank line
	pass_till_lf(pf_write, pf_read);
	// Then the pieces taken by the players. First black and then white.
	for (int i_player = 0; i_player < 2; ++i_player) {
		// fprintf(stderr, "i_player: %d\n", i_player);
		fscanf(pf_read, i_player ? "white" : "black");
		fputs(i_player ? COLORED_TEXT("9", "white") :
				 COLORED_TEXT("12", "black"),
		      pf_write);
		char c_space = '\0';
		while ((c_space = fputc(fgetc(pf_read), pf_write)) != '\n') {
			// The number of pieces of this role (<= 8)
			const char c_num = fputc(fgetc(pf_read), pf_write);
			if (c_num < '0' || c_num > '9') {
				// fprintf(stderr, "c_num happen to be %c.\n", c_num);
				pass_till_lf(pf_write, pf_read);
				break;
			}
			// The role of pieces (upper case)
			fputs(a_shogi_piecenames_fast[0][fgetc(pf_read) - 'A'],
			      pf_write);
		}
	}
}

void show_board(FILE *pf_gnushogi_in, FILE *pf_gnushogi_out, FILE *pf_write)
{
	fprintf(stderr, "Running GNU Shogi command %s\n", "bd");
	fputs("bd\n", pf_gnushogi_in);
	translate_board(pf_write, pf_gnushogi_out);
}

#ifdef TESTEXEC_TRANSLATE_BOARD
#include <stdlib.h>
int main(int argc, char *argv[])
{
	int n_lines_to_pass = 0;
	if (argc > 1) {
		n_lines_to_pass = atoi(argv[1]);
	}
	for (int i = 0; i < n_lines_to_pass; ++i)
		pass_till_lf(stdout, stdin);
	translate_board(stdout, stdin);
	return 0;
}
#endif /* TESTEXEC_TRANSLATE_BOARD */
