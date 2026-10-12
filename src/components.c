#include "components.h"

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
