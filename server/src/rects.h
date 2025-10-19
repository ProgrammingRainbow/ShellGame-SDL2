#ifndef RECTS_H
#define RECTS_H

#include "game.h"

typedef struct Rect {
        bool in_use;
        SDL_Rect rect;
        RectField anchor_h;
        RectField anchor_v;
        float x;
        float y;
        float angle;
        SDL_RendererFlip flip;
} Rect;

void rect_set_pos(Game *g, int id, float x, float y);
void rect_set_size(Game *g, int id, int w, int h);
void rect_update_pos(Game *g, int id, float x_vel, float y_vel);
void rect_update_angle(Game *g, int id, float angle);
int rect_get_rect_field(Game *g, int id, RectField field);
void rect_set_rect_field(Game *g, int id, RectField field, float value);
bool rect_new(Game *g, int *id, int x, int y, int w, int h);
bool rect_from_image(Game *g, int *id, int img_id);
void rect_free(Game *g, int id);
void rects_free(Game *g);

#endif
