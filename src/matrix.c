#include "matrix.h"

struct matrix *create_char_matrix(void) {
    struct matrix *cm = calloc(1,sizeof(struct matrix));
    if (!cm)
        errx(1, "create_char_matrix : Couldn't allocate memory");
    cm->width = 28;
    cm->height = 28;
    cm->data = calloc(28,sizeof(char *));
    for (size_t y = 0; y < 28; y++) {
        cm->data[y] = calloc(28, sizeof(unsigned char));
        if (!cm->data[y])
            errx(1, "create_char_matrix : Couldn't allocate memory");
    }
    return cm;
}

void print_matrix(struct matrix *m) {
    for (size_t a = 0; a < 2*m->width +2; a++) printf("_");
    printf("\n");
    for (size_t y = 0; y < m->height; y++) {
        printf("|");
        for (size_t x = 0; x < m->width; x++) {
            if (m->data[y][x] == 0) {
                printf("  ");
            } else {
                printf("##");
            }
        }
        printf("|\n");
    }
    for (size_t a = 0; a < 2*m->width +2; a++) printf("_");
    printf("\n");
}

void destroy_matrix(struct matrix *m) {
    if (m) {
        for (size_t i = 0; i < m->height; i++) {
            free(m->data[i]);
        }
        free(m->data);
        free(m);
    }
}
