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

    // count black px on the whole image
    struct px_count_arr *total_blk_counts = nb_blk_count(
            mat, 0,0,mat->width-1,mat->height-1
    );

    // find the areas
    struct int_list **area_coords_arr = find_area_coords(
            total_blk_counts, 0.01
    );
   // draw areas on the image 
    draw_areas(s,area_coords_arr);

    destroy_px_count_arr(total_blk_counts);
    int_list_destroy(area_coords_arr[0]);
    int_list_destroy(area_coords_arr[1]);
    free(area_coords_arr);
    destroy_matrix(mat);

    return 0;
}
