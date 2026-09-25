#include "img_loading.h"
#include "grid_detection.h"
#include <stdlib.h>

SDL_Surface *load_image(const char *path) {
    SDL_Surface *tmp = IMG_Load(path);
    if (!tmp)
        errx(1, "Error : Fail loading image - %s", IMG_GetError());
    // Convert in a universal format : RGBA32
    SDL_Surface *s = SDL_ConvertSurfaceFormat(tmp, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(tmp);
    if (!s)
        errx(1, "Error : Fail to convert the image to RGBA32 - %s", 
                SDL_GetError());
    return s;
}

Uint32 get_px(SDL_Surface *s, int x, int y)
{
    return ((Uint32 *)s->pixels)[y * (s->pitch / 4) + x];
}

struct matrix *img_to_matrix(SDL_Surface *s) {
    struct matrix *mat = malloc(sizeof(struct matrix));
    mat->width = s->w;
    mat->height = s->h;
    mat->data = malloc(mat->height*sizeof(char *));
    
    SDL_LockSurface(s);
    for (int y = 0; y < s->h; y++) {
        mat->data[y] = malloc(mat->width*sizeof(char));
        for (int x = 0; x < s->w; x++) {
            Uint8 r, g, b, a;
            SDL_GetRGBA(get_px(s, x, y), s->format, &r, &g, &b, &a);

            // temporary binarisation (unusable)
            // This part will be replaced by the actual image binarisation
            // algorithm developped by Gustave
            if (r > 128) {
                mat->data[y][x] = 255;
            } else {
                mat->data[y][x] = 0;
            }
        }
    }
    SDL_UnlockSurface(s);
    return mat;
}
