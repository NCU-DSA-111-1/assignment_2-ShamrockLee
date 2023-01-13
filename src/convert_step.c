#include <myshogi.h>

#include <stdio.h>
#include <string.h>

void convert_step_to_gnushogi_inplace(char *step)
{
	if (step[1] == '*') {
		step[0] = step[0] < 'a' ? step[0] :
					  (char)(step[0] - ('a' - 'A'));
	} else {
		step[1] += 'a' - '1';
	}
	step[3] += 'a' - '1';
}

void convert_step_to_myshogi_inplace(char *step)
{
	if (step[1] == '*') {
		step[0] = step[0] < 'a' ? step[0] :
					  (char)(step[0] - ('a' - 'A'));
	} else {
		step[1] -= 'a' - '1';
	}
	step[3] -= 'a' - '1';
}

void scan_gnushogi_step_from_reply(char *step, FILE *pf_gnushogi_out) {
    // Remove "3. " or "1. ... "
	{
		const char c_peek = fgetc(pf_gnushogi_out);
		if (c_peek == 'I') {
			ungetc(c_peek, pf_gnushogi_out);
			step = "Illegal";
			return;
		}
		ungetc(c_peek, pf_gnushogi_out);
	}
	{
		int _;
    	fscanf(pf_gnushogi_out, "%d. ", &_);
	}
	fscanf(pf_gnushogi_out, "%s", step);
	if (strcmp(step, "...")) {
		fscanf(pf_gnushogi_out, " %s", step);
	}
}
