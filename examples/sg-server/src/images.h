#ifndef IMAGES_H
#define IMAGES_H

#include "game.h"

typedef struct Image {
        bool in_use;
        SDL_Texture *image;
} Image;

bool image_new(Game *g, int *id, const char *filename);
bool image_update_image(Game *g, int id, const char *filename);
bool image_get_size(Game *g, int id, int *w, int *h);
void image_draw(Game *g, int id, int src_id, int dest_id);
void image_free(Game *g, int id);
void images_free(Game *g);

#endif
