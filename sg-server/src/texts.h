#ifndef TEXTS_H
#define TEXTS_H

#include "game.h"

typedef struct Text {
        bool in_use;
        char str[BUFFER_SIZE];
        TTF_Font *font;
        SDL_Texture *image;
        SDL_Rect rect;
        SDL_Color color;
        RectField anchor_h;
        RectField anchor_v;
        float x;
        float y;
        float angle;
        SDL_RendererFlip flip;
} Text;

void text_set_pos(Game *g, int id, float x, float y);
void text_set_size(Game *g, int id, float w, float h);
void text_update_pos(Game *g, int id, float x_vel, float y_vel);
void text_update_angle(Game *g, int id, float angle);
int text_get_rect_field(Game *g, int id, RectField field);
void text_set_rect_field(Game *g, int id, RectField field, float value);
bool text_set_color(Game *game, int id, uint8_t r, uint8_t g, uint8_t b,
                    uint8_t a);
bool text_set_font(Game *g, int id, const char *file, int size);
bool text_set_str(Game *g, int id, const char *str);
bool text_new(Game *g, int *id, const char *file, int size, const char *str);
void text_draw(Game *g, int id);
void text_free(Game *g, int id);
void texts_free(Game *g);

#endif
