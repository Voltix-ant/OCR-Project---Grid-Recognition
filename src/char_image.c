#include "char_image.h"

struct matrix *letter_to_28(struct matrix *img, struct box b) {
    /*This function takes a box containing a letter, resizes it (keeping its
    aspect ratio) and centers it in a 28x28 matrix for the neural network.
    Each output px is the proportion of ink found in the source area it
    represents. The caller must free the result with destroy_matrix().*/
    int box_w = b.x2 - b.x1 + 1;
    int box_h = b.y2 - b.y1 + 1;

    int longest = box_w > box_h ? box_w : box_h;
    double scale = (double)CHAR_RATIO / longest;

    int new_w = (int)round(box_w * scale);
    int new_h = (int)round(box_h * scale);
    if (new_w < 1) new_w = 1;
    if (new_h < 1) new_h = 1;

    int off_x = (28 - new_w) / 2;
    int off_y = (28 - new_h) / 2;

    // factors computed from the ROUNDED sizes, so the source box is mapped
    // exactly onto the rectangle placed in the output
    double fx = (double)new_w / box_w;
    double fy = (double)new_h / box_h;

    struct matrix *out = create_char_matrix();

    for (int y = 0; y < 28; y++) {
        for (int x = 0; x < 28; x++) {
            if (x < off_x || x >= off_x + new_w ||
                y < off_y || y >= off_y + new_h) {
                out->data[y][x] = 255;   // margin : white
                continue;
            }
            int lx = x - off_x;          // coords relative to the letter
            int ly = y - off_y;

            // source area represented by this output px (absolute coords)
            int sx_start = b.x1 + (int)floor(lx / fx);
            int sx_end   = b.x1 + (int)ceil((lx + 1) / fx) - 1;
            int sy_start = b.y1 + (int)floor(ly / fy);
            int sy_end   = b.y1 + (int)ceil((ly + 1) / fy) - 1;

            // at least one px, and never outside of the box
            if (sx_end < sx_start) sx_end = sx_start;
            if (sy_end < sy_start) sy_end = sy_start;
            if (sx_end > b.x2) sx_end = b.x2;
            if (sy_end > b.y2) sy_end = b.y2;

            int total = 0;
            int ink = 0;
            for (int sy = sy_start; sy <= sy_end; sy++) {
                for (int sx = sx_start; sx <= sx_end; sx++) {
                    total++;
                    if (img->data[sy][sx] == 0)
                        ink++;
                }
            }
            double coverage = (double)ink / total;   // 0.0 .. 1.0

            // black ink on white background, like the rest of the matrices.
            out->data[y][x] = (unsigned char)(255 - coverage * 255 + 0.5);
        }
    }
    print_matrix(out);
    return out;
}
