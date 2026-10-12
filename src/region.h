#ifndef REGION_H
#define REGION_H

#include <stdlib.h>
#include <err.h>

/* Regions are the macro block that contani the whole grid, the whole
   wordlist*/

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

// Functions

struct Region *create_region(
    int x1, int y1, int x2, int y2, int level,
    enum cut_direction cut_dir
);
void region_add_child(struct Region *parent, struct Region *child);
void region_destroy(struct Region *root);
void collect_leaves(struct Region *r, struct Region ***out, size_t *count);

#endif
