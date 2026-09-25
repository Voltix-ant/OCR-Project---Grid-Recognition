#include "grid_detection.h"

void destroy_matrix(struct matrix *m) {
    if (m) {
        for (size_t i = 0; i < m->height; i++) {
            free(m->data[i]);
        }
        free(m->data);
        free(m);
    }
}
