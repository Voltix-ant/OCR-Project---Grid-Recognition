#include "segmentation.h"

void find_subregions(
        struct matrix *m, struct Region *p, enum cut_direction cut_dir,
        int threshold
) {
    /*This function finds all the subregions of a region p depending
    on the direction given with cut_dir, all the subregions are
    added as children of the parent region p. To do so, it uses the
    number of black px of each row/column given by nb_blk_count().
    A gap is considered a real separator if its size (in px) exceeds
    the threshold given by the caller. The caller (segment_region) is
    responsible for choosing it : it raises the threshold progressively
    until the number of subregions is small enough.
    This function is meant for macro layout segmentation only (title /
    grid / word list / etc.), not for fine text segmentation.*/

    struct px_count_arr *arr = nb_blk_count(m,p->x1,p->y1,p->x2,p->y2);
    int horizontal = (cut_dir == CUT_HORIZONTAL);
    const int *counts = horizontal ? arr->rows : arr->cols;
    int offset = horizontal ? arr->start_y : arr->start_x;
    int n = horizontal ? arr->end_y - arr->start_y + 1
                       : arr->end_x - arr->start_x + 1;

    // A row/column counts as content only if it has enough black px, so
    // that isolated noise px do not fill the gaps
    int perp = horizontal ? (arr->end_x - arr->start_x + 1)
                          : (arr->end_y - arr->start_y + 1);
    int min_count = (int)(perp * 0.005);   // 0.5 % of the perpendicular size
    if (min_count < 1) min_count = 1;

    // ------ Identify runs of black pixels --------

    int *run_start = malloc(n * sizeof(int));
    int *run_last  = malloc(n * sizeof(int));
    int nb_runs = 0;
    int start = -1;

    for (int i = 0; i < n; i++) {
        if (counts[i] >= min_count) {
            if (start == -1)
                start = i;
        } else if (start != -1) {
            run_start[nb_runs] = start;
            run_last[nb_runs]  = i - 1;
            nb_runs++;
            start = -1;
        }
    }
    if (start != -1) { // region ends on a run not closed
        run_start[nb_runs] = start;
        run_last[nb_runs]  = n - 1;
        nb_runs++;
    }

    if (nb_runs == 0) { // nothing in this region
        free(run_start);
        free(run_last);
        destroy_px_count_arr(arr);
        return;
    }

    // ------- Merge the runs based on the threshold ----------

    int cur_start = run_start[0];
    int cur_last  = run_last[0];

    for (int i = 1; i < nb_runs; i++) {
        int gap = run_start[i] - cur_last - 1;
        if (gap <= threshold) {
            cur_last = run_last[i]; // small gap : merge
            continue;
        }
        // true separator : close current region
        struct Region *c;
        if (horizontal)
            c = create_region(arr->start_x, offset + cur_start,
                              arr->end_x, offset + cur_last,
                              p->level + 1, cut_dir);
        else
            c = create_region(offset + cur_start, arr->start_y,
                              offset + cur_last, arr->end_y,
                              p->level + 1, cut_dir);
        region_add_child(p, c);
        cur_start = run_start[i];
        cur_last  = run_last[i];
    }

    // last pending block
    struct Region *c;
    if (horizontal)
        c = create_region(arr->start_x, offset + cur_start,
                          arr->end_x, offset + cur_last,
                          p->level + 1, cut_dir);
    else
        c = create_region(offset + cur_start, arr->start_y,
                          offset + cur_last, arr->end_y,
                          p->level + 1, cut_dir);
    region_add_child(p, c);

    free(run_start);
    free(run_last);
    destroy_px_count_arr(arr);
}

void segment_region(
        struct matrix *m, struct Region *p,
        enum cut_direction cut_dir, int threshold, int stalled
) {
    /*This function is recursive, it calls find_subregions() with the cut
    direction changing each call to find all the macro layout regions of
    the image. threshold is the minimal gap (in px) to separate two
    subregions, it is computed once for the whole image by the caller.
    stalled is 1 when the other direction has already failed to cut this
    region : it prevents an infinite back and forth between directions.*/

    float min_width_prop = 0.01;  // min prop of the image width for a region
    float min_height_prop = 0.01; // ----------------------height -----------
    enum cut_direction other = (cut_dir == CUT_VERTICAL)
            ? CUT_HORIZONTAL : CUT_VERTICAL;

    find_subregions(m, p, cut_dir, threshold);

    if (p->nb_children == 0)
        return; // nothing in this region

    if (p->nb_children == 1) {
        struct Region *c = p->children[0];
        if (p->x1 == c->x1 && p->x2 == c->x2 &&
            p->y1 == c->y1 && p->y2 == c->y2) {
            // nothing to cut in this direction : remove the useless child
            region_destroy(c);
            free(p->children);
            p->children = NULL;
            p->nb_children = 0;
            if (!stalled) {
                // try the other direction once before giving up
                segment_region(m, p, other, threshold, 1);
            }
            return;
        }
        // the child is smaller than p (margins removed) : keep cutting
        segment_region(m, c, other, threshold, 0);
        return;
    }

    for (size_t i = 0; i < p->nb_children; i++) {
        int width = p->children[i]->x2 - p->children[i]->x1;
        int height = p->children[i]->y2 - p->children[i]->y1;
        if (cut_dir == CUT_HORIZONTAL) {
            if (height > min_height_prop * m->height)
                segment_region(m, p->children[i], CUT_VERTICAL,
                               threshold, 0);
        } else {
            if (width > min_width_prop * m->width)
                segment_region(m, p->children[i], CUT_HORIZONTAL,
                               threshold, 0);
        }
    }
}
