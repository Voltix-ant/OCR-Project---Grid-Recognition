#ifndef PROJECTION_H
#define PROJECTION_H

#include "matrix.h"
#include <stdlib.h>

struct px_count_arr {
    /*Contain 2 arrays of int representing the number of black px on each
    rows/columns */
    int start_x;
    int start_y;
    int end_x;
    int end_y;
    int *rows;
    int *cols;
};

// Functions

struct px_count_arr *nb_blk_count(struct matrix *image,
        int start_x, int start_y,
        int end_x, int end_y);
void destroy_px_count_arr(struct px_count_arr *arr);

#endif
