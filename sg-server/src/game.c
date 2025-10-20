#include "fps.h"
#include "images.h"
#include "init_sdl.h"
#include "music.h"
#include "rects.h"
#include "sounds.h"
#include "sprites.h"
#include "texts.h"

Game *game_new(void) {
    Game *game = calloc(1, sizeof(Game));
    if (game == NULL) {
        fprintf(stderr, "Error in Calloc of New Game.\n");
        return NULL;
    }

    strncpy(game->title, WINDOW_TITLE, sizeof(game->title) - 1);
    game->title[sizeof(game->title) - 1] = '\0';

    game->width = WINDOW_WIDTH;
    game->height = WINDOW_HEIGHT;

    game->images.elem_size = sizeof(Image);
    game->images.inc_size = 10;

    game->rects.elem_size = sizeof(Rect);
    game->rects.inc_size = 10;

    game->texts.elem_size = sizeof(Text);
    game->texts.inc_size = 10;

    game->sprites.elem_size = sizeof(Sprite);
    game->sprites.inc_size = 10;

    game->sounds.elem_size = sizeof(Sound);
    game->sounds.inc_size = 10;

    game->music.elem_size = sizeof(Music);
    game->music.inc_size = 10;

    game->keystate = SDL_GetKeyboardState(NULL);

    return game;
}

void game_free(struct Game *g) {
    if (g) {
        Mix_HaltMusic();
        Mix_HaltChannel(-1);

        if (g->texts.texts) {
            texts_free(g);
            g->texts.texts = NULL;
        }

        if (g->images.images) {
            images_free(g);
            g->images.images = NULL;
        }

        if (g->rects.rects) {
            rects_free(g);
            g->rects.rects = NULL;
        }

        if (g->sprites.sprites) {
            sprites_free(g);
            g->sprites.sprites = NULL;
        }

        if (g->sounds.sounds) {
            sounds_free(g);
            g->sounds.sounds = NULL;
        }

        if (g->music.music) {
            music_free_all(g);
            g->music.music = NULL;
        }

        if (g->fps) {
            fps_free(g->fps);
            g->fps = NULL;
        }

        if (g->renderer) {
            SDL_DestroyRenderer(g->renderer);
            g->renderer = NULL;
        }
        if (g->window) {
            SDL_DestroyWindow(g->window);
            g->window = NULL;
        }

        Mix_Quit();
        TTF_Quit();
        SDL_Quit();

        free(g);

        printf("SDL Shutdown.\n");
    }
}

bool game_start(Game *g) {
    if (!game_init_sdl(g, g->title, g->width, g->height)) {
        return false;
    }

    g->fps = fps_new();
    if (g->fps == NULL) {
        return false;
    }

    g->running = true;

    return true;
}

bool game_icon(Game *g, const char *filename) {
    SDL_Surface *icon = IMG_Load(filename);
    if (!icon) {
        fprintf(stderr, "Error loading Icon Surface: %s\n", SDL_GetError());
        return false;
    }

    SDL_SetWindowIcon(g->window, icon);
    SDL_FreeSurface(icon);

    return true;
}

void game_set_size(Game *g, int w, int h) {
    g->width = w;
    g->height = h;

    SDL_SetWindowSize(g->window, w, h);
    SDL_RenderSetLogicalSize(g->renderer, w, h);
}

bool game_update(Game *g) {
    while (SDL_PollEvent(&g->event)) {
        if (g->event.type == SDL_QUIT) {
            return false;
        }
    }

    memcpy(g->prev_keystate, g->curr_keystate,
           SDL_NUM_SCANCODES * sizeof(bool));
    memcpy(g->curr_keystate, g->keystate, SDL_NUM_SCANCODES * sizeof(bool));

    g->prev_mouse_buttons = g->curr_mouse_buttons;
    g->curr_mouse_buttons = SDL_GetMouseState(&g->mouse_x, &g->mouse_y);

    g->dt = fps_update(g->fps);

    return true;
}

bool game_key_press(Game *g, SDL_Scancode sc) {
    return !g->prev_keystate[sc] && g->curr_keystate[sc];
}

bool game_key_release(Game *g, SDL_Scancode sc) {
    return g->prev_keystate[sc] && !g->curr_keystate[sc];
}

bool mouse_button_pressed(Game *g, Uint8 button) {
    return !(g->prev_mouse_buttons & SDL_BUTTON(button)) &&
           (g->curr_mouse_buttons & SDL_BUTTON(button));
}

bool mouse_button_released(Game *g, Uint8 button) {
    return (g->prev_mouse_buttons & SDL_BUTTON(button)) &&
           !(g->curr_mouse_buttons & SDL_BUTTON(button));
}

bool mouse_button_held(Game *g, Uint8 button) {
    return g->curr_mouse_buttons & SDL_BUTTON(button);
}
