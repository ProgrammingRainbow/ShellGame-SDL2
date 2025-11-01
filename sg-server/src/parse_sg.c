#include "parse_sg.h"
#include "fps.h"

bool parse_sg(SdlServer *s, char *action) {
    char *p1 = strtok(NULL, " ");
    char *p2 = strtok(NULL, " ");
    char *p3 = strtok(NULL, " ");
    char *p4 = strtok(NULL, " ");

    if (!strcmp(action, "update") && !p1) {
        // update sg
        if (!game_update(s->game)) {
            s->running = false;
        }
        return true;
    } else if (!strcmp(action, "start") && !p1) {
        // start sg
        if (!game_start(s->game)) {
            s->running = false;
            return false;
        }
        return true;
    } else if (!strcmp(action, "quit") && !p1) {
        // quit sg
        s->running = false;
        return true;
    } else if (!strcmp(action, "set")) {
        if (!strcmp(p1, "title") && p2) {
            // set sg title <string>
            char *title = s->orig_str + 13;
            SDL_SetWindowTitle(s->game->window, title);
            return true;
        } else if (!strcmp(p1, "icon") && p2 && !p3) {
            // set sg icon <filename>
            return game_icon(s->game, p2);
        } else if (!strcmp(p1, "size") && p3 && !p4) {
            // set sg size <width> <height>
            int w = 0;
            if (!str_to_i(p2, &w, s->orig_str)) {
                return false;
            }
            int h = 0;
            if (!str_to_i(p3, &h, s->orig_str)) {
                return false;
            }
            game_set_size(s->game, w, h);
            return true;
        } else if (!strcmp(p1, "scale") && p2 && !p3) {
            // set sg scale <float>
            float scale = 0;
            if (!str_to_f(p2, &scale, s->orig_str)) {
                return false;
            }
            game_set_scale(s->game, scale);
            return true;
        } else if (!strcmp(p1, "resizable") && p2 && !p3) {
            if (!strcmp(p2, "enable")) {
                // set sg resizable enable
                SDL_SetWindowResizable(s->game->window, SDL_TRUE);
                return true;
            } else if (!strcmp(p2, "disable")) {
                // set sg resizable disable
                SDL_SetWindowResizable(s->game->window, SDL_FALSE);
                return true;
            }
        } else if (!strcmp(p1, "fullscreen") && p2 && !p3) {
            if (!strcmp(p2, "enable")) {
                // set sg fullscreen enable
                SDL_SetWindowFullscreen(s->game->window, SDL_WINDOW_FULLSCREEN);
                return true;
            } else if (!strcmp(p2, "desktop")) {
                // set sg fullscreen desktop
                SDL_SetWindowFullscreen(s->game->window,
                                        SDL_WINDOW_FULLSCREEN_DESKTOP);
                return true;
            } else if (!strcmp(p2, "disable")) {
                // set sg fullscreen disable
                SDL_SetWindowFullscreen(s->game->window, 0);
                return true;
            } else if (!strcmp(p2, "toggle")) {
                // set sg fullscreen toggle
                if (SDL_GetWindowFlags(s->game->window) &
                    SDL_WINDOW_FULLSCREEN) {
                    SDL_SetWindowFullscreen(s->game->window, false);
                } else {
                    SDL_SetWindowFullscreen(s->game->window, true);
                }
                return true;
            }
        } else if (!strcmp(p1, "fps") && p2 && !p3) {
            // set sg fps <int>
            float target = 0;
            if (!str_to_f(p2, &target, s->orig_str)) {
                return false;
            }
            if (target > 0) {
                fps_set_target(s->game->fps, target);
                return true;
            }
        }
    } else if (!strcmp(action, "get") && p1 && !p2) {
        if (!strcmp(p1, "width")) {
            // get sg width -> <int>
            s->msg.type = SEND_INT;
            int w = 0;
            int h = 0;
            SDL_GetWindowSize(s->game->window, &w, &h);
            s->msg.i_val = w;
            return true;
        } else if (!strcmp(p1, "height")) {
            // get sg height -> <int>
            s->msg.type = SEND_INT;
            int w = 0;
            int h = 0;
            SDL_GetWindowSize(s->game->window, &w, &h);
            s->msg.i_val = h;
            return true;
        } else if (!strcmp(p1, "fps")) {
            // get sg fps -> <int>
            s->msg.type = SEND_INT;
            s->msg.i_val = s->game->fps->fps;
            return true;
        }
    } else if (!strcmp(action, "arr") && p1 && !p2) {
        if (!strcmp(p1, "size")) {
            // arr sg size -> {int array}
            s->msg.type = SEND_STRING;
            int w = 0;
            int h = 0;
            SDL_GetWindowSize(s->game->window, &w, &h);
            if (!int_to_sval(s, w)) {
                return false;
            }
            if (!int_to_sval(s, h)) {
                return false;
            }
            return true;
        }
    }

    fprintf(stderr, "Error: \"%s\" is not correct.\n", s->orig_str);
    return false;
}
