#include "texts.h"

void text_recalculate_pos(Text *text);
bool text_update_image(SDL_Renderer *renderer, Text *t,
                       SDL_ScaleMode scale_mode);

void text_recalculate_pos(Text *text) {
    if (text->anchor_h == RECT_R) {
        text->rect.x = (int)text->x - text->rect.w;
    } else if (text->anchor_h == RECT_CX) {
        text->rect.x = (int)text->x - text->rect.w / 2;
    } else {
        text->rect.x = (int)text->x;
    }

    if (text->anchor_v == RECT_B) {
        text->rect.y = (int)text->y - text->rect.h;
    } else if (text->anchor_v == RECT_CY) {
        text->rect.y = (int)text->y - text->rect.h / 2;
    } else {
        text->rect.y = (int)text->y;
    }
}

void text_set_size(Game *g, int id, float w, float h) {
    g->texts.texts[id].rect.w = (int)w;
    g->texts.texts[id].rect.h = (int)h;

    text_recalculate_pos(&g->texts.texts[id]);
}

void text_set_pos(Game *g, int id, float x, float y) {
    g->texts.texts[id].x = x;
    g->texts.texts[id].y = y;

    text_recalculate_pos(&g->texts.texts[id]);
}

void text_update_pos(Game *g, int id, float x_vel, float y_vel) {
    g->texts.texts[id].x += x_vel * g->dt;
    g->texts.texts[id].y += y_vel * g->dt;

    text_recalculate_pos(&g->texts.texts[id]);
}

void text_update_angle(Game *g, int id, float angle) {
    float new_angle = g->texts.texts[id].angle + angle * g->dt;

    if (new_angle < 0) {
        new_angle = 360.0f - fmodf(-new_angle, 360.0f);
    } else {
        new_angle = fmodf(new_angle, 360.0f);
    }

    g->texts.texts[id].angle = new_angle;
}

int text_get_rect_field(Game *g, int id, RectField field) {
    switch (field) {
    case RECT_X:
        return g->texts.texts[id].rect.x;
        break;
    case RECT_Y:
        return g->texts.texts[id].rect.y;
        break;
    case RECT_W:
        return g->texts.texts[id].rect.w;
        break;
    case RECT_H:
        return g->texts.texts[id].rect.h;
        break;
    case RECT_R:
        return g->texts.texts[id].rect.x + g->texts.texts[id].rect.w;
        break;
    case RECT_B:
        return g->texts.texts[id].rect.y + g->texts.texts[id].rect.h;
        break;
    case RECT_CX:
        return g->texts.texts[id].rect.x + g->texts.texts[id].rect.w / 2;
        break;
    case RECT_CY:
        return g->texts.texts[id].rect.y + g->texts.texts[id].rect.h / 2;
        break;
    default:
        return 0;
    }

    return 0;
}

void text_set_rect_field(Game *g, int id, RectField field, float value) {
    switch (field) {
    case RECT_X:
        g->texts.texts[id].anchor_h = RECT_X;
        g->texts.texts[id].x = value;
        break;
    case RECT_Y:
        g->texts.texts[id].anchor_v = RECT_Y;
        g->texts.texts[id].y = value;
        break;
    case RECT_W:
        g->texts.texts[id].rect.w = (int)value;
        break;
    case RECT_H:
        g->texts.texts[id].rect.h = (int)value;
        break;
    case RECT_R:
        g->texts.texts[id].anchor_h = RECT_R;
        g->texts.texts[id].x = value;
        break;
    case RECT_B:
        g->texts.texts[id].anchor_v = RECT_B;
        g->texts.texts[id].y = value;
        break;
    case RECT_CX:
        g->texts.texts[id].anchor_h = RECT_CX;
        g->texts.texts[id].x = value;
        break;
    case RECT_CY:
        g->texts.texts[id].anchor_v = RECT_CY;
        g->texts.texts[id].y = value;
        break;
    default:
        return;
    }

    text_recalculate_pos(&g->texts.texts[id]);
}

