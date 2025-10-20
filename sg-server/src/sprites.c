#include "sprites.h"

void sprite_recalculate_pos(Sprite *sprite);

void sprite_recalculate_pos(Sprite *sprite) {
    if (sprite->anchor_h == RECT_R) {
        sprite->rect.x = (int)sprite->x - sprite->rect.w;
    } else if (sprite->anchor_h == RECT_CX) {
        sprite->rect.x = (int)sprite->x - sprite->rect.w / 2;
    } else {
        sprite->rect.x = (int)sprite->x;
    }

    if (sprite->anchor_v == RECT_B) {
        sprite->rect.y = (int)sprite->y - sprite->rect.h;
    } else if (sprite->anchor_v == RECT_CY) {
        sprite->rect.y = (int)sprite->y - sprite->rect.h / 2;
    } else {
        sprite->rect.y = (int)sprite->y;
    }
}

void sprite_set_size(Game *g, int id, int w, int h) {
    g->sprites.sprites[id].rect.w = w;
    g->sprites.sprites[id].rect.h = h;

    sprite_recalculate_pos(&g->sprites.sprites[id]);
}

void sprite_set_pos(Game *g, int id, float x, float y) {
    g->sprites.sprites[id].x = x;
    g->sprites.sprites[id].y = y;

    sprite_recalculate_pos(&g->sprites.sprites[id]);
}

void sprite_update_pos(Game *g, int id, float x_vel, float y_vel) {
    g->sprites.sprites[id].x += x_vel * g->dt;
    g->sprites.sprites[id].y += y_vel * g->dt;

    sprite_recalculate_pos(&g->sprites.sprites[id]);
}

void sprite_update_angle(Game *g, int id, float angle) {
    float new_angle = g->sprites.sprites[id].angle + angle * g->dt;

    if (new_angle < 0) {
        new_angle = 360.0f - fmodf(-new_angle, 360.0f);
    } else {
        new_angle = fmodf(new_angle, 360.0f);
    }

    g->sprites.sprites[id].angle = new_angle;
}

int sprite_get_rect_field(Game *g, int id, RectField field) {
    switch (field) {
    case RECT_X:
        return g->sprites.sprites[id].rect.x;
        break;
    case RECT_Y:
        return g->sprites.sprites[id].rect.y;
        break;
    case RECT_W:
        return g->sprites.sprites[id].rect.w;
        break;
    case RECT_H:
        return g->sprites.sprites[id].rect.h;
        break;
    case RECT_R:
        return g->sprites.sprites[id].rect.x + g->sprites.sprites[id].rect.w;
        break;
    case RECT_B:
        return g->sprites.sprites[id].rect.y + g->sprites.sprites[id].rect.h;
        break;
    case RECT_CX:
        return g->sprites.sprites[id].rect.x +
               g->sprites.sprites[id].rect.w / 2;
        break;
    case RECT_CY:
        return g->sprites.sprites[id].rect.y +
               g->sprites.sprites[id].rect.h / 2;
        break;
    default:
        return 0;
        break;
    }

    return 0;
}

void sprite_set_rect_field(Game *g, int id, RectField field, float value) {
    switch (field) {
    case RECT_X:
        g->sprites.sprites[id].anchor_h = RECT_X;
        g->sprites.sprites[id].x = value;
        break;
    case RECT_Y:
        g->sprites.sprites[id].anchor_v = RECT_Y;
        g->sprites.sprites[id].y = value;
        break;
    case RECT_W:
        g->sprites.sprites[id].rect.w = (int)value;
        break;
    case RECT_H:
        g->sprites.sprites[id].rect.h = (int)value;
        break;
    case RECT_R:
        g->sprites.sprites[id].anchor_h = RECT_R;
        g->sprites.sprites[id].x = value;
        break;
    case RECT_B:
        g->sprites.sprites[id].anchor_v = RECT_B;
        g->sprites.sprites[id].y = value;
        break;
    case RECT_CX:
        g->sprites.sprites[id].anchor_h = RECT_CX;
        g->sprites.sprites[id].x = value;
        break;
    case RECT_CY:
        g->sprites.sprites[id].anchor_v = RECT_CY;
        g->sprites.sprites[id].y = value;
        break;
    default:
        return;
        break;
    }

    sprite_recalculate_pos(&g->sprites.sprites[id]);
}

bool sprite_new(Game *g, int *id, const char *filename) {
    Sprite s = {.in_use = true};

    s.image = IMG_LoadTexture(g->renderer, filename);
    if (s.image == NULL) {
        fprintf(stderr, "Error loading texture %s: %s\n", filename,
                SDL_GetError());
        return false;
    }

    SDL_SetTextureScaleMode(s.image, g->scale_mode);

    if (SDL_QueryTexture(s.image, NULL, NULL, &s.rect.w, &s.rect.h)) {
        fprintf(stderr, "Error getting Texture size: %s\n", SDL_GetError());
        return false;
    }

    s.anchor_h = RECT_X;
    s.anchor_v = RECT_Y;

    return buffer_push(&g->sprites, &s, id);
}

bool sprite_update_image(Game *g, int id, const char *filename) {
    if (g->sprites.sprites[id].image) {
        SDL_DestroyTexture(g->sprites.sprites[id].image);
        g->sprites.sprites[id].image = NULL;
    }

    g->sprites.sprites[id].image = IMG_LoadTexture(g->renderer, filename);
    if (g->sprites.sprites[id].image == NULL) {
        fprintf(stderr, "Error loading texture %s: %s\n", filename,
                SDL_GetError());
        return false;
    }

    SDL_SetTextureScaleMode(g->sprites.sprites[id].image, g->scale_mode);

    if (SDL_QueryTexture(g->sprites.sprites[id].image, NULL, NULL,
                         &g->sprites.sprites[id].rect.w,
                         &g->sprites.sprites[id].rect.h)) {
        fprintf(stderr, "Error getting Texture size: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

void sprite_draw(Game *g, int id) {
    SDL_RenderCopyEx(g->renderer, g->sprites.sprites[id].image, NULL,
                     &g->sprites.sprites[id].rect, g->sprites.sprites[id].angle,
                     NULL, g->sprites.sprites[id].flip);
}

void sprite_free(Game *g, int id) {
    if (g->sprites.sprites[id].image) {
        SDL_DestroyTexture(g->sprites.sprites[id].image);
        g->sprites.sprites[id].image = NULL;
    }

    buffer_free(&g->sprites, id);
}

void sprites_free(Game *g) {
    if (g && g->sprites.sprites) {
        for (int i = 0; i < g->sprites.cap; i++) {
            if (g->sprites.sprites[i].image) {
                SDL_DestroyTexture(g->sprites.sprites[i].image);
                g->sprites.sprites[i].image = NULL;
            }
        }
        free(g->sprites.sprites);

        printf("Sprites Free.\n");
    }
}
