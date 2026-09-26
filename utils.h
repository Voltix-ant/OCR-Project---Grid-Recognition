#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdio.h>

struct int_list {
	int data; // malloc
	struct int_list *next;
};

// Fonctions :
struct int_list *int_list_push(struct int_list *list, int data);
void int_list_destroy(struct int_list *l);
void int_list_print(struct int_list *l);


#endif
