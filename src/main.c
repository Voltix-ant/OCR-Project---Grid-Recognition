#include <stdlib.h>
#include <stdio.h>
#include "image_io.h"
#include "matrix.h"
#include "region.h"
#include "components.h"
#include "char_image.h"
#include "segmentation.h"
#include "classification.h"
#include "debug_draw.h"

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
    struct Region *grid_region = get_grid_region(leaves, nb_leaves);
    if (!grid_region)
        errx(1, "No grid region found");

    // draw areas on the image 
    SDL_Surface *s_macro_zones = SDL_DuplicateSurface(s);
    if (!s_macro_zones)
        errx(1, "%s", SDL_GetError());
    draw_macro_zones(s_macro_zones,leaves,nb_leaves);

    // Save result
    save_result(s_macro_zones, "macro_zones.png");
    SDL_FreeSurface(s_macro_zones);

    // letter grid
    struct box_list *comps = find_components(mat, grid_region->x1, 
            grid_region->y1,grid_region->x2, grid_region->y2);
    filter_letter_boxes(comps, char_h);
    merge_close_boxes(comps, char_h);
    //destroy_matrix(letter_to_28(mat,comps->items[5]));
    struct letter_grid *lg = build_letter_grid(comps, char_h); // lg owns comps
    check_letter_grid(lg);

    SDL_Surface *s_letter_grid = SDL_DuplicateSurface(s);
    if (!s_letter_grid)
        errx(1, "%s", SDL_GetError());
    draw_letter_grid(s_letter_grid, lg);
    save_result(s_letter_grid,"letter_grid.png");
    SDL_FreeSurface(s_letter_grid);

    // Destroy everything
    destroy_letter_grid(lg);
    free(leaves);
    region_destroy(r_region);
    destroy_matrix(mat);

    return 0;
}
