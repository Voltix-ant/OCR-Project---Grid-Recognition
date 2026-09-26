#ifndef IMG_LOADING_H
#define IMG_LOADING_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <err.h>
#include "grid_detection.h"

// loading
SDL_Surface *load_image(const char *path);
struct matrix *img_to_matrix(SDL_Surface *s);

// image edition
void draw_rect_outline(
        SDL_Surface *s, SDL_Rect r, Uint32 color, int thickness);
void draw_areas(SDL_Surface *s, struct int_list **points);

#endif
