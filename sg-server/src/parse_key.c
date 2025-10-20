#include "parse_key.h"

bool parse_key(SdlServer *s, char *action) {
    char *p1 = strtok(NULL, " ");
    char *p2 = strtok(NULL, " ");
    if (!strcmp(action, "get") && p2) {
        char *p3 = strtok(NULL, " ");
        if (!p3) {
            SDL_Scancode key = SDL_SCANCODE_UNKNOWN;
            if (!get_scancode(p2, &key, s->orig_str)) {
                return false;
            }
            s->msg.type = SEND_BOOL;
            if (!strcmp(p1, "h") || !strcmp(p1, "held")) {
                // get key (h|held) <key> -> <bool>
                s->msg.b_val = s->game->curr_keystate[key];
                return true;
            } else if (!strcmp(p1, "p") || !strcmp(p1, "pressed")) {
                // get key (p|pressed) <key> -> <bool>
                s->msg.b_val = game_key_press(s->game, key);
                return true;
            } else if (!strcmp(p1, "r") || !strcmp(p1, "released")) {
                // get key (r|released) <key> -> <bool>
                s->msg.b_val = game_key_release(s->game, key);
                return true;
            }
        }
    } else if (!strcmp(action, "arr") && p2) {
        s->msg.type = SEND_STRING;
        SDL_Scancode key = SDL_SCANCODE_UNKNOWN;
        if (!strcmp(p1, "h") || !strcmp(p1, "held")) {
            // arr key (h|held) {key array} -> {bool array}
            while (p2) {
                if (!get_scancode(p2, &key, s->orig_str)) {
                    return false;
                }
                if (!int_to_sval(s, s->game->curr_keystate[key])) {
                    return false;
                }
                p2 = strtok(NULL, " ");
            }
            return true;
        } else if (!strcmp(p1, "p") || !strcmp(p1, "pressed")) {
            // arr key (p|pressed) {key array} -> {bool array}
            while (p2) {
                if (!get_scancode(p2, &key, s->orig_str)) {
                    return false;
                }
                if (!int_to_sval(s, game_key_press(s->game, key))) {
                    return false;
                }
                p2 = strtok(NULL, " ");
            }
            return true;
        } else if (!strcmp(p1, "r") || !strcmp(p1, "released")) {
            // arr key (r|released) {key array} -> {bool array}
            while (p2) {
                if (!get_scancode(p2, &key, s->orig_str)) {
                    return false;
                }
                if (!int_to_sval(s, game_key_release(s->game, key))) {
                    return false;
                }
                p2 = strtok(NULL, " ");
            }
            return true;
        }
    }

    fprintf(stderr, "Error: \"%s\" is not correct.\n", s->orig_str);
    return false;
}
