#ifndef GAME_H
#define GAME_H

#include "buffer.h"

#define TITLE_LENGTH 256

typedef struct Game {
        SDL_Window *window;
        SDL_Renderer *renderer;
        SDL_ScaleMode scale_mode;
        char title[TITLE_LENGTH];
        int width;
        int height;
        Buffer images;
        Buffer rects;
        Buffer texts;
        Buffer sprites;
        Buffer sounds;
        Buffer music;
        struct Fps *fps;
        SDL_Event event;
        float dt;
        const Uint8 *keystate;
        Uint8 prev_keystate[SDL_NUM_SCANCODES];
        Uint8 curr_keystate[SDL_NUM_SCANCODES];
        Uint32 prev_mouse_buttons;
        Uint32 curr_mouse_buttons;
        int mouse_x;
        int mouse_y;
        bool running;
} Game;

typedef struct SdlServer {
        char orig_str[BUFFER_SIZE];
        char token_str[BUFFER_SIZE];
        Game *game;
        SendMsg msg;
        bool running;
} SdlServer;

Game *game_new(void);
void game_free(struct Game *game);
bool game_start(Game *g);
bool game_icon(Game *g, const char *filename);
void game_set_size(Game *g, int w, int h);
bool game_update(Game *g);
bool game_key_press(Game *g, SDL_Scancode sc);
bool game_key_release(Game *g, SDL_Scancode sc);
bool mouse_button_pressed(Game *g, Uint8 button);
bool mouse_button_released(Game *g, Uint8 button);
bool mouse_button_held(Game *g, Uint8 button);

#endif
