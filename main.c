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
    // search region once for test purpose
    //find_subregions(mat,r_region, CUT_HORIZONTAL,0.01);
    segment_region(mat, r_region, CUT_HORIZONTAL);

    // draw areas on the image 
    draw_regions(s,r_region);

    // Save result
    save_result(s);

    // Destroy everything
    region_destroy(r_region);
    destroy_matrix(mat);

    return 0;
}
