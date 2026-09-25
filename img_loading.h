#ifndef IMG_LOADING_H
#define IMG_LOADING_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <err.h>
#include "grid_detection.h"

SDL_Surface *load_image(const char *path);
struct matrix *img_to_matrix(SDL_Surface *s);

#endif
