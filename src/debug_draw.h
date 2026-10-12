#ifndef DEBUG_DRAW_H
#define DEBUG_DRAW_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <err.h>
#include <sys/stat.h>
#include "region.h"

// Colors for regions
static const SDL_Color colors[6] = {
    {255, 255, 0, 255}, // 0
    {255, 200, 0, 255}, // 1
    {255, 150, 0, 255}, // 2
    {255, 100, 0, 255}, // 3
    {255, 50, 0, 255},  // 4
    {255, 0, 0, 255}    // 5
};

static const SDL_Color colors_r_type[5] = {
    {255,0,255,255},    // REGION_UNKNOWN       -> pink
    {0,0,255,255},      // REGION_GRID          -> blue
    {0,255,0,255},      // REGION_WORDLIST      -> green
    {255,0,0,255},      // REGION_UNNECESSARY   -> red
};

// Functions

// image edition
void draw_rect_outline(
        SDL_Surface *s, SDL_Rect r, Uint32 px_color, int thickness);
void draw_regions(SDL_Surface *s, struct Region *r);
void draw_macro_zones(SDL_Surface *s, struct Region **l, size_t nb_leaves);

// Saving
void save_result(SDL_Surface *s);

#endif
