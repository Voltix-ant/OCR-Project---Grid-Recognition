#ifndef IMG_LOADING_H
#define IMG_LOADING_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <err.h>
#include "grid_detection.h"

// Colors for regions
static const SDL_Color colors[6] = {
    {255, 255, 0, 255}, // 0
    {255, 200, 0, 255}, // 1
    {255, 150, 0, 255}, // 2
    {255, 100, 0, 255}, // 3
    {255, 50, 0, 255},  // 4
    {255, 0, 0, 255}    // 5
};


// loading
SDL_Surface *load_image(const char *path);
struct matrix *img_to_matrix(SDL_Surface *s);

// image edition
void draw_rect_outline(
        SDL_Surface *s, SDL_Rect r, int index, int thickness);
void draw_regions(SDL_Surface *s, struct Region *r);

// Saving
void save_result(SDL_Surface *s);

#endif
