#include <err.h>
#include "img_loading.h"
#include "grid_detection.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

int main(int argc, char **argv) {
    if (argc != 2)
        errx(1, "Incorrect number of paramters");
    
    // Load the image
    SDL_Surface *s = load_image(argv[1]);
    
    // Translate to matrix
    struct matrix *mat = img_to_matrix(s);

    // gap threshold, in number of letter heights
    // to adjust with broader image selection
    const double gap_factor = 1.5;
    // provisional: works on test images from 1.0 to 1.9, middle = 1.5

    int char_h = median_char_height(mat);
    int threshold = (int)(gap_factor * char_h);
    if (threshold < 2)
        threshold = 2;               // noise floor, also covers char_h == 0
    printf("median char height = %d, threshold = %d\n", char_h, threshold);

    // root region = the whole image
    struct Region *r_region = create_region(0, 0, (int)mat->width - 1,
                                    (int)mat->height - 1, 0, CUT_HORIZONTAL);
    segment_region(mat, r_region, CUT_HORIZONTAL, threshold, 0);

    struct Region **leaves = NULL;
    size_t nb_leaves = 0;

    collect_leaves(r_region, &leaves, &nb_leaves);
    printf("%zu regions found\n",nb_leaves);

    classify_leaves(mat, leaves, nb_leaves);

    // draw areas on the image 
    // draw_regions(s,r_region);
    draw_leaves(s,leaves,nb_leaves);

    // Save result
    save_result(s);

    // Destroy everything
    free(leaves);
    region_destroy(r_region);
    destroy_matrix(mat);

    return 0;
}
