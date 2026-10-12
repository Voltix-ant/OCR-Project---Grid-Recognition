#ifndef IMAGE_IO_H
#define IMAGE_IO_H

#include "matrix.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <err.h>

// Functions

SDL_Surface *load_image(const char *path);
Uint32 get_px(SDL_Surface *s, int x, int y);
struct matrix *img_to_matrix(SDL_Surface *s);

#endif
