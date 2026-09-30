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
                res->rows[y]++;
                res->cols[x]++;
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

// ===================== Regions struct ==========================

struct Region *create_region(
    int x1, int y1, int x2, int y2, int level,
    enum cut_direction cut_dir
) {
    struct Region *new = calloc(1,sizeof(struct Region));
    if (!new) {
        errx(1, "Could not allocate memory");
        return NULL;
    }
    new->x1 = x1;
    new->x2 = x2;
    new->y1 = y1;
    new->y2 = y2;
    new->level = level;
    new->cut_dir = cut_dir;
    new->type = REGION_UNKNOWN;
    new->children = NULL;
    new->nb_children = 0;
    return new;
}

void region_add_child(struct Region *parent, struct Region *child) {
    struct Region **nl = realloc(
            parent->children,(parent->nb_children+1)*sizeof(struct Region)
    );
    if (!nl) {
        errx(1, "Could not allocate memory");
        return;
    }
    parent->children = nl;
    parent->children[parent->nb_children] = child;
    parent->nb_children++;
}

void region_destroy(struct Region *root) {
    for (size_t i = 0; i < root->nb_children; i++) {
        region_destroy(root->children[i]);
    }
    free(root->children);
    free(root);
}

// ==============================================================

void find_subregions(
        struct matrix *m,
        struct Region *p, enum cut_direction cut_dir,
        float void_block_prop
) {
    /*This function find all the subregions of a region p depending
    on the direction given with cut_dir, all the subregions are
    added as children of the parent region p. To do so, it use the
    number of black px of each rows/columns given by nb_blk_count().
    We consider there is a gap (region change) if the blank area exceed
    the proportion given by void_block_prop of the width/height of the 
    parent region*/

    struct px_count_arr *arr = nb_blk_count(m,p->x1,p->y1,p->x2,p->y2);
    int x = arr->start_x;
    int y = arr->start_y;
    int height = arr->end_y - arr->start_y + 1;
    int width = arr->end_x - arr->start_x + 1;

    int zero_ctr = 0;
    int start_region = -1;
    int possible_end_region = -1;

    if (cut_dir == CUT_HORIZONTAL) {
        // rows
        while (y <= arr->end_y) {
            if (arr->rows[y] == 0) {
                if (zero_ctr == 0 && y != arr->start_y) {
                    // end of a region
                    possible_end_region = y;
                }
                zero_ctr++;
            } else {
                if ((zero_ctr > height*void_block_prop) || y == arr->start_y) {
                    // begining of a new region
                    if (possible_end_region > 0) {
                        // create the child region
                        struct Region *c = create_region(
                            arr->start_x, start_region,
                            arr->end_x, possible_end_region,
                            p->level+1,cut_dir
                        );
                        region_add_child(p,c);
                    }
                    start_region = y;
                }
                zero_ctr = 0;
            }
            y++;
        }
        if (zero_ctr == 0) {
            struct Region *c = create_region(
                arr->start_x, start_region,
                arr->end_x, arr->end_y,
                p->level+1,cut_dir
            );
            region_add_child(p,c);
        } else {
            struct Region *c = create_region(
                arr->start_x, start_region,
                arr->end_x, possible_end_region,
                p->level+1,cut_dir
            );
            region_add_child(p,c);
        }
    } else {
        //cols
        while (x <= arr->end_x) {
            if (arr->cols[x] == 0) {
                if (zero_ctr == 0 && x != arr->start_x) {
                    // end of a region
                    possible_end_region = x;
                }
                zero_ctr++;
            } else {
                if ((zero_ctr > width*void_block_prop) || x == arr->start_x) {
                    // begining of a new region
                    if (possible_end_region > 0) {
                        // create the child region
                        struct Region *c = create_region(
                            start_region, arr->start_y,
                            possible_end_region, arr->end_y,
                            p->level+1,cut_dir
                        );
                        region_add_child(p,c);
                    }
                    start_region = x;
                }
                zero_ctr = 0;
            }
            x++;
        }
        if (zero_ctr == 0) {
            struct Region *c = create_region(
                start_region, arr->start_y,
                arr->end_x, arr->end_y,
                p->level+1,cut_dir
            );
            region_add_child(p,c);
        } else {
            struct Region *c = create_region(
                start_region, arr->start_y,
                possible_end_region, arr->end_y,
                p->level+1,cut_dir
            );
            region_add_child(p,c);
        }
    }
    destroy_px_count_arr(arr);
}
