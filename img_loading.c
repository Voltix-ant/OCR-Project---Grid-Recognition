#include "img_loading.h"
#include "grid_detection.h"
#include <stdlib.h>
#include <sys/stat.h>

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


void draw_rect_outline(
        SDL_Surface *s, SDL_Rect r, int index, int thickness
) {
    /*This function draw a rectangle on a surface (used to show) areas for 
      debug purpose*/
    SDL_Color c = colors[index];
    Uint32 px_color = SDL_MapRGB(s->format, c.r, c.g, c.b);

    SDL_Rect top    = { r.x, r.y, r.w, thickness };
    SDL_Rect bottom = { r.x, r.y + r.h - thickness, r.w, thickness };
    SDL_Rect left   = { r.x, r.y, thickness, r.h };
    SDL_Rect right  = { r.x + r.w - thickness, r.y, thickness, r.h };

    SDL_FillRect(s, &top, px_color);
    SDL_FillRect(s, &bottom, px_color);
    SDL_FillRect(s, &left, px_color);
    SDL_FillRect(s, &right, px_color);
}

void draw_regions(SDL_Surface *s, struct Region *r) {
    /*This function does a DFS in the region tree of root r to draw all the 
    regions*/
    if (r != NULL) {
        // Draw the root area
        SDL_Rect zone = {.x = r->x1,
                         .y = r->y1,
                         .w = r->x2 - r->x1 + 1,
                         .h = r->y2 - r->y1 + 1};
        draw_rect_outline(s, zone, r->level % 6, 2);
    }
    for (size_t i = 0; i < r->nb_children; i++) {
        draw_regions(s,r->children[i]);
    }
}

void save_result(SDL_Surface *s) {
    mkdir("detec_steps", 0755);
    IMG_SavePNG(s, "detec_steps/result.png");
}
