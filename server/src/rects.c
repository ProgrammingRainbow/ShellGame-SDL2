#include "rects.h"
#include "images.h"

void rect_recalculate_pos(Rect *rect);

void rect_recalculate_pos(Rect *rect) {
    if (rect->anchor_h == RECT_R) {
        rect->rect.x = (int)rect->x - rect->rect.w;
    } else if (rect->anchor_h == RECT_CX) {
        rect->rect.x = (int)rect->x - rect->rect.w / 2;
    } else {
        rect->rect.x = (int)rect->x;
    }

    if (rect->anchor_v == RECT_B) {
        rect->rect.y = (int)rect->y - rect->rect.h;
    } else if (rect->anchor_v == RECT_CY) {
        rect->rect.y = (int)rect->y - rect->rect.h / 2;
    } else {
        rect->rect.y = (int)rect->y;
    }
}

void rect_set_size(Game *g, int id, int w, int h) {
    g->rects.rects[id].rect.w = w;
    g->rects.rects[id].rect.h = h;

    rect_recalculate_pos(&g->rects.rects[id]);
}

void rect_set_pos(Game *g, int id, float x, float y) {
    g->rects.rects[id].x = x;
    g->rects.rects[id].y = y;

    rect_recalculate_pos(&g->rects.rects[id]);
}

void rect_update_pos(Game *g, int id, float x_vel, float y_vel) {
    g->rects.rects[id].x += x_vel * g->dt;
    g->rects.rects[id].y += y_vel * g->dt;

    rect_recalculate_pos(&g->rects.rects[id]);
}

void rect_update_angle(Game *g, int id, float angle) {
    float new_angle = g->rects.rects[id].angle + angle * g->dt;

    if (new_angle < 0) {
        new_angle = 360.0f - fmodf(-new_angle, 360.0f);
    } else {
        new_angle = fmodf(new_angle, 360.0f);
    }

    g->rects.rects[id].angle = new_angle;
}

int rect_get_rect_field(Game *g, int id, RectField field) {
    switch (field) {
    case RECT_X:
        return g->rects.rects[id].rect.x;
        break;
    case RECT_Y:
        return g->rects.rects[id].rect.y;
        break;
    case RECT_W:
        return g->rects.rects[id].rect.w;
        break;
    case RECT_H:
        return g->rects.rects[id].rect.h;
        break;
    case RECT_R:
        return g->rects.rects[id].rect.x + g->rects.rects[id].rect.w;
        break;
    case RECT_B:
        return g->rects.rects[id].rect.y + g->rects.rects[id].rect.h;
        break;
    case RECT_CX:
        return g->rects.rects[id].rect.x + g->rects.rects[id].rect.w / 2;
        break;
    case RECT_CY:
        return g->rects.rects[id].rect.y + g->rects.rects[id].rect.h / 2;
        break;
    default:
        return 0;
    }

    return 0;
}

void rect_set_rect_field(Game *g, int id, RectField field, float value) {
    switch (field) {
    case RECT_X:
        g->rects.rects[id].anchor_h = RECT_X;
        g->rects.rects[id].x = value;
        break;
    case RECT_Y:
        g->rects.rects[id].anchor_v = RECT_Y;
        g->rects.rects[id].y = value;
        break;
    case RECT_W:
        g->rects.rects[id].rect.w = (int)value;
        break;
    case RECT_H:
        g->rects.rects[id].rect.h = (int)value;
        break;
    case RECT_R:
        g->rects.rects[id].anchor_h = RECT_R;
        g->rects.rects[id].x = value;
        break;
    case RECT_B:
        g->rects.rects[id].anchor_v = RECT_B;
        g->rects.rects[id].y = value;
        break;
    case RECT_CX:
        g->rects.rects[id].anchor_h = RECT_CX;
        g->rects.rects[id].x = value;
        break;
    case RECT_CY:
        g->rects.rects[id].anchor_v = RECT_CY;
        g->rects.rects[id].y = value;
        break;
    default:
        return;
    }

    rect_recalculate_pos(&g->rects.rects[id]);
}

bool rect_new(Game *g, int *id, int x, int y, int w, int h) {
    Rect r = {.in_use = true};
    r.rect.x = x;
    r.x = (float)x;
    r.rect.y = y;
    r.y = (float)y;
    r.rect.w = w;
    r.rect.h = h;
    r.anchor_h = RECT_X;
    r.anchor_v = RECT_Y;

    return buffer_push(&g->rects, &r, id);
}

bool rect_from_image(Game *g, int *id, int img_id) {
    Rect r = {.in_use = true};

    r.anchor_h = RECT_X;
    r.anchor_v = RECT_Y;

    if (SDL_QueryTexture(g->images.images[img_id].image, NULL, NULL, &r.rect.w,
                         &r.rect.h)) {
        fprintf(stderr, "Error getting Texture size: %s\n", SDL_GetError());
        return false;
    }

    return buffer_push(&g->rects, &r, id);
}

void rect_free(Game *g, int id) { buffer_free(&g->images, id); }

void rects_free(Game *g) {
    if (g && g->rects.rects) {
        free(g->rects.rects);

        printf("Rects Free.\n");
    }
}
