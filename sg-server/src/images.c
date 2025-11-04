#include "images.h"
#include "rects.h"

bool image_new(Game *g, int *id, const char *filename) {
    Image i = {.in_use = true};

    i.image = IMG_LoadTexture(g->renderer, filename);
    if (i.image == NULL) {
        fprintf(stderr, "Error loading texture %s: %s\n", filename,
                SDL_GetError());
        return false;
    }

    SDL_SetTextureScaleMode(i.image, g->scale_mode);

    return buffer_push(&g->images, &i, id);
}

bool image_update_image(Game *g, int id, const char *filename) {
    if (id == -1) {
        return true;
    }

    if (g->images.images[id].image) {
        SDL_DestroyTexture(g->images.images[id].image);
        g->images.images[id].image = NULL;
    }

    g->images.images[id].image = IMG_LoadTexture(g->renderer, filename);
    if (g->images.images[id].image == NULL) {
        fprintf(stderr, "Error loading texture %s: %s\n", filename,
                SDL_GetError());
        return false;
    }

    SDL_SetTextureScaleMode(g->images.images[id].image, g->scale_mode);

    return true;
}

bool image_get_size(Game *g, int id, int *w, int *h) {
    if (id == -1) {
        return true;
    }

    if (SDL_QueryTexture(g->images.images[id].image, NULL, NULL, w, h)) {
        fprintf(stderr, "Error getting Texture size: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

void image_draw(Game *g, int img_id, int src_id, int dest_id) {
    if (img_id == -1) {
        return;
    }

    if (src_id == -1 && dest_id == -1) {
        SDL_RenderCopyEx(g->renderer, g->images.images[img_id].image, NULL,
                         NULL, 0, NULL, 0);
    } else if (src_id == -1) {
        SDL_RenderCopyEx(g->renderer, g->images.images[img_id].image, NULL,
                         &g->rects.rects[dest_id].rect,
                         g->rects.rects[dest_id].angle, NULL,
                         g->rects.rects[dest_id].flip);
    } else if (dest_id == -1) {
        SDL_RenderCopyEx(g->renderer, g->images.images[img_id].image,
                         &g->rects.rects[src_id].rect, NULL, 0, NULL, 0);
    } else {
        SDL_RenderCopyEx(
            g->renderer, g->images.images[img_id].image,
            &g->rects.rects[src_id].rect, &g->rects.rects[dest_id].rect,
            g->rects.rects[dest_id].angle, NULL, g->rects.rects[dest_id].flip);
    }
}

void image_free(Game *g, int id) {
    if (id == -1) {
        return;
    }

    if (g->images.images[id].image) {
        SDL_DestroyTexture(g->images.images[id].image);
        g->images.images[id].image = NULL;
    }

    buffer_free(&g->images, id);
}

void images_free(Game *g) {
    if (g && g->images.images) {
        for (int i = 0; i < g->images.cap; i++) {
            if (g->images.images[i].image != NULL) {
                SDL_DestroyTexture(g->images.images[i].image);
                g->images.images[i].image = NULL;
            }
        }
        free(g->images.images);

        printf("Images Free.\n");
    }
}
