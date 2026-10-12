#include "debug_draw.h"

void draw_rect_outline(
        SDL_Surface *s, SDL_Rect r, Uint32 px_color, int thickness
) {
    /*This function draw a rectangle on a surface (used to show) areas for 
      debug purpose*/

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
        SDL_Color c = colors[r->level % 6];
        Uint32 px_color = SDL_MapRGB(s->format, c.r, c.g, c.b);
        draw_rect_outline(s, zone, px_color, 2);
    }
    for (size_t i = 0; i < r->nb_children; i++) {
        draw_regions(s,r->children[i]);
    }
}

void draw_macro_zones(SDL_Surface *s, struct Region **l, size_t nb_leaves) {
    for (size_t i = 0; i < nb_leaves; i++) {
        struct Region *r = l[i];
        SDL_Rect zone = {.x = r->x1,
                         .y = r->y1,
                         .w = r->x2 - r->x1 + 1,
                         .h = r->y2 - r->y1 + 1};
        SDL_Color c = colors_r_type[r->type % 5];
        Uint32 px_color = SDL_MapRGB(s->format, c.r, c.g, c.b);
        draw_rect_outline(s, zone, px_color, 2);
    }
}

void save_result(SDL_Surface *s) {
    mkdir("detec_steps", 0755);
    IMG_SavePNG(s, "detec_steps/macro_zones.png");
}

