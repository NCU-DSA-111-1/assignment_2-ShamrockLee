#include <myshogi.h>

#include <stddef.h>

/**
 * Linux-kernel-style linked container implementation.
 *
 * The "node content" isn't packaed inside the node struct,
 * but would be declared externally instead for each use case.
 *
 * ```c
 * struct node_pair {
 *   type_a value_a;
 *   type_b value_b;
 *   struct type_node node;
 * };
 * node_pair *p_pair;
 * ```
 *
 * This way, we can reuse `struct type_node` and related functions
 * for different types of contents.
 *
 * To extract p_pair from p_node, use the `offsetof` macro from `stddef.h`
 *
 * ```c
 * p_pair = (struct node_pair *)(p_node - offsetof(struct node_pair, node))
 * ```
 *
 * Reference:
 * *   https://github.com/fbuihuu/libtree
 * *   https://zhuanlan.zhihu.com/p/425254012
 **/

void node_ll_init(struct node_ll *p_node)
{
	p_node->p_next = NULL;
}

void ll_init(struct linked_list *p_linked_list)
{
	p_linked_list->p_head = NULL;
	return 0;
}

void ll_insert_after(struct node_ll *p_node_reference,
		     struct node_ll *p_node_to_insert)
{
	p_node_to_insert->p_next = p_node_reference->p_next;
	p_node_reference->p_next = p_node_to_insert;
}

void ll_prepend(struct linked_list *p_linked_list,
		struct node_ll *p_node_to_insert)
{
	p_node_to_insert->p_next = p_linked_list->p_head;
	p_linked_list->p_head = p_node_to_insert;
}

struct node_ll *ll_pop_after(struct node_ll *p_node_reference)
{
	struct node_ll *p_node_to_pop = p_node_reference->p_next;
	if (p_node_to_pop)
		p_node_reference->p_next = p_node_to_pop->p_next;
	return p_node_to_pop;
}

struct node_ll *ll_pop_head(struct linked_list *p_linked_list)
{
	struct node_ll *p_node_to_pop = p_linked_list->p_head;
	if (p_node_to_pop)
		p_linked_list->p_head = p_node_to_pop->p_next;
	return p_node_to_pop;
}

struct link_list *ll_reverse(struct linked_list *p_linked_list)
{
	struct node_ll *p_node_prev = NULL;
	struct node_ll *p_node_now = p_linked_list->p_head;
	while (p_node_now) {
		struct node_ll *p_node_next = p_node_now->p_next;
		p_node_now->p_next = p_node_prev;
		p_node_prev = p_node_now;
		p_node_now = p_node_next;
	}
}
