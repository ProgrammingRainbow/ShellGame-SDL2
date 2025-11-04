#include "parser.h"
#include "buffer.h"
#include "parse_image.h"
#include "parse_key.h"
#include "parse_mouse.h"
#include "parse_music.h"
#include "parse_rect.h"
#include "parse_render.h"
#include "parse_sg.h"
#include "parse_sound.h"
#include "parse_sprite.h"
#include "parse_text.h"

bool str_to_u8(const char *str, Uint8 *value, const char *orig_str) {
    if (str && *str) {
        char *endptr = NULL;
        unsigned long long_value = strtoul(str, &endptr, 10);

        if (endptr != str && *endptr == '\0') {
            if (long_value <= 255) {
                *value = (Uint8)long_value;
                return true;
            }
            fprintf(stderr, "Error: %li not in bounds %i to %i.\n", long_value,
                    0, 255);
        } else {
            fprintf(stderr, "Error: \"%s\" is not a valid uint8.\n", str);
        }
    }

    fprintf(stderr, "Error: \"%s\" is not correct.\n", orig_str);
    return false;
}

bool str_to_i(const char *str, int *value, const char *orig_str) {
    if (str && *str) {
        char *endptr = NULL;
        long long_value = strtol(str, &endptr, 10);

        if (endptr != str && *endptr == '\0') {
            if (long_value <= INT_MAX && long_value >= INT_MIN) {
                *value = (int)long_value;
                return true;
            }
            fprintf(stderr, "Error: %li not in bounds %i to %i.\n", long_value,
                    INT_MIN, INT_MAX);
        } else {
            fprintf(stderr, "Error: \"%s\" is not a valid integer.\n", str);
        }
    }

    fprintf(stderr, "Error: \"%s\" is not correct.\n", orig_str);
    return false;
}

bool str_to_ul(const char *str, unsigned long *value, const char *orig_str) {
    if (str && *str) {
        char *endptr = NULL;
        unsigned long ul_value = strtoul(str, &endptr, 10);

        if (endptr != str && *endptr == '\0') {
            *value = ul_value;
            return true;
        } else {
            fprintf(stderr, "Error: \"%s\" is not a valid unsigned long.\n",
                    str);
        }
    }

    fprintf(stderr, "Error: \"%s\" is not correct.\n", orig_str);
    return false;
}

bool str_to_f(const char *str, float *value, const char *orig_str) {
    if (str && *str) {
        char *endptr = NULL;
        double double_value = strtod(str, &endptr);

        if (endptr != str && *endptr == '\0') {
            if (double_value <= FLT_MAX && double_value >= -FLT_MAX) {
                *value = (float)double_value;
                return true;
            }
            fprintf(stderr, "Error: Int %f not in bounds %lf to %lf.\n",
                    double_value, FLT_MIN, FLT_MAX);
        } else {
            fprintf(stderr, "Error: \"%s\" is not a valid float.\n", str);
        }
    }

    fprintf(stderr, "Error: \"%s\" is not correct.\n", orig_str);
    return false;
}

bool str_to_id(Buffer *buff, const char *str, int *index,
               const char *orig_str) {
    if (!strcmp(str, "NULL")) {
        *index = -1;
        return true;
    }

    if (!str_to_i(str, index, orig_str)) {
        return false;
    }

    if (*index == -1) {
        // fprintf(stderr, "Error: \"%s\" means the id was probably freed.\n",
        //         str);
        return true;
    } else if (*index < 0 || *index >= buff->cap) {
        fprintf(stderr, "Error: \"%s\" is not a valid id.\n", str);
    } else {
        if (buffer_check_id(buff, *index)) {
            return true;
        } else {
            fprintf(stderr, "Error: \"%s\" is not a valid id.\n", str);
        }
    }

    fprintf(stderr, "Error: \"%s\" is not correct.\n", orig_str);
    return false;
}

bool int_to_sval(SdlServer *s, int value) {
    size_t str_len = strlen(s->msg.s_val);
    if (str_len) {
        s->msg.s_val[str_len] = ' ';
        str_len++;
    }

    size_t val_len = (size_t)snprintf(NULL, 0, "%i", value);
    if (str_len + val_len + 1 >= BUFFER_SIZE) {
        fprintf(stderr, "Error: Buffer size %d exceeded.\n", BUFFER_SIZE);
        fprintf(stderr, "Error: \"%s\" is not correct.\n", s->orig_str);
        return false;
    }

    char *str = s->msg.s_val + str_len;
    snprintf(str, val_len + 1, "%i", value);

    return true;
}

bool float_to_sval(SdlServer *s, float value) {
    size_t str_len = strlen(s->msg.s_val);
    if (str_len) {
        s->msg.s_val[str_len] = ' ';
        str_len++;
    }

    size_t val_len = (size_t)snprintf(NULL, 0, "%f", value);
    if (str_len + val_len + 1 >= BUFFER_SIZE) {
        fprintf(stderr, "Error: Buffer size %d exceeded.\n", BUFFER_SIZE);
        fprintf(stderr, "Error: \"%s\" is not correct.\n", s->orig_str);
        return false;
    }

    char *str = s->msg.s_val + str_len;
    snprintf(str, val_len + 1, "%f", value);

    return true;
}

