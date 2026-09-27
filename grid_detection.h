#ifndef GRID_DETECTION_H
#define GRID_DETECTION_H

#include <stdlib.h>
#include "utils.h"
#include <err.h>

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
    REGION_UNKNOWN,
    REGION_GRID,
    REGION_WORDLIST,
    REGION_UNNECESSARY
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



struct int_list **find_area_coords(struct px_count_arr *arr,
        float void_block_prop);

#endif
