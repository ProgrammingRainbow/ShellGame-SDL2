#include "parse_render.h"

bool parse_render(SdlServer *s, char *action) {
    char *p1 = strtok(NULL, " ");
    char *p2 = strtok(NULL, " ");

    if (!strcmp(action, "set") && p1) {
        if (!strcmp(p1, "clear") && !p2) {
            // set render clear
            SDL_RenderClear(s->game->renderer);
            return true;
        } else if (!strcmp(p1, "present") && !p2) {
            // set render present
            SDL_RenderPresent(s->game->renderer);
            return true;
        } else if (!strcmp(p1, "color") && p1) {
            char *p3 = strtok(NULL, " ");
            char *p4 = strtok(NULL, " ");
            char *p5 = strtok(NULL, " ");
            if (p4 && !p5) {
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
                SDL_SetRenderDrawColor(s->game->renderer, r, g, b, 255);
                return true;
            }
        }
    }

    fprintf(stderr, "Error: \"%s\" is not correct.\n", s->orig_str);
    return false;
}
