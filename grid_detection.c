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

    // printf("(%i,%i) -> (%i,%i)\n",start_x,start_y,end_x,end_y);
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
                res->rows[y - start_y]++;
                res->cols[x - start_x]++;
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

// ============== Region detection ==========================

static int cmp_int_asc(const void *a, const void *b) {
    int x = *(const int *)a;
    int y = *(const int *)b;
    return (x > y) - (x < y);
}

int median_char_height(struct matrix *m) {
    /*Returns the median height (in px) of the connected components of
    black px in the whole image. Since most components are letters, this
    gives a reference size that follows the scale of the image.
    Components smaller than 4 px (noise) are ignored.
    Returns 0 if no component is found.*/

    int w = (int)m->width;
    int h = (int)m->height;
    unsigned char *visited = calloc((size_t)w * h, sizeof(unsigned char));
    // each px is pushed at most once (marked visited when pushed),
    // so w*h entries are always enough
    int *stack = malloc((size_t)w * h * sizeof(int));
    if (!visited || !stack)
        errx(1, "median_char_height : Could not allocate memory");

    int *heights = NULL;
    size_t nb_heights = 0;
    size_t capacity = 0;

    for (int y0 = 0; y0 < h; y0++) {
        for (int x0 = 0; x0 < w; x0++) {
            if (m->data[y0][x0] != 0 || visited[y0 * w + x0])
                continue;

            // new component found : explore it
            int min_y = y0;
            int max_y = y0;
            size_t sp = 0;
            stack[sp++] = y0 * w + x0;
            visited[y0 * w + x0] = 1;

            while (sp > 0) {
                int idx = stack[--sp];
                int cy = idx / w;
                int cx = idx % w;
                if (cy < min_y) min_y = cy;
                if (cy > max_y) max_y = cy;

                // 8-connectivity
                for (int dy = -1; dy <= 1; dy++) {
                    for (int dx = -1; dx <= 1; dx++) {
                        int ny = cy + dy;
                        int nx = cx + dx;
                        if (ny < 0 || ny >= h || nx < 0 || nx >= w)
                            continue;
                        if (m->data[ny][nx] != 0 || visited[ny * w + nx])
                            continue;
                        visited[ny * w + nx] = 1;
                        stack[sp++] = ny * w + nx;
                    }
                }
            }

            int comp_h = max_y - min_y + 1;
            if (comp_h < 4)
                continue; // noise

            if (nb_heights == capacity) {
                capacity = capacity == 0 ? 256 : capacity * 2;
                int *nh = realloc(heights, capacity * sizeof(int));
                if (!nh)
                    errx(1, "median_char_height : Could not allocate memory");
                heights = nh;
            }
            heights[nb_heights++] = comp_h;
        }
    }

    int result = 0;
    if (nb_heights > 0) {
        qsort(heights, nb_heights, sizeof(int), cmp_int_asc);
        result = heights[nb_heights / 2];
    }

    free(heights);
    free(stack);
    free(visited);
    return result;
}

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
