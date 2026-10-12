#include "projection.h"

struct px_count_arr *nb_blk_count(struct matrix *mat,
        int start_x, int start_y,
        int end_x, int end_y) {
    /*This function return an array of 2 pointers : one to an array of the
    number of black px on each row and the second one to an array of the
    number of black px on each column.
    It do this only on the area of the matrix between the coordinates given*/

    struct px_count_arr *res = malloc(sizeof(struct px_count_arr));
    res->start_x = start_x;
    res->start_y = start_y;
    res->end_x = end_x;
    res->end_y = end_y;
    res->rows = calloc((end_y - start_y+1),sizeof(int));
    res->cols = calloc((end_x - start_x+1),sizeof(int));
    for (int y = start_y; y <= end_y; y++) {
        for (int x = start_x; x <= end_x; x++) {
            if (mat->data[y][x] == 0) {
                res->rows[y - start_y]++;
                res->cols[x - start_x]++;
            }
        }
    }
    return res;
}

void destroy_px_count_arr(struct px_count_arr *arr) {
    if (arr != NULL) {
        free(arr->cols);
        free(arr->rows);
        free(arr);
    }
}