bool text_update_image(SDL_Renderer *renderer, Text *t,
                       SDL_ScaleMode scale_mode) {
    SDL_Surface *surf = TTF_RenderText_Blended(t->font, t->str, t->color);
    if (surf == NULL) {
        fprintf(stderr, "Error creating text Surface: %s\n", SDL_GetError());
        return false;
    }

    SDL_DestroyTexture(t->image);
    t->image = NULL;

    t->image = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
    if (t->image == NULL) {
        fprintf(stderr, "Error creating Texture from Surface: %s\n",
                SDL_GetError());
        return false;
    }

    SDL_SetTextureScaleMode(t->image, scale_mode);

    if (SDL_QueryTexture(t->image, NULL, NULL, &t->rect.w, &t->rect.h)) {
        fprintf(stderr, "Error getting Texture size: %s\n", SDL_GetError());
        return false;
    }

    text_recalculate_pos(t);

    return true;
}

bool text_set_color(Game *game, int id, uint8_t r, uint8_t g, uint8_t b,
                    uint8_t a) {
    game->texts.texts[id].color = (SDL_Color){r, g, b, a};

    return text_update_image(game->renderer, &game->texts.texts[id],
                             game->scale_mode);
}

bool text_set_font(Game *g, int id, const char *file, int size) {
    TTF_CloseFont(g->texts.texts[id].font);
    g->texts.texts[id].font = NULL;

    g->texts.texts[id].font = TTF_OpenFont(file, size);
    if (!g->texts.texts[id].font) {
        fprintf(stderr, "Error opening new Font: %s\n", SDL_GetError());
        return false;
    }

    return text_update_image(g->renderer, &g->texts.texts[id], g->scale_mode);
}

bool text_set_str(Game *g, int id, const char *str) {
    memcpy(g->texts.texts[id].str, str, BUFFER_SIZE);

    return text_update_image(g->renderer, &g->texts.texts[id], g->scale_mode);
}

bool text_new(Game *g, int *id, const char *file, int size, const char *str) {
    Text t = {.in_use = true};

    t.font = TTF_OpenFont(file, size);
    if (!t.font) {
        fprintf(stderr, "Error opening new Font: %s\n", SDL_GetError());
        return false;
    }

    memcpy(t.str, str, BUFFER_SIZE);

    t.color = (SDL_Color){255, 255, 255, 255};

    t.anchor_h = RECT_X;
    t.anchor_v = RECT_Y;

    if (!text_update_image(g->renderer, &t, g->scale_mode)) {
        return false;
    }

    return buffer_push(&g->texts, &t, id);
}

void text_draw(Game *g, int id) {
    SDL_RenderCopyEx(g->renderer, g->texts.texts[id].image, NULL,
                     &g->texts.texts[id].rect, g->texts.texts[id].angle, NULL,
                     g->texts.texts[id].flip);
}

void text_free(Game *g, int id) {
    if (g->texts.texts[id].font) {
        TTF_CloseFont(g->texts.texts[id].font);
        g->texts.texts[id].font = NULL;
    }
    if (g->texts.texts[id].image) {
        SDL_DestroyTexture(g->texts.texts[id].image);
        g->texts.texts[id].image = NULL;
    }

    buffer_free(&g->texts, id);
}

void texts_free(Game *g) {
    if (g && g->texts.texts) {
        for (int i = 0; i < g->texts.cap; i++) {
            if (g->texts.texts[i].font) {
                TTF_CloseFont(g->texts.texts[i].font);
                g->texts.texts[i].font = NULL;
            }
            if (g->texts.texts[i].image) {
                SDL_DestroyTexture(g->texts.texts[i].image);
                g->texts.texts[i].image = NULL;
            }
        }
        free(g->texts.texts);

        printf("Texts Free.\n");
    }
}
