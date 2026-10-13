#include "letter_grid.h"

static int cmp_box_cy(const void *a, const void *b) {
    const struct box *p = a;
    const struct box *q = b;
    int cp = p->y1 + p->y2;
    int cq = q->y1 + q->y2;
    return (cp > cq) - (cp < cq);
}

static int cmp_box_x(const void *a, const void *b) {
    const struct box *p = a;
    const struct box *q = b;
    return (p->x1 > q->x1) - (p->x1 < q->x1);
}

void destroy_letter_grid(struct letter_grid *g) {
    if (g) {
        destroy_box_list(g->letters);
        free(g->row_start);
        free(g);
    }
}

struct letter_grid *build_letter_grid(struct box_list *l, int char_h) {
    /*Sorts the letter boxes row by row, then left to right.
    A new row starts when the vertical center of a box differs from the
    mean center of the current row by more than half a letter height.*/
    struct letter_grid *g = calloc(1, sizeof(struct letter_grid));
    if (!g)
        errx(1, "build_letter_grid : Could not allocate memory");
    g->letters = l;
    if (l->count == 0)
        return g;

    g->row_start = malloc(l->count * sizeof(size_t)); // worst case : 1 box/row
    if (!g->row_start)
        errx(1, "build_letter_grid : Could not allocate memory");

    qsort(l->items, l->count, sizeof(struct box), cmp_box_cy);

    double row_sum = 0;
    size_t row_len = 0;
    for (size_t i = 0; i < l->count; i++) {
        double cy = (l->items[i].y1 + l->items[i].y2) / 2.0;
        if (row_len == 0 || cy - row_sum / row_len > char_h / 2.0) {
            if (row_len > 0)
                qsort(&l->items[i - row_len], row_len,
                      sizeof(struct box), cmp_box_x);
            g->row_start[g->nb_rows++] = i;
            row_sum = 0;
            row_len = 0;
        }
        row_sum += cy;
        row_len++;
    }
    qsort(&l->items[l->count - row_len], row_len,
          sizeof(struct box), cmp_box_x);
    return g;
}

void check_letter_grid(struct letter_grid *g) {
    size_t max = 0;
    for (size_t r = 0; r < g->nb_rows; r++) {
        size_t end = (r + 1 < g->nb_rows) ? g->row_start[r + 1]
                                          : g->letters->count;
        size_t len = end - g->row_start[r];
        if (len > max) max = len;
    }
    printf("grid : %zu rows, %zu letters max per row\n", g->nb_rows, max);
    for (size_t r = 0; r < g->nb_rows; r++) {
        size_t end = (r + 1 < g->nb_rows) ? g->row_start[r + 1]
                                          : g->letters->count;
        size_t len = end - g->row_start[r];
        if (len != max)
            printf("WARNING : row %zu has %zu letters (expected %zu)\n",
                   r, len, max);
    }
}
