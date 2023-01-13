#ifndef __MYSHOGI_H__
#define __MYSHOGI_H__

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include "shogi_piecename2kanji_table.h"

#ifndef BUFFSIZE_ERRPRINTER
#define BUFFSIZE_ERRPRINTER 1024ul
#endif

#define COLORED_TEXT(STR_COLOR, STR_TEXT) "\033[38;5;" STR_COLOR "m" STR_TEXT "\033[0m"

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

char absorb_char(FILE *pf_read, const char c)
{
	char c_peek = '\0';
	while ((c_peek = fgetc(pf_read)) == c && c_peek != '\n')
		;
	ungetc(c_peek, pf_read);
	return c_peek;
}

char absorb_space(FILE *pf_read)
{
	char c_peek = '\0';
	while (isspace(c_peek = fgetc(pf_read)) && c_peek != '\n')
		;
	ungetc(c_peek, pf_read);
	return c_peek;
}

char peek_char(FILE *pf_read)
{
	const char c_peek = fgetc(pf_read);
	ungetc(c_peek, pf_read);
	return c_peek;
}

void scan_gnushogi_step_from_reply(char *step, FILE *pf_gnushogi_out);

enum enum_myshogi {
	MODE_NEW_GAME,
	MODE_VIEW_GAME_FILE,
};

// Get union ptr
#ifndef container_of_type
#define container_of_type(type_node, p_node, type_pair, node_member_in_pair) \
	(type_pair *)((char *)((type_node *)(p_node)) -                      \
		      offsetof(type_pair, node_member_in_pair))
#endif /* container_of_type */

struct node_ll {
	struct node_ll *p_next;
};

struct linked_list {
	struct node_ll *p_head;
};

struct node_pair_step {
	const char *str_step;
	struct node_ll node;
};

void node_ll_init(struct node_ll *p_node);

void ll_init(struct linked_list *p_linked_list);

void ll_insert_after(struct node_ll *p_node_reference,
		     struct node_ll *p_node_to_insert);

void ll_prepend(struct linked_list *p_linked_list,
		struct node_ll *p_node_to_insert);

struct node_ll *ll_pop_after(struct node_ll *p_node_reference);

struct node_ll *ll_pop_head(struct linked_list *p_linked_list);

struct link_list *ll_reverse(struct linked_list *p_linked_list);

void errprinter(const int fd_target_err, const pid_t pid_target,
		const char *const name);

#ifndef BOARD_LENGTH
#define BOARD_LENGTH 9
#endif

void translate_board(FILE *pf_write, FILE *pf_read);

void show_board(FILE *pf_gnushogi_in, FILE *pf_gnushogi_out, FILE *pf_write);

#endif /* __MYSHOGI_H__ */
