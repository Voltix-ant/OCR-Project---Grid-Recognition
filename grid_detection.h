#ifndef GRID_DETECTION_H
#define GRID_DETECTION_H

#include <stdlib.h>
#include <err.h>
#include <math.h>
#include <stdio.h>

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

// ============= Regions ==================

enum region_type {
    REGION_UNKNOWN,         // Not classified yet
    REGION_GRID,            // Letters grid
    REGION_WORDLIST,        // List of words to search
    REGION_UNNECESSARY      // Titles, textes, illustrations not important
};

enum cut_direction { CUT_HORIZONTAL, CUT_VERTICAL };

struct Region {
    /* type representing a region in the image, follow a general tree structure
    , the children of a region is a region inside the parent region.*/
    int x1, y1, x2, y2;
    int level;                  // depth in XY-cut tree
    enum cut_direction cut_dir; // direction of the cut that produced this node
    enum region_type type;      // filled after identification
    struct Region **children;
    size_t nb_children;
};

struct region_stats {
    /*stats about a leaf region to classify*/
    double density;         // black pixels / total area
    double aspect_ratio;    // width / height
    double regularity_rows; // variance in the sizes of internal runs on rows
    double regularity_cols; // variance in the sizes of internal runs on cols
    int nb_row_runs;   // number of row runs
    int nb_col_runs;   // number of col runs
};

// FUNCTIONS

// matrix
void destroy_matrix(struct matrix *m);

// Blk px counters arrays
struct px_count_arr *nb_blk_count(struct matrix *image,
        int start_x, int start_y,
        int end_x, int end_y);
void destroy_px_count_arr(struct px_count_arr *arr);

// ======== Regions =======
struct Region *create_region(
    int x1, int y1, int x2, int y2, int level,
    enum cut_direction cut_dir
);
void region_add_child(struct Region *parent, struct Region *child);
void region_destroy(struct Region *root);

// ------ Macro zone dectection ------
int median_char_height(struct matrix *m);
void find_subregions(
        struct matrix *m, struct Region *p, enum cut_direction cut_dir,
        int threshold
);
void segment_region(struct matrix *m, struct Region *p,
                    enum cut_direction cut_dir, int threshold, int stalled);

void collect_leaves(struct Region *r, struct Region ***out, size_t *count);
double compute_cv(const int *counts, int n, int *nb_runs);
struct region_stats compute_region_stats(struct matrix *m, struct Region *r);

// ------ Zone classification ------
void detect_noise_leaves(struct matrix *m, 
        struct Region **l, size_t nb_leaves);
void detect_grid_leaves(struct matrix *m, struct Region **l, 
        size_t nb_leaves);
void detect_wordlist_leaves(struct Region **l, size_t nb_leaves);
void classify_leaves(struct matrix *m, struct Region **l, size_t nb_leaves);

#endif
