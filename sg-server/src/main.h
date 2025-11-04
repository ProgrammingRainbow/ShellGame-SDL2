#ifndef MAIN_H
#define MAIN_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#define SDL_FLAGS SDL_INIT_EVERYTHING
#define IMG_FLAGS IMG_INIT_PNG
#define MIX_FLAGS MIX_INIT_OGG
#define RENDERER_FLAGS SDL_RENDERER_ACCELERATED
#define CHUNK_SIZE 1024

#define WINDOW_TITLE "Bash SDL"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define FONT_FILE "fonts/freesansbold.ttf"
#define FONT_SIZE 32
#define FONT_COLOR 255, 255, 255, 255

#define FPS_TARGET 60
#define FPS_MAX_DURATION 50

#define BUFFER_SIZE 10240

typedef enum RectField {
    RECT_X,
    RECT_Y,
    RECT_W,
    RECT_H,
    RECT_R,
    RECT_B,
    RECT_CX,
    RECT_CY
} RectField;

typedef enum SendType {
    SEND_NONE,
    SEND_BOOL,
    SEND_INT,
    SEND_FLOAT,
    SEND_STRING
} SendType;

typedef struct SendMsg {
        enum SendType type;
        union {
                bool b_val;
                int i_val;
                float f_val;
                char s_val[BUFFER_SIZE];
        };
        bool enable;
} SendMsg;

#endif
