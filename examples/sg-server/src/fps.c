#include "fps.h"

Uint32 fps_time_since(Uint32 last_time, Uint32 *new_last_time);

Fps *fps_new(void) {
    Fps *f = calloc(1, sizeof(struct Fps));
    if (f == NULL) {
        fprintf(stderr, "Error in Calloc of New Fps!\n");
        return NULL;
    }

    f->last_time = SDL_GetTicks();
    f->target_duration = 1000.0f / FPS_TARGET;
    f->carry_duration = 0;
    f->fps_last_time = f->last_time;
    f->fps_counter = 0;

    return f;
}

void fps_free(struct Fps *f) {
    if (f) {
        free(f);
    }

    printf("FPS Free.\n");
}

void fps_set_target(struct Fps *f, float target) {
    if (target > 0) {
        f->target_duration = 1000.0f / target;
    }
}

Uint32 fps_time_since(Uint32 last_time, Uint32 *new_last_time) {
    Uint32 current_time = SDL_GetTicks();

    Uint32 elapsed_time = (current_time >= last_time)
                              ? current_time - last_time
                              : (Uint32)-1 - last_time + current_time;

    if (new_last_time) {
        *new_last_time = current_time;
    }

    return elapsed_time;
}

float fps_update(struct Fps *f) {
    Uint32 elapsed_time = fps_time_since(f->last_time, NULL);
    float delay = f->target_duration + f->carry_duration;

    if (delay > (float)elapsed_time) {
        Uint32 current_delay = (Uint32)delay - elapsed_time;
        SDL_Delay(current_delay);
    }

    elapsed_time = fps_time_since(f->last_time, &f->last_time);

    f->carry_duration = delay - (float)elapsed_time;
    if (f->carry_duration > FPS_MAX_DURATION) {
        f->carry_duration = FPS_MAX_DURATION;
    } else if (f->carry_duration < -FPS_MAX_DURATION) {
        f->carry_duration = -FPS_MAX_DURATION;
    }

    if (fps_time_since(f->fps_last_time, NULL) > 1000) {
        f->fps = f->fps_counter;
        f->fps_counter = 0;
        f->fps_last_time += 1000;
    }
    f->fps_counter++;

    return (float)elapsed_time / 1000;
}
