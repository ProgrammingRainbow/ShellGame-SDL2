#include "parse_sprite.h"

bool parse_sprite(SdlServer *s, char *action) {
    char *p1 = strtok(NULL, " ");
    if (!strcmp(action, "new") && p1) {
        // new sprite <filename> -> <sprt_id>
        char *p2 = strtok(NULL, " ");
        if (!p2) {
            int sprt_id = 0;
            if (!sprite_new(s->game, &sprt_id, p1)) {
                return false;
            }
            s->msg.type = SEND_INT;
            s->msg.i_val = sprt_id;
            return true;
        }
    } else if (!strcmp(action, "draw") && p1) {
        // draw sprite {sprt_id array}
        while (p1) {
            int sprt_id = 0;
            if (!str_to_id(&s->game->sprites, p1, &sprt_id, s->orig_str)) {
                return false;
            }
            sprite_draw(s->game, sprt_id);
            p1 = strtok(NULL, " ");
        }
        return true;
    } else if (!strcmp(action, "update")) {
        char *p2 = strtok(NULL, " ");
        char *p3 = strtok(NULL, " ");
        if (!strcmp(p1, "pos")) {
            // update sprite pos <x_vel> <y_vel> {sprt_id array}
            char *p4 = strtok(NULL, " ");
            if (p4) {
                float x_vel = 0;
                if (!str_to_f(p2, &x_vel, s->orig_str)) {
                    return false;
                }
                float y_vel = 0;
                if (!str_to_f(p3, &y_vel, s->orig_str)) {
                    return false;
                }
                while (p4) {
                    int sprt_id = 0;
                    if (!str_to_id(&s->game->sprites, p4, &sprt_id,
                                   s->orig_str)) {
                        return false;
                    }
                    sprite_update_pos(s->game, sprt_id, x_vel, y_vel);
                    p4 = strtok(NULL, " ");
                }
                return true;
            }
        } else if (!strcmp(p1, "angle") && p3) {
            // update sprite angle <angle> {sprt_id array}
            float angle = 0;
            if (!str_to_f(p2, &angle, s->orig_str)) {
                return false;
            }
            while (p3) {
                int sprt_id = 0;
                if (!str_to_id(&s->game->sprites, p3, &sprt_id, s->orig_str)) {
                    return false;
                }
                sprite_update_angle(s->game, sprt_id, angle);
                p3 = strtok(NULL, " ");
            }
            return true;
        } else if (p3) {
            // update sprite (x|y) <float> {rect_id array}
            RectField field = 0;
            if (!strcmp(p1, "x")) {
                field = RECT_X;
            } else if (!strcmp(p1, "y")) {
                field = RECT_Y;
            }
            if (field == RECT_X || field == RECT_Y) {
                float value = 0;
                if (!str_to_f(p2, &value, s->orig_str)) {
                    return false;
                }
                while (p3) {
                    int sprt_id = 0;
                    if (!str_to_id(&s->game->sprites, p3, &sprt_id,
                                   s->orig_str)) {
                        return false;
                    }
                    if (field == RECT_X) {
                        sprite_update_pos(s->game, sprt_id, value, 0);
                    } else {
                        sprite_update_pos(s->game, sprt_id, 0, value);
                    }
                    p3 = strtok(NULL, " ");
                }
                return true;
            }
        }
    } else if (!strcmp(action, "set") && p1) {
        char *p2 = strtok(NULL, " ");
        char *p3 = strtok(NULL, " ");
        if (!strcmp(p1, "image") && p3) {
            // set sprite image <filename> {sprt_id array}
            while (p3) {
                int sprt_id = 0;
                if (!str_to_id(&s->game->sprites, p3, &sprt_id, s->orig_str)) {
                    return false;
                }
                if (!sprite_update_image(s->game, sprt_id, p2)) {
                    return false;
                }
                p3 = strtok(NULL, " ");
            }
            return true;
        } else if (!strcmp(p1, "pos") && p3) {
            // set sprite pos <x> <y> {sprt_id array}
            char *p4 = strtok(NULL, " ");
            if (p4) {
                float x_pos = 0;
                if (!str_to_f(p2, &x_pos, s->orig_str)) {
                    return false;
                }
                float y_pos = 0;
                if (!str_to_f(p3, &y_pos, s->orig_str)) {
                    return false;
                }
                while (p4) {
                    int sprt_id = 0;
                    if (!str_to_id(&s->game->sprites, p4, &sprt_id,
                                   s->orig_str)) {
                        return false;
                    }
                    sprite_set_pos(s->game, sprt_id, x_pos, y_pos);
                    p4 = strtok(NULL, " ");
                }
                return true;
            }
        } else if (!strcmp(p1, "angle") && p3) {
            // set sprite angle <angle> {sprt_id array}
            float angle = 0;
            if (!str_to_f(p2, &angle, s->orig_str)) {
                return false;
            }
            while (p3) {
                int sprt_id = 0;
                if (!str_to_id(&s->game->sprites, p3, &sprt_id, s->orig_str)) {
                    return false;
                }
                s->game->sprites.sprites[sprt_id].angle = angle;
                p3 = strtok(NULL, " ");
            }
            return true;
        } else if (!strcmp(p1, "flip") && p3) {
            // set sprite flip <flip> {sprt_id array}
            SDL_RendererFlip flip = 0;
            if (!str_to_flip(p2, &flip, s->orig_str)) {
                return false;
            }
            while (p3) {
                int sprt_id = 0;
                if (!str_to_id(&s->game->sprites, p3, &sprt_id, s->orig_str)) {
                    return false;
                }
                s->game->sprites.sprites[sprt_id].flip = flip;
                p3 = strtok(NULL, " ");
            }
            return true;
        } else if (!strcmp(p1, "size") && p3) {
            // set sprite size <width> <height> {sprt_id array}
            int width = 0;
            if (!str_to_i(p2, &width, s->orig_str)) {
                return false;
            }
            int height = 0;
            if (!str_to_i(p3, &height, s->orig_str)) {
                return false;
            }
            char *p4 = strtok(NULL, " ");
            if (p4) {
                while (p4) {
                    int sprt_id = 0;
                    if (!str_to_id(&s->game->sprites, p4, &sprt_id,
                                   s->orig_str)) {
                        return false;
                    }
                    sprite_set_size(s->game, sprt_id, width, height);
                    p4 = strtok(NULL, " ");
                }
                return true;
            }
        } else if (p3) {
            // set sprite
            // (x|l|left|y|t|top|w|width|h|height|r|right|b|bottom|cx|cy)
            // <float> {sprt_id array}
            RectField field = 0;
            if (!str_to_field(p1, &field, s->orig_str)) {
                return false;
            }
            float value = 0;
            if (!str_to_f(p2, &value, s->orig_str)) {
                return false;
            }
            while (p3) {
                int sprt_id = 0;
                if (!str_to_id(&s->game->sprites, p3, &sprt_id, s->orig_str)) {
                    return false;
                }
                sprite_set_rect_field(s->game, sprt_id, field, value);
                p3 = strtok(NULL, " ");
            }
            return true;
        }
    } else if (!strcmp(action, "get")) {
        char *p2 = strtok(NULL, " ");
        char *p3 = strtok(NULL, " ");
        if (!strcmp(p1, "angle") && p2 && !p3) {
            // get sprite angle <sprt_id> -> <int>
            int sprt_id = 0;
            if (!str_to_id(&s->game->sprites, p2, &sprt_id, s->orig_str)) {
                return false;
            }
            s->msg.type = SEND_INT;
            s->msg.i_val = (int)s->game->sprites.sprites[sprt_id].angle;
            return true;
        } else if (p2 && !p3) {
            // get sprite
            // (x|l|left|y|t|top|w|width|h|height|r|right|b|bottom|cx|cy)
            // <sprt_id> -> <int>
            RectField field = 0;
            if (!str_to_field(p1, &field, s->orig_str)) {
                return false;
            }
            int sprt_id = 0;
            if (!str_to_id(&s->game->sprites, p2, &sprt_id, s->orig_str)) {
                return false;
            }
            s->msg.type = SEND_INT;
            s->msg.i_val = sprite_get_rect_field(s->game, sprt_id, field);
            return true;
        }
    } else if (!strcmp(action, "getf")) {
        char *p2 = strtok(NULL, " ");
        char *p3 = strtok(NULL, " ");
        if (!strcmp(p1, "angle") && p2 && !p3) {
            // getf sprite angle <sprt_id> -> <float>
            int sprt_id = 0;
            if (!str_to_id(&s->game->sprites, p2, &sprt_id, s->orig_str)) {
                return false;
            }
            s->msg.type = SEND_FLOAT;
            s->msg.f_val = s->game->sprites.sprites[sprt_id].angle;
            return true;
        }
    } else if (!strcmp(action, "arr")) {
        s->msg.type = SEND_STRING;
        char *p2 = strtok(NULL, " ");
        if (!strcmp(p1, "pos") && p2) {
            // arr sprite pos <sprt_id> -> {int array}
            char *p3 = strtok(NULL, " ");
            if (!p3) {
                int sprt_id = 0;
                if (!str_to_id(&s->game->sprites, p2, &sprt_id, s->orig_str)) {
                    return false;
                }
                int value =
                    (int)sprite_get_rect_field(s->game, sprt_id, RECT_X);
                if (!int_to_sval(s, value)) {
                    return false;
                }
                value = (int)sprite_get_rect_field(s->game, sprt_id, RECT_Y);
                if (!int_to_sval(s, value)) {
                    return false;
                }
                return true;
            }
        } else if (!strcmp(p1, "angle") && p2) {
            // arr sprite angle {sprt_id array} -> {int array}
            while (p2) {
                int sprt_id = 0;
                if (!str_to_id(&s->game->sprites, p2, &sprt_id, s->orig_str)) {
                    return false;
                }
                int value = (int)s->game->sprites.sprites[sprt_id].angle;
                if (!int_to_sval(s, value)) {
                    return false;
                }
                p2 = strtok(NULL, " ");
            }
            return true;
        } else if (!strcmp(p1, "size") && p2) {
            // arr sprite size <sprt_id> -> {int array}
            char *p3 = strtok(NULL, " ");
            if (!p3) {
                int sprt_id = 0;
                if (!str_to_id(&s->game->sprites, p2, &sprt_id, s->orig_str)) {
                    return false;
                }
                int value =
                    (int)sprite_get_rect_field(s->game, sprt_id, RECT_W);
                if (!int_to_sval(s, value)) {
                    return false;
                }
                value = (int)sprite_get_rect_field(s->game, sprt_id, RECT_H);
                if (!int_to_sval(s, value)) {
                    return false;
                }
                return true;
            }
        } else if (!strcmp(p1, "rect") && p2) {
            // arr sprite rect <sprt_id> -> {int array}
            char *p3 = strtok(NULL, " ");
            if (!p3) {
                int sprt_id = 0;
                if (!str_to_id(&s->game->sprites, p2, &sprt_id, s->orig_str)) {
                    return false;
                }
                int value =
                    (int)sprite_get_rect_field(s->game, sprt_id, RECT_X);
                if (!int_to_sval(s, value)) {
                    return false;
                }
                value = (int)sprite_get_rect_field(s->game, sprt_id, RECT_Y);
                if (!int_to_sval(s, value)) {
                    return false;
                }
                value = (int)sprite_get_rect_field(s->game, sprt_id, RECT_W);
                if (!int_to_sval(s, value)) {
                    return false;
                }
                value = (int)sprite_get_rect_field(s->game, sprt_id, RECT_H);
                if (!int_to_sval(s, value)) {
                    return false;
                }
                value = (int)sprite_get_rect_field(s->game, sprt_id, RECT_R);
                if (!int_to_sval(s, value)) {
                    return false;
                }
                value = (int)sprite_get_rect_field(s->game, sprt_id, RECT_B);
                if (!int_to_sval(s, value)) {
                    return false;
                }
                value = (int)sprite_get_rect_field(s->game, sprt_id, RECT_CX);
                if (!int_to_sval(s, value)) {
                    return false;
                }
                value = (int)sprite_get_rect_field(s->game, sprt_id, RECT_CY);
                if (!int_to_sval(s, value)) {
                    return false;
                }
                return true;
            }
        } else if (p2) {
            // arr sprite
            // (x|l|left|y|t|top|w|width|h|height|r|right|b|bottom|cx|cy)
            // {sprt_id array} -> {int array}
            RectField field = 0;
            if (!str_to_field(p1, &field, s->orig_str)) {
                return false;
            }
            while (p2) {
                int sprt_id = 0;
                if (!str_to_id(&s->game->sprites, p2, &sprt_id, s->orig_str)) {
                    return false;
                }
                int value = (int)sprite_get_rect_field(s->game, sprt_id, field);
                if (!int_to_sval(s, value)) {
                    return false;
                }
                p2 = strtok(NULL, " ");
            }
            return true;
        }
    } else if (!strcmp(action, "free") && p1) {
        // free sprite {sprt_id array} -> {sprt_id array}
        s->msg.type = SEND_STRING;
        while (p1) {
            int sprt_id = 0;
            if (!str_to_id(&s->game->sprites, p1, &sprt_id, s->orig_str)) {
                return false;
            }
            sprite_free(s->game, sprt_id);
            if (!int_to_sval(s, -1)) {
                return false;
            }
            p1 = strtok(NULL, " ");
        }
        return true;
    }

    fprintf(stderr, "Error: \"%s\" is not correct.\n", s->orig_str);
    return false;
}
