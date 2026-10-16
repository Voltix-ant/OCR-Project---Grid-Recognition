#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "matrix.h"
#include <err.h>
#include <stdlib.h>
#include <math.h>

struct box {
    int x1, y1, x2, y2;          // inclusive, absolute image coordinates
};

struct box_list {
    struct box *items;
    size_t count;
    size_t capacity;
};

// Functions

void box_list_add(struct box_list *l, struct box b);
struct box_list *find_components(struct matrix *m,
                                 int rx1, int ry1, int rx2, int ry2);
void filter_letter_boxes(struct box_list *l, int char_h);
void destroy_box_list(struct box_list *l);
int median_char_height(struct matrix *m);
int median_box_height(struct box_list *l);
void merge_close_boxes(struct box_list *l, int char_h);
void split_wide_boxes(struct matrix *m, struct box_list *l, int char_h);

#endif
