#ifndef GRID_DETECTION_H
#define GRID_DETECTION_H

#include <stdlib.h>
#include "utils.h"

struct matrix {
    size_t width;
    size_t height;
    char **data;
};


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

// matrix
void destroy_matrix(struct matrix *m);

// Blk px counters arrays
struct px_count_arr *nb_blk_count(struct matrix *image,
        int start_x, int start_y,
        int end_x, int end_y);
void destroy_px_count_arr(struct px_count_arr *arr);

struct int_list **find_area_coords(struct px_count_arr *arr);

#endif
