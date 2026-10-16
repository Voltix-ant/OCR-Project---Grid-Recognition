#include "word_list.h"

struct letter_grid *build_word_list(struct matrix *m, struct Region *r) {
    /*Finds the letters of a word list region. Returns a letter_grid whose
    rows are the words : row i = word i, letters sorted left to right.*/
    struct box_list *comps = find_components(m, r->x1, r->y1, r->x2, r->y2);
    int local_h = median_box_height(comps);
    split_wide_boxes(m, comps, local_h);
    filter_letter_boxes(comps, local_h);
    return build_letter_grid(comps, local_h);   // takes ownership of comps
}

void print_row_sizes(struct letter_grid *g) {
    /*Prints the number of letters of each word, to compare with the image*/
    for (size_t r = 0; r < g->nb_rows; r++) {
        size_t end = (r + 1 < g->nb_rows) ? g->row_start[r + 1]
                                          : g->letters->count;
        printf("word %zu : %zu letters\n", r, end - g->row_start[r]);
    }
}
