#include "bubble.h"

void blit_symmetric_points(SDL_Surface *src_surf, SDL_Surface *target_surf,
                           int radius, int x, int y);
SDL_Surface *solid_color_surface(SDL_Surface *src_surf, SDL_Color color);

void blit_symmetric_points(SDL_Surface *src_surf, SDL_Surface *target_surf,
                           int radius, int x, int y) {
    SDL_BlitSurface(
        src_surf, NULL, target_surf,
        &(SDL_Rect){radius + x, radius + y, src_surf->w, src_surf->h});
    SDL_BlitSurface(
        src_surf, NULL, target_surf,
        &(SDL_Rect){radius + x, radius - y, src_surf->w, src_surf->h});
    SDL_BlitSurface(
        src_surf, NULL, target_surf,
        &(SDL_Rect){radius - x, radius + y, src_surf->w, src_surf->h});
    SDL_BlitSurface(
        src_surf, NULL, target_surf,
        &(SDL_Rect){radius - x, radius - y, src_surf->w, src_surf->h});
    SDL_BlitSurface(
        src_surf, NULL, target_surf,
        &(SDL_Rect){radius + y, radius + x, src_surf->w, src_surf->h});
    SDL_BlitSurface(
        src_surf, NULL, target_surf,
        &(SDL_Rect){radius + y, radius - x, src_surf->w, src_surf->h});
    SDL_BlitSurface(
        src_surf, NULL, target_surf,
        &(SDL_Rect){radius - y, radius + x, src_surf->w, src_surf->h});
    SDL_BlitSurface(
        src_surf, NULL, target_surf,
        &(SDL_Rect){radius - y, radius - x, src_surf->w, src_surf->h});
}

SDL_Surface *solid_color_surface(SDL_Surface *src_surf, SDL_Color color) {
    if (!src_surf) {
        fprintf(stderr, "Error: Surface is NULL.\n");
        return NULL;
    }

    SDL_Surface *color_surf = SDL_DuplicateSurface(src_surf);
    if (!color_surf) {
        fprintf(stderr, "Error copying Surface: %s\n", SDL_GetError());
        return NULL;
    }

    if (SDL_LockSurface(color_surf)) {
        fprintf(stderr, "Error locking Surface: %s\n", SDL_GetError());
        SDL_FreeSurface(color_surf);
        color_surf = NULL;
        return NULL;
    }

    int width = color_surf->w;
    int height = color_surf->h;

    SDL_PixelFormat *format = color_surf->format;
    if (!format) {
        fprintf(stderr, "Error getting pixel format: %s\n", SDL_GetError());
        SDL_FreeSurface(color_surf);
        color_surf = NULL;
        return NULL;
    }

    if (format->BitsPerPixel != 32) {
        fprintf(stderr, "Error: not a 32-bit surface.\n");
        SDL_FreeSurface(color_surf);
        color_surf = NULL;
        return NULL;
    }

    Uint32 *pixels = (Uint32 *)color_surf->pixels;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Uint32 pixel = pixels[width * y + x];
            Uint8 r, g, b, a;
            SDL_GetRGBA(pixel, format, &r, &g, &b, &a);
            if (a > 0) {
                pixel = SDL_MapRGBA(format, color.r, color.g, color.b, a);
                pixels[width * y + x] = pixel;
            }
        }
    }

    SDL_UnlockSurface(color_surf);

    return color_surf;
}

SDL_Surface *bubble_two_surfaces(int radius, SDL_Surface *inner_surf,
                                 SDL_Surface *outer_surf) {
    if (!inner_surf) {
        fprintf(stderr, "Error: inner Surface is NULL.\n");
        return NULL;
    }
    if (!outer_surf) {
        fprintf(stderr, "Error: outer Surface is NULL.\n");
        return NULL;
    }

    int width = inner_surf->w + radius * 2;
    int height = inner_surf->h + radius * 2;

    SDL_Surface *target_surf = SDL_CreateRGBSurfaceWithFormat(
        0, width, height, 32, SDL_PIXELFORMAT_RGBA32);
    if (!target_surf) {
        fprintf(stderr, "Error creating new Surface: %s\n", SDL_GetError());
        return NULL;
    }

    // // Polar Coordinates Trigonometry Algorithm
    // for (int index = 0; index < (2 * M_PI * radius); index++) {
    //     double rad = (double)index / radius;
    //     int x = (int)(cos(rad) * radius) + radius;
    //     int y = (int)(sin(rad) * radius) + radius;
    //
    //     SDL_BlitSurface(outer_surf, NULL, target_surf,
    //                     &(SDL_Rect){x, y, outer_surf->w, outer_surf->h});
    // }

    // Bresenham's Circle Drawing Algorithm
    // https://www.geeksforgeeks.org/bresenhams-circle-drawing-algorithm
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;
    blit_symmetric_points(outer_surf, target_surf, radius, x, y);
    while (y >= x) {
        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
        blit_symmetric_points(outer_surf, target_surf, radius, x, y);
    }

    SDL_BlitSurface(inner_surf, NULL, target_surf,
                    &(SDL_Rect){radius, radius, inner_surf->w, inner_surf->h});

    return target_surf;
}

SDL_Surface *bubble_one_surface(int radius, SDL_Surface *src_surf,
                                SDL_Color outer_color) {
    if (!src_surf) {
        fprintf(stderr, "Error: src Surface is NULL.\n");
        return NULL;
    }

    SDL_Surface *outer_surf = solid_color_surface(src_surf, outer_color);
    if (outer_surf == NULL) {
        return NULL;
    }

    SDL_Surface *bubble_surf =
        bubble_two_surfaces(radius, src_surf, outer_surf);
    SDL_FreeSurface(outer_surf);
    outer_surf = NULL;
    if (bubble_surf == NULL) {
        return NULL;
    }

    return bubble_surf;
}

SDL_Texture *bubble_texture(SDL_Renderer *renderer, const char *filename,
                            int radius, SDL_Color color) {
    SDL_Surface *surf = IMG_Load(filename);
    if (!surf) {
        fprintf(stderr, "Error loading Surface: %s\n", SDL_GetError());
        return NULL;
    }

    SDL_Surface *bubble_surf = bubble_one_surface(radius, surf, color);
    SDL_FreeSurface(surf);
    surf = NULL;
    if (!bubble_surf) {
        return NULL;
    }

    SDL_Texture *image = SDL_CreateTextureFromSurface(renderer, bubble_surf);
    SDL_FreeSurface(bubble_surf);
    bubble_surf = NULL;
    if (image == NULL) {
        fprintf(stderr, "Error creating Texture from Surface: %s\n",
                SDL_GetError());
        return NULL;
    }

    return image;
}
