#include "region.h"

struct Region *create_region(
    int x1, int y1, int x2, int y2, int level,
    enum cut_direction cut_dir
) {
    struct Region *new = calloc(1,sizeof(struct Region));
    if (!new) {
        errx(1, "Create_region : Could not allocate memory");
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
            parent->children,(parent->nb_children+1)*sizeof(struct Region*)
    );
    if (!nl) {
        errx(1, "region_add_child : Could not allocate memory");
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

void collect_leaves(struct Region *r, struct Region ***out, size_t *count) {
    // this function put all the leaves from the region tree in a list
    if (r->nb_children == 0) {
        struct Region **nl = realloc(*out, 
                (*count + 1) * sizeof(struct Region*));
        if (!nl)
            errx(1, "collect_leaves : Could not allocate memory");
        *out = nl;
        (*out)[*count] = r;
        (*count)++;
        return;
    }
    for (size_t i = 0; i < r->nb_children; i++) {
        collect_leaves(r->children[i], out, count);
    }
}
