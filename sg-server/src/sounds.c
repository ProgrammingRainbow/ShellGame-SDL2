#include "sounds.h"

bool sound_new(Game *g, int *id, const char *filename) {
    Sound s = {.in_use = true};

    s.sound = Mix_LoadWAV(filename);
    if (s.sound == NULL) {
        fprintf(stderr, "Error loading chunk %s: %s\n", filename,
                SDL_GetError());
        return false;
    }
    return buffer_push(&g->sounds, &s, id);
}

bool sound_update(Game *g, int id, const char *filename) {
    if (g->sounds.sounds[id].sound) {
        Mix_FreeChunk(g->sounds.sounds[id].sound);
        g->sounds.sounds[id].sound = NULL;
    }

    g->sounds.sounds[id].sound = Mix_LoadWAV(filename);
    if (g->sounds.sounds[id].sound == NULL) {
        fprintf(stderr, "Error loading chunk %s: %s\n", filename,
                SDL_GetError());
        return false;
    }

    return true;
}

void sound_play(Game *g, int id, int repeat) {
    Mix_PlayChannel(-1, g->sounds.sounds[id].sound, repeat);
}

void sound_free(Game *g, int id) {
    if (g->sounds.sounds[id].sound) {
        Mix_FreeChunk(g->sounds.sounds[id].sound);
        g->sounds.sounds[id].sound = NULL;
    }

    buffer_free(&g->sounds, id);
}

void sounds_free(Game *g) {
    if (g && g->sounds.sounds) {
        for (int i = 0; i < g->sounds.cap; i++) {
            if (g->sounds.sounds[i].sound != NULL) {
                Mix_FreeChunk(g->sounds.sounds[i].sound);
                g->sounds.sounds[i].sound = NULL;
            }
        }
        free(g->sounds.sounds);

        printf("Sounds Free.\n");
    }
}
