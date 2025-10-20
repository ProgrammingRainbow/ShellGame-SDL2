#include "parse_mouse.h"

bool parse_mouse(SdlServer *s, char *action) {
    char *p1 = strtok(NULL, " ");
    char *p2 = strtok(NULL, " ");
    if (!strcmp(action, "get")) {
        char *p3 = strtok(NULL, " ");
        if (!strcmp(p1, "x") && !p2) {
            // get mouse x -> <bool>
            s->msg.type = SEND_INT;
            s->msg.i_val = s->game->mouse_x;
            return true;
        } else if (!strcmp(p1, "y") && !p2) {
            // get mouse y -> <bool>
            s->msg.type = SEND_INT;
            s->msg.i_val = s->game->mouse_y;
            return true;
        } else if (p2 && !p3) {
            Uint8 button = 0;
            if (!str_to_u8(p2, &button, s->orig_str)) {
                return false;
            }
            s->msg.type = SEND_BOOL;
            if (!strcmp(p1, "h") || !strcmp(p1, "held")) {
                // get mouse (h|held) <button> -> <bool>
                s->msg.b_val = mouse_button_held(s->game, button);
                return true;
            } else if (!strcmp(p1, "p") || !strcmp(p1, "pressed")) {
                // get mouse (p|pressed) <button> -> <bool>
                s->msg.b_val = mouse_button_pressed(s->game, button);
                return true;
            } else if (!strcmp(p1, "r") || !strcmp(p1, "released")) {
                // get mouse (r|released) <button> -> <bool>
                s->msg.b_val = mouse_button_released(s->game, button);
                return true;
            }
        }
    } else if (!strcmp(action, "arr") && p1) {
        s->msg.type = SEND_STRING;
        Uint8 button = 0;
        if ((!strcmp(p1, "h") || !strcmp(p1, "held")) && p2) {
            // arr mouse (h|held) {button array} -> {bool array}
            while (p2) {
                if (!str_to_u8(p2, &button, s->orig_str)) {
                    return false;
                }
                if (!int_to_sval(s, mouse_button_held(s->game, button))) {
                    return false;
                }
                p2 = strtok(NULL, " ");
            }
            return true;
        } else if ((!strcmp(p1, "p") || !strcmp(p1, "pressed")) && p2) {
            // arr mouse (p|pressed) {button array} -> {bool array}
            while (p2) {
                if (!str_to_u8(p2, &button, s->orig_str)) {
                    return false;
                }
                if (!int_to_sval(s, mouse_button_pressed(s->game, button))) {
                    return false;
                }
                p2 = strtok(NULL, " ");
            }
            return true;
        } else if ((!strcmp(p1, "r") || !strcmp(p1, "released")) && p2) {
            // arr mouse (r|released) {button array} -> {bool array}
            while (p2) {
                if (!str_to_u8(p2, &button, s->orig_str)) {
                    return false;
                }
                if (!int_to_sval(s, mouse_button_released(s->game, button))) {
                    return false;
                }
                p2 = strtok(NULL, " ");
            }
            return true;
        } else if (!strcmp(p1, "pos") && !p2) {
            // arr mouse pos -> {int array}
            if (!int_to_sval(s, s->game->mouse_x)) {
                return false;
            }
            if (!int_to_sval(s, s->game->mouse_y)) {
                return false;
            }
            return true;
        }
    }

    fprintf(stderr, "Error: \"%s\" is not correct.\n", s->orig_str);
    return false;
}
