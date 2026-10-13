#ifndef LETTER_GRID_H
#define LETTER_GRID_H

#include "components.h"
#include <stdio.h>
#include <stdlib.h>
#include <err.h>

struct letter_grid {
    struct box_list *letters;    // sorted row by row, then left to right
    size_t *row_start;           // index in letters of the first box of each row
    size_t nb_rows;
};

// Functions

struct letter_grid *build_letter_grid(struct box_list *l, int char_h);
void check_letter_grid(struct letter_grid *g);
void destroy_letter_grid(struct letter_grid *g);

#endif
