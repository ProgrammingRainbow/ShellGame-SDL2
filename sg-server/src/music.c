#include "music.h"

bool music_new(Game *g, int *id, const char *filename) {
    Music m = {.in_use = true};

    m.music = Mix_LoadMUS(filename);
    if (m.music == NULL) {
        fprintf(stderr, "Error loading chunk %s: %s\n", filename,
                SDL_GetError());
        return false;
    }
    return buffer_push(&g->music, &m, id);
}

bool music_update(Game *g, int id, const char *filename) {
    if (g->music.music[id].music) {
        Mix_FreeMusic(g->music.music[id].music);
        g->music.music[id].music = NULL;
    }

    g->music.music[id].music = Mix_LoadMUS(filename);
    if (g->music.music[id].music == NULL) {
        fprintf(stderr, "Error loading chunk %s: %s\n", filename,
                SDL_GetError());
        return false;
    }

    return true;
}

bool music_play(Game *g, int id, int repeat) {
    if (Mix_PlayMusic(g->music.music[id].music, repeat)) {
        fprintf(stderr, "Error playing Music: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

void music_free(Game *g, int id) {
    if (g->music.music[id].music) {
        Mix_FreeMusic(g->music.music[id].music);
        g->music.music[id].music = NULL;
    }

    buffer_free(&g->music, id);
}

void music_free_all(Game *g) {
    if (g && g->music.music) {
        for (int i = 0; i < g->music.cap; i++) {
            if (g->music.music[i].music != NULL) {
                Mix_FreeMusic(g->music.music[i].music);
                g->music.music[i].music = NULL;
            }
        }
        free(g->music.music);

        printf("music Free.\n");
    }
}
