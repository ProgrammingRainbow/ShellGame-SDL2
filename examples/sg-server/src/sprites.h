#ifndef SPRITE_H
#define SPRITE_H

#include "game.h"

typedef struct Sprite {
        bool in_use;
        SDL_Texture *image;
        SDL_Rect rect;
        RectField anchor_h;
        RectField anchor_v;
        float x;
        float y;
        float angle;
        SDL_RendererFlip flip;
} Sprite;

void sprite_set_pos(Game *g, int id, float x, float y);
void sprite_set_size(Game *g, int id, int w, int h);
void sprite_update_pos(Game *g, int id, float x_vel, float y_vel);
void sprite_update_angle(Game *g, int id, float angle);
int sprite_get_rect_field(Game *g, int id, RectField field);
void sprite_set_rect_field(Game *g, int id, RectField field, float value);
bool sprite_new(Game *g, int *id, const char *filename);
bool sprite_update_image(Game *g, int id, const char *filename);
void sprite_draw(Game *g, int id);
void sprite_free(Game *g, int id);
void sprites_free(Game *g);

#endif
