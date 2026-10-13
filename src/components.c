#include "components.h"

void box_list_add(struct box_list *l, struct box b) {
    if (l->count == l->capacity) {
        size_t nc = l->capacity == 0 ? 64 : l->capacity * 2;
        struct box *ni = realloc(l->items, nc * sizeof(struct box));
        if (!ni)
            errx(1, "box_list_add : Could not allocate memory");
        l->items = ni;
        l->capacity = nc;
    }
    l->items[l->count++] = b;
}

void destroy_box_list(struct box_list *l) {
    if (l) {
        free(l->items);
        free(l);
    }
}

struct box_list *find_components(struct matrix *m,
                                 int rx1, int ry1, int rx2, int ry2) {
    /*Returns the bounding boxes of all the connected components of black
    px (8-connectivity) inside the region given. Coordinates are absolute.*/
    int w = rx2 - rx1 + 1;
    int h = ry2 - ry1 + 1;
    unsigned char *visited = calloc((size_t)w * h, sizeof(unsigned char));
    // each px is pushed at most once, so w*h entries are enough
    int *stack = malloc((size_t)w * h * sizeof(int));
    struct box_list *res = calloc(1, sizeof(struct box_list));
    if (!visited || !stack || !res)
        errx(1, "find_components : Could not allocate memory");

    for (int y0 = ry1; y0 <= ry2; y0++) {
        for (int x0 = rx1; x0 <= rx2; x0++) {
            int i0 = (y0 - ry1) * w + (x0 - rx1);
            if (m->data[y0][x0] != 0 || visited[i0])
                continue;

            struct box b = {x0, y0, x0, y0};
            size_t sp = 0;
            stack[sp++] = i0;
            visited[i0] = 1;

            while (sp > 0) {
                int idx = stack[--sp];
                int cy = ry1 + idx / w;
                int cx = rx1 + idx % w;
                if (cx < b.x1) b.x1 = cx;
                if (cx > b.x2) b.x2 = cx;
                if (cy < b.y1) b.y1 = cy;
                if (cy > b.y2) b.y2 = cy;

                for (int dy = -1; dy <= 1; dy++) {
                    for (int dx = -1; dx <= 1; dx++) {
                        int ny = cy + dy;
                        int nx = cx + dx;
                        if (ny < ry1 || ny > ry2 || nx < rx1 || nx > rx2)
                            continue;
                        int ni = (ny - ry1) * w + (nx - rx1);
                        if (m->data[ny][nx] != 0 || visited[ni])
                            continue;
                        visited[ni] = 1;
                        stack[sp++] = ni;
                    }
                }
            }
            box_list_add(res, b);
        }
    }
    free(visited);
    free(stack);
    return res;
}

static int cmp_int_asc(const void *a, const void *b) {
    int x = *(const int *)a;
    int y = *(const int *)b;
    return (x > y) - (x < y);
}

int median_char_height(struct matrix *m) {
    /*Returns the median height (in px) of the connected components of the
    whole image, ignoring noise (< 4 px). Since most components are
    letters, this gives a reference size that follows the scale of the
    image. Returns 0 if nothing is found.*/
    struct box_list *comps = find_components(m, 0, 0,
                                             (int)m->width - 1,
                                             (int)m->height - 1);
    if (comps->count == 0) {
        destroy_box_list(comps);
        return 0;
    }

    int *heights = malloc(comps->count * sizeof(int));
    if (!heights)
        errx(1, "median_char_height : Could not allocate memory");

    size_t n = 0;
    for (size_t i = 0; i < comps->count; i++) {
        int h = comps->items[i].y2 - comps->items[i].y1 + 1;
        if (h >= 4)
            heights[n++] = h;
    }

    int result = 0;
    if (n > 0) {
        qsort(heights, n, sizeof(int), cmp_int_asc);
        result = heights[n / 2];
    }
    free(heights);
    destroy_box_list(comps);
    return result;
}

void filter_letter_boxes(struct box_list *l, int char_h) {
    /*Keeps only the boxes whose size is compatible with a letter :
    removes noise (too small) and grid lines / frames (too big).*/
    const double min_h_prop = 0.5;  // min height, relative to char_h
    const double max_prop = 2.0;    // max width and height, relative to char_h

    size_t kept = 0;
    for (size_t i = 0; i < l->count; i++) {
        int bw = l->items[i].x2 - l->items[i].x1 + 1;
        int bh = l->items[i].y2 - l->items[i].y1 + 1;
        if (bh < min_h_prop * char_h)
            continue;
        if (bw > max_prop * char_h || bh > max_prop * char_h)
            continue;
        l->items[kept++] = l->items[i];
    }
    l->count = kept;
}

void merge_close_boxes(struct box_list *l, int char_h) {
    /*Merges the boxes that are very close to each other, if the merged box
    is still the size of one letter. This repairs the letters broken in
    several pieces by the binarisation. Meant for the letter grid only :
    in a tight word list it could also merge neighbour letters.*/
    const double max_gap_prop = 0.5;  // max gap between 2 pieces
    const double max_size_prop = 1.3; // max width/height of the merged box
    int max_gap = (int)(max_gap_prop * char_h);
    int max_size = (int)(max_size_prop * char_h);

    int merged = 1;
    while (merged) {
        merged = 0;
        for (size_t i = 0; i < l->count && !merged; i++) {
            for (size_t j = i + 1; j < l->count; j++) {
                struct box *a = &l->items[i];
                struct box *b = &l->items[j];

                // gap between the boxes on each axis (0 if they overlap)
                int gx = (b->x1 > a->x2) ? b->x1 - a->x2 - 1
                       : (a->x1 > b->x2) ? a->x1 - b->x2 - 1 : 0;
                int gy = (b->y1 > a->y2) ? b->y1 - a->y2 - 1
                       : (a->y1 > b->y2) ? a->y1 - b->y2 - 1 : 0;
                if (gx > max_gap || gy > max_gap)
                    continue;

                // size of the box we would get by merging them
                int nx1 = a->x1 < b->x1 ? a->x1 : b->x1;
                int ny1 = a->y1 < b->y1 ? a->y1 : b->y1;
                int nx2 = a->x2 > b->x2 ? a->x2 : b->x2;
                int ny2 = a->y2 > b->y2 ? a->y2 : b->y2;
                if (nx2 - nx1 + 1 > max_size || ny2 - ny1 + 1 > max_size)
                    continue;

                a->x1 = nx1; a->y1 = ny1; a->x2 = nx2; a->y2 = ny2;
                l->items[j] = l->items[--l->count]; // remove b
                merged = 1;
                break;
            }
        }
    }
}
