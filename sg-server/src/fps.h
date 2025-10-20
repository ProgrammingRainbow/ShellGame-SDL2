#ifndef FPS_H
#define FPS_H

#include "main.h"

typedef struct Fps {
        Uint32 last_time;
        float target_duration;
        float carry_duration;
        float delta_time;
        Uint32 fps_last_time;
        int fps_counter;
        int fps;
} Fps;

Fps *fps_new(void);
void fps_free(struct Fps *f);
void fps_set_target(struct Fps *f, float target);
float fps_update(struct Fps *f);

#endif
