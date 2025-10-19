#include "init_sdl.h"

bool game_init_sdl(struct Game *g, const char *title, int width, int height) {
    if (SDL_Init(SDL_FLAGS)) {
        fprintf(stderr, "Error initializing SDL2: %s\n", SDL_GetError());
        return false;
    }

    if ((IMG_Init(IMG_FLAGS) & IMG_FLAGS) != IMG_FLAGS) {
        fprintf(stderr, "Error initializing SDL_image: %s\n", IMG_GetError());
        return false;
    }

    if (TTF_Init()) {
        fprintf(stderr, "Error initializing SDL_ttf: %s\n", TTF_GetError());
        return false;
    }

    if ((Mix_Init(MIX_FLAGS) & MIX_FLAGS) != MIX_FLAGS) {
        fprintf(stderr, "Error initializing SDL_mixer: %s\n", Mix_GetError());
        return false;
    }

    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT,
                      MIX_DEFAULT_CHANNELS, CHUNK_SIZE) < 0) {
        fprintf(stderr, "Error Opening Audio: %s\n", Mix_GetError());
        return false;
    }

    g->window =
        SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                         WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!g->window) {
        fprintf(stderr, "Error creating SDL_Window: %s\n", SDL_GetError());
        return false;
    }

    g->renderer = SDL_CreateRenderer(g->window, -1, RENDERER_FLAGS);
    if (!g->renderer) {
        fprintf(stderr, "Error creating SDL_Renderer: %s\n", SDL_GetError());
        return false;
    }

    SDL_SetWindowSize(g->window, width, height);
    SDL_RenderSetLogicalSize(g->renderer, width, height);

    return true;
}
