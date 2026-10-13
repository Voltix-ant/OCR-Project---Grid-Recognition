#ifndef CLASSIFICATION_H
#define CLASSIFICATION_H

#include "matrix.h"
#include "region.h"
#include "projection.h"
#include <stdlib.h>
#include <math.h>

struct region_stats {
    /*stats about a leaf region to classify*/
    double density;         // black pixels / total area
    double aspect_ratio;    // width / height
    double regularity_rows; // variance in the sizes of internal runs on rows
    double regularity_cols; // variance in the sizes of internal runs on cols
    int nb_row_runs;   // number of row runs
    int nb_col_runs;   // number of col runs
};

// Functions

double compute_cv(const int *counts, int n, int *nb_runs);
struct region_stats compute_region_stats(struct matrix *m, struct Region *r);

void detect_noise_leaves(struct matrix *m, 
        struct Region **l, size_t nb_leaves);
void detect_grid_leaves(struct matrix *m, struct Region **l, 
        size_t nb_leaves);
void detect_wordlist_leaves(struct Region **l, size_t nb_leaves);
void classify_leaves(struct matrix *m, struct Region **l, size_t nb_leaves);
struct Region *get_grid_region(struct Region **l, size_t nb_leaves);

#endif
