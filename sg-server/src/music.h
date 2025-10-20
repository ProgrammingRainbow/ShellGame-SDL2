#ifndef MUSIC_H
#define MUSIC_H

#include "game.h"

typedef struct Music {
        bool in_use;
        Mix_Music *music;
} Music;

bool music_new(Game *g, int *id, const char *filename);
bool music_update(Game *g, int id, const char *filename);
bool music_play(Game *g, int id, int repeat);
void music_free(Game *g, int id);
void music_free_all(Game *g);

#endif
