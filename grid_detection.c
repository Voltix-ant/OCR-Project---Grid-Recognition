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

struct int_list **find_area_coords(struct px_count_arr *arr,
        float void_block_prop) {
    /*this array contain the coordinates of the top left (1) and bottom 
      right (2) corners of each area like so :  {x_1,y_1,x_2,y_2,...}
      It takes in parameter the arrays that count the numbers of black px
      per rows and columns and a float that represent the proportion of the
      width / height that a void zone must be to be consider as one*/
    struct int_list *y_points = NULL;
    struct int_list *x_points = NULL;
    int x = arr->start_x;
    int y = arr->start_y;
    int height = arr->end_y - arr->start_y + 1;
    int width = arr->end_x - arr->start_x + 1;

    int zero_ctr = 0;
    int possible_end_area = -1;
    // rows
    while (y <= arr->end_y) {
        if (arr->rows[y] == 0) {
            if (zero_ctr == 0 && y != arr->start_y) {
                // end of an area
                possible_end_area = y;
            }
            zero_ctr++;
        } else {
            if ((zero_ctr > height*void_block_prop) || y == arr->start_y) {
                // begining of a new area
                if (possible_end_area > 0)
                    y_points = int_list_push(y_points,possible_end_area);
                y_points = int_list_push(y_points,y);
            }
            zero_ctr = 0;
        }
        y++;
    }
    if (zero_ctr == 0)
        y_points = int_list_push(y_points,arr->end_y);
    else
        y_points = int_list_push(y_points,possible_end_area);
    zero_ctr = 0; //reset
    possible_end_area = -1;
    //cols
    while (x <= arr->end_x) {
        if (arr->cols[x] == 0) {
            if (zero_ctr == 0 && x != arr->start_x) {
                // end of an area
                possible_end_area = x;
            }
            zero_ctr++;
        } else {
            if ((zero_ctr > width*void_block_prop) || x == arr->start_x) {
                // begining of a new area
                if (possible_end_area > 0)
                    x_points = int_list_push(x_points,possible_end_area);
                x_points = int_list_push(x_points,x);
            }
            zero_ctr = 0;
        }
        x++;
    }
    if (zero_ctr == 0)
        x_points = int_list_push(x_points,arr->end_x);
    else
        x_points = int_list_push(x_points,possible_end_area);
    int_list_print(y_points); // Must have an even number of points
    int_list_print(x_points);

    // nb of area is (len(x_pts) / 2) * (len(pts_y) / 2)

    struct int_list **points = malloc(2*sizeof(struct int_list*));
    points[0] = y_points;
    points[1] = x_points;
    return points;
}