bool str_to_field(const char *str, RectField *field, const char *orig_str) {
    if (!strcmp(str, "x") || !strcmp(str, "l") || !strcmp(str, "left")) {
        *field = RECT_X;
    } else if (!strcmp(str, "y") || !strcmp(str, "t") || !strcmp(str, "top")) {
        *field = RECT_Y;
    } else if (!strcmp(str, "w") || !strcmp(str, "width")) {
        *field = RECT_W;
    } else if (!strcmp(str, "h") || !strcmp(str, "height")) {
        *field = RECT_H;
    } else if (!strcmp(str, "r") || !strcmp(str, "right")) {
        *field = RECT_R;
    } else if (!strcmp(str, "b") || !strcmp(str, "bottom")) {
        *field = RECT_B;
    } else if (!strcmp(str, "cx") || !strcmp(str, "centerx")) {
        *field = RECT_CX;
    } else if (!strcmp(str, "cy") || !strcmp(str, "centery")) {
        *field = RECT_CY;
    } else {
        fprintf(stderr, "Error: \"%s\" is not a valid rect field.\n", str);
        fprintf(stderr, "Error: \"%s\" is not correct.\n", orig_str);
        return false;
    }

    return true;
}

bool str_to_pos(const char *str, RectField *field, const char *orig_str) {
    if (!strcmp(str, "w") || !strcmp(str, "width")) {
        *field = RECT_W;
    } else if (!strcmp(str, "h") || !strcmp(str, "height")) {
        *field = RECT_H;
    } else {
        fprintf(stderr, "Error: \"%s\" is not a valid rect field.\n", str);
        fprintf(stderr, "Error: \"%s\" is not correct.\n", orig_str);
        return false;
    }

    return true;
}

bool str_to_flip(const char *str, SDL_RendererFlip *flip,
                 const char *orig_str) {
    if (!strcmp(str, "h")) {
        *flip = SDL_FLIP_HORIZONTAL;
    } else if (!strcmp(str, "v")) {
        *flip = SDL_FLIP_VERTICAL;
    } else if (!strcmp(str, "n")) {
        *flip = SDL_FLIP_NONE;
    } else {
        fprintf(stderr, "Error: %s is not h, v, or n.\n", str);
        fprintf(stderr, "Error: \"%s\" is not correct.\n", orig_str);
        return false;
    }

    return true;
}

bool get_scancode(const char *input, SDL_Scancode *code, const char *orig_str) {
    if (strlen(input) == 1) {
        char c = input[0];
        if (c >= 32 && c <= 126) {
            *code = SDL_GetScancodeFromKey((SDL_Keycode)c);
            return true;
        }
    } else if (strcmp(input, "space") == 0) {
        *code = SDL_SCANCODE_SPACE;
        return true;
    } else if (strcmp(input, "enter") == 0) {
        *code = SDL_SCANCODE_RETURN;
        return true;
    } else if (strcmp(input, "tab") == 0) {
        *code = SDL_SCANCODE_TAB;
        return true;
    } else if (strcmp(input, "left") == 0) {
        *code = SDL_SCANCODE_LEFT;
        return true;
    } else if (strcmp(input, "right") == 0) {
        *code = SDL_SCANCODE_RIGHT;
        return true;
    } else if (strcmp(input, "up") == 0) {
        *code = SDL_SCANCODE_UP;
        return true;
    } else if (strcmp(input, "down") == 0) {
        *code = SDL_SCANCODE_DOWN;
        return true;
    } else if (strcmp(input, "esc") == 0 || strcmp(input, "escape") == 0) {
        *code = SDL_SCANCODE_ESCAPE;
        return true;
    }

    *code = SDL_SCANCODE_UNKNOWN;
    fprintf(stderr, "Error: \"%s\" is not a valid key scancode.\n", input);
    fprintf(stderr, "Error: \"%s\" is not correct.\n", orig_str);
    return false;
}

bool parse_line(SdlServer *s) {
    if (!s->game->running) {
        if (strcmp(s->orig_str, "start sg")) {
            printf("Warning: Run \"start sg\" before \"%s\"!\n", s->orig_str);
            if (!game_start(s->game)) {
                s->running = false;
                return false;
            }
        }
    }

    char *action = strtok(s->token_str, " ");
    char *target = strtok(NULL, " ");

    if (target) {
        if (!strcmp(action, "get") || !strcmp(action, "getf") ||
            !strcmp(action, "arr") || !strcmp(action, "new") ||
            !strcmp(action, "free")) {
            s->msg.enable = true;
        }

        if (!strcmp(target, "sg")) {
            return parse_sg(s, action);
        } else if (!strcmp(target, "rect")) {
            return parse_rect(s, action);
        } else if (!strcmp(target, "key")) {
            return parse_key(s, action);
        } else if (!strcmp(target, "render")) {
            return parse_render(s, action);
        } else if (!strcmp(target, "image")) {
            return parse_image(s, action);
        } else if (!strcmp(target, "text")) {
            return parse_text(s, action);
        } else if (!strcmp(target, "sprite")) {
            return parse_sprite(s, action);
        } else if (!strcmp(target, "sound")) {
            return parse_sound(s, action);
        } else if (!strcmp(target, "music")) {
            return parse_music(s, action);
        } else if (!strcmp(target, "mouse")) {
            return parse_mouse(s, action);
        }
    }

    fprintf(stderr, "Error: \"%s\" is not correct.\n", s->orig_str);
    return false;
}
