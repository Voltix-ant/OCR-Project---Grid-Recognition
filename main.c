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

    // root region = the whole image
    struct Region *r_region = create_region(0,0,mat->width-1,mat->height-1,
                                            0, CUT_VERTICAL);
    segment_region(mat, r_region, CUT_HORIZONTAL);
    
    struct Region **leaves = NULL;
    size_t nb_leaves = 0;

    collect_leaves(r_region, &leaves, &nb_leaves);
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
