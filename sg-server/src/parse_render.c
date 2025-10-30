#include "parse_render.h"

bool parse_render(SdlServer *s, char *action) {
    char *p1 = strtok(NULL, " ");
    char *p2 = strtok(NULL, " ");
    char *p3 = strtok(NULL, " ");

    if (!strcmp(action, "set") && p1) {
        if (!strcmp(p1, "clear") && !p2) {
            // set render clear
            game_render_clear(s->game);
            return true;
        } else if (!strcmp(p1, "present") && !p2) {
            // set render present
            SDL_RenderPresent(s->game->renderer);
            return true;
        } else if (!strcmp(p1, "intscale") && p2 && !p3) {
            if (!strcmp(p2, "enable")) {
                // set render intscale enable
                SDL_RenderSetIntegerScale(s->game->renderer, SDL_TRUE);
                return true;
            } else if (!strcmp(p2, "disable")) {
                // set render intscale disable
                SDL_RenderSetIntegerScale(s->game->renderer, SDL_FALSE);
                return true;
            }
        } else if (!strcmp(p1, "scaling") && p2 && !p3) {
            // set render scaling (nearest|linear|best)
            if (!strcmp(p2, "nearest")) {
                s->game->scale_mode = SDL_ScaleModeNearest;
                return true;
            } else if (!strcmp(p2, "linear")) {
                s->game->scale_mode = SDL_ScaleModeLinear;
                return true;
            } else if (!strcmp(p2, "best")) {
                s->game->scale_mode = SDL_ScaleModeBest;
                return true;
            }
        } else if (!strcmp(p1, "color") && p1) {
            // set render color <r> <g> <b> <a>
            char *p4 = strtok(NULL, " ");
            char *p5 = strtok(NULL, " ");
            char *p6 = strtok(NULL, " ");
            if (p4 && !p6) {
                Uint8 r = 0;
                if (!str_to_u8(p2, &r, s->orig_str)) {
                    return false;
                }
                Uint8 g = 0;
                if (!str_to_u8(p3, &g, s->orig_str)) {
                    return false;
                }
                Uint8 b = 0;
                if (!str_to_u8(p4, &b, s->orig_str)) {
                    return false;
                }
                Uint8 a = 255;
                if (p5) {
                    if (!str_to_u8(p5, &a, s->orig_str)) {
                        return false;
                    }
                }
                SDL_SetRenderDrawColor(s->game->renderer, r, g, b, a);
                s->game->render_color.r = r;
                s->game->render_color.g = g;
                s->game->render_color.b = b;
                s->game->render_color.a = a;
                return true;
            }
        }
    }

    fprintf(stderr, "Error: \"%s\" is not correct.\n", s->orig_str);
    return false;
}
