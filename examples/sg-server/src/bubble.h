#ifndef BUBBLE_H
#define BUBBLE_H

#include "main.h"

SDL_Surface *bubble_two_surfaces(int radius, SDL_Surface *inner_surf,
                                 SDL_Surface *outer_surf);
SDL_Surface *bubble_one_surface(int radius, SDL_Surface *src_surf,
                                SDL_Color outer_color);
SDL_Texture *bubble_texture(SDL_Renderer *renderer, const char *filename,
                            int radius, SDL_Color color);

#endif
