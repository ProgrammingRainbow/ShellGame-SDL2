#include "parse_render.h"

bool parse_render(SdlServer *s, char *action) {
    char *p1 = strtok(NULL, " ");
    char *p2 = strtok(NULL, " ");

    if (!strcmp(action, "set") && p1 && !p2) {
        if (!strcmp(p1, "clear")) {
            // set render clear
            SDL_RenderClear(s->game->renderer);
            return true;
        } else if (!strcmp(p1, "present")) {
            // set render present
            SDL_RenderPresent(s->game->renderer);
            return true;
        }
    }

    fprintf(stderr, "Error: \"%s\" is not correct.\n", s->orig_str);
    return false;
}
