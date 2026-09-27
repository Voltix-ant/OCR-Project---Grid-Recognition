#include "utils.h"

// ================= STRING LISTS ========================

struct int_list *int_list_push(struct int_list *list, int data) {
	struct int_list *new = calloc(1,sizeof(struct int_list));
	new->data = data;
	if (list == NULL) {
		return new;
	}
	struct int_list *cur = list;
	while (cur->next != NULL) {
		cur = cur->next;
	}
	cur->next = new;
	return list;
} 

void int_list_destroy(struct int_list *l) {
	struct int_list *cur = l;
	while (cur != NULL) {
		struct int_list *temp = cur;
		cur = cur->next;
		free(temp);
	}
}

void int_list_print(struct int_list *l) {
	struct int_list *cur = l;
	while (cur != NULL) {
		printf("%i -> ",cur->data);
		cur = cur->next;
	}
	printf("NULL\n");
}
