#ifndef GRID_DETECTION_H
#define GRID_DETECTION_H

#include <stdlib.h>

struct matrix {
    size_t width;
    size_t height;
    char **data;
};

void destroy_matrix(struct matrix *m);

#endif
