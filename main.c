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

    destroy_matrix(mat); 

    return 0;
}
