#include "classification.h"

double compute_cv(const int *counts, int n, int *nb_runs) {
    int run_count = 0;
    double sum = 0, sum_sq = 0;
    int start = -1;
    for (int i = 0; i <= n; i++) {
        int black = (i < n) && counts[i] != 0;
        if (black) {
            if (start == -1) start = i;
        } else if (start != -1) {
            int len = i - start;
            sum += len;
            sum_sq += (double)len * len;
            run_count++;
            start = -1;
        }
    }
    *nb_runs = run_count;
    if (run_count <= 1)
        return 0;
    double mean = sum / run_count;
    double variance = sum_sq / run_count - mean * mean;
    return (mean > 0) ? sqrt(variance) / mean : 0;
}

struct region_stats compute_region_stats(struct matrix *m, struct Region *r) {
    struct region_stats s = {0};
    // Aspect ratio
    int width  = r->x2 - r->x1 + 1;
    int height = r->y2 - r->y1 + 1;
    s.aspect_ratio = (double)width / height;

    // Density
    struct px_count_arr *arr = nb_blk_count(m, r->x1, r->y1, r->x2, r->y2);

    long nb_black = 0;
    for (int i = 0; i < width; i++)
        nb_black += arr->cols[i];
    s.density = (double)nb_black / ((long)width * height);

    // regularity: runs along the axis perpendicular to the last cut_dir
    const int *counts = (r->cut_dir == CUT_HORIZONTAL) ? arr->cols : arr->rows;
    int n = (r->cut_dir == CUT_HORIZONTAL) ? width : height;

    int run_count = 0;
    double sum = 0, sum_sq = 0;
    int start = -1;
    for (int i = 0; i <= n; i++) {
        int black = (i < n) && counts[i] != 0;
        if (black) {
            if (start == -1) start = i;
        } else if (start != -1) {
            int len = i - start;
            sum += len;
            sum_sq += (double)len * len;
            run_count++;
            start = -1;
        }
    }
    s.regularity_rows = compute_cv(arr->rows, height, &s.nb_row_runs);
    s.regularity_cols = compute_cv(arr->cols, width,  &s.nb_col_runs);

    destroy_px_count_arr(arr);
    return s;
}

void detect_noise_leaves(struct matrix *m, struct Region **l, 
        size_t nb_leaves) {
    const double min_density = 0.01; // to adjust
    const double max_density = 0.95;

    for (size_t i = 0; i < nb_leaves; i++) {
        struct region_stats s = compute_region_stats(m, l[i]);
        if (s.density < min_density || s.density > max_density) {
            l[i]->type = REGION_UNNECESSARY;
        }
    }
}

void detect_grid_leaves(struct matrix *m, struct Region **l, 
        size_t nb_leaves) {
    // const double grid_max_regularity = ...; // to caliber
    const double grid_ratio_min = 0.5;
    const double grid_ratio_max = 2.0;
    const double grid_max_regularity = 0.3;

    for (size_t i = 0; i < nb_leaves; i++) {
        if (l[i]->type != REGION_UNKNOWN)
            continue; // Already a noise region
        struct region_stats s = compute_region_stats(m, l[i]);
        if (s.aspect_ratio > grid_ratio_min && s.aspect_ratio < grid_ratio_max
            && s.regularity_rows < grid_max_regularity
            && s.regularity_cols < grid_max_regularity) {
            l[i]->type = REGION_GRID;
        }
    }
}

void detect_wordlist_leaves(struct Region **l, size_t nb_leaves) {
    /*TEMP Function : for now we consider that every region not a grid or a 
    noise block is a wordlist, we must modify this when we use test image with
    illustration, titles or other textes*/
    for (size_t i = 0; i < nb_leaves; i++) {
        if (l[i]->type == REGION_UNKNOWN) {
            l[i]->type = REGION_WORDLIST;
        }
    }
}

void classify_leaves(struct matrix *m, struct Region **l, size_t nb_leaves) {
    detect_noise_leaves(m,l, nb_leaves);
    detect_grid_leaves(m,l, nb_leaves);
    detect_wordlist_leaves(l, nb_leaves);
}

struct Region *get_grid_region(struct Region **l, size_t nb_leaves) {
    for (size_t i = 0; i < nb_leaves; i++) {
        if (l[i]->type == REGION_GRID) {
            return l[i];
        }
    }
    return NULL;
}
