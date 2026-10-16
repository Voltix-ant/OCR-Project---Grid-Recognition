#ifndef WORD_LIST_H
#define WORD_LIST_H

#include "matrix.h"
#include "region.h"
#include "letter_grid.h"
#include "components.h"
#include <stdio.h>

struct letter_grid *build_word_list(struct matrix *m, struct Region *r);
void print_row_sizes(struct letter_grid *g);

#endif
