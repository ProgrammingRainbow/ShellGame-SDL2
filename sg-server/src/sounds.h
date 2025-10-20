#ifndef SOUNDS_H
#define SOUNDS_H

#include "game.h"

typedef struct Sound {
        bool in_use;
        Mix_Chunk *sound;
} Sound;

bool sound_new(Game *g, int *id, const char *filename);
bool sound_update(Game *g, int id, const char *filename);
void sound_play(Game *g, int id, int repeat);
void sound_free(Game *g, int id);
void sounds_free(Game *g);

#endif
