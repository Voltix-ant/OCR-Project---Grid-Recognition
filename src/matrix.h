#ifndef MATRIX_H
#define MATRIX_H

#include <stdlib.h>
#include <err.h>
#include <stdio.h>

struct matrix {
    /* The binarised image stored as a 2d matrix of width columns and height 
       rows that contain 0 for black px and 255 for white ones*/
    size_t width;
    size_t height;
    unsigned char **data;
};

// Functions 

struct matrix *create_char_matrix(void);
void print_matrix(struct matrix *m);
void destroy_matrix(struct matrix *m);

#endif
