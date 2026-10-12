#ifndef MATRIX_H
#define MATRIX_H

#include <stdlib.h>

struct matrix {
    /* The binarised image stored as a 2d matrix of width columns and height 
       rows that contain 0 for black px and 255 for white ones*/
    size_t width;
    size_t height;
    char **data;
};

// Functions 

void destroy_matrix(struct matrix *m);

#endif
