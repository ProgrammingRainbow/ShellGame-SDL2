#include "parse_text.h"

bool parse_text(SdlServer *s, char *action) {
    char *p1 = strtok(NULL, " ");
    if (!strcmp(action, "new")) {
        // new text <font> <size> <string> -> <txt_id>
        char *p2 = strtok(NULL, " ");
        char *p3 = strtok(NULL, " ");
        if (p3) {
            int size = 0;
            if (!str_to_i(p2, &size, s->orig_str)) {
                return false;
            }
            char *str = (p3 - action) + s->orig_str;
            int txt_id = 0;
            if (!text_new(s->game, &txt_id, p1, size, str)) {
                return false;
            }
            s->msg.type = SEND_INT;
            s->msg.i_val = txt_id;
            return true;
        }
    } else if (!strcmp(action, "draw") && p1) {
        // draw text {txt_id array}
        while (p1) {
            int txt_id = 0;
            if (!str_to_id(&s->game->texts, p1, &txt_id, s->orig_str)) {
                return false;
            }
            text_draw(s->game, txt_id);
            p1 = strtok(NULL, " ");
        }
        return true;
    } else if (!strcmp(action, "update")) {
        char *p2 = strtok(NULL, " ");
        char *p3 = strtok(NULL, " ");
        if (!strcmp(p1, "pos")) {
            // update text pos <x_vel> <y_vel> {txt_id array}
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
                    int txt_id = 0;
                    if (!str_to_id(&s->game->texts, p4, &txt_id, s->orig_str)) {
                        return false;
                    }
                    text_update_pos(s->game, txt_id, x_vel, y_vel);
                    p4 = strtok(NULL, " ");
                }
                return true;
            }
        } else if (!strcmp(p1, "angle") && p3) {
            // update text angle <angle> {txt_id array}
            float angle = 0;
            if (!str_to_f(p2, &angle, s->orig_str)) {
                return false;
            }
            while (p3) {
                int txt_id = 0;
                if (!str_to_id(&s->game->texts, p3, &txt_id, s->orig_str)) {
                    return false;
                }
                text_update_angle(s->game, txt_id, angle);
                p3 = strtok(NULL, " ");
            }
            return true;
        } else if (p3) {
            // update text (x|y) <float> {rect_id array}
            RectField field = -1;
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
                    int txt_id = 0;
                    if (!str_to_id(&s->game->texts, p3, &txt_id, s->orig_str)) {
                        return false;
                    }
                    if (field == RECT_X) {
                        text_update_pos(s->game, txt_id, value, 0);
                    } else {
                        text_update_pos(s->game, txt_id, 0, value);
                    }
                    p3 = strtok(NULL, " ");
                }
                return true;
            }
        }

    } else if (!strcmp(action, "set") && p1) {
        char *p2 = strtok(NULL, " ");
        char *p3 = strtok(NULL, " ");
        if (!strcmp(p1, "string") && p3) {
            // set text string <txt_id> <string>
            int txt_id = 0;
            if (!str_to_id(&s->game->texts, p2, &txt_id, s->orig_str)) {
                return false;
            }
            char *str = (p3 - action) + s->orig_str;
            return text_set_str(s->game, txt_id, str);
        } else if (!strcmp(p1, "font")) {
            // set text font <txt_id> <font> <size>
            char *p4 = strtok(NULL, " ");
            char *p5 = strtok(NULL, " ");
            if (p4 && !p5) {
                int txt_id = 0;
                if (!str_to_id(&s->game->texts, p2, &txt_id, s->orig_str)) {
                    return false;
                }
                int size = 0;
                if (!str_to_i(p4, &size, s->orig_str)) {
                    return false;
                }
                return text_set_font(s->game, txt_id, p3, size);
            }
        } else if (!strcmp(p1, "color")) {
            char *p4 = strtok(NULL, " ");
            char *p5 = strtok(NULL, " ");
            char *p6 = strtok(NULL, " ");
            char *p7 = strtok(NULL, " ");
            int txt_id = 0;
            if (!str_to_id(&s->game->texts, p2, &txt_id, s->orig_str)) {
                return false;
            }
            Uint8 r = 0;
            if (!str_to_u8(p3, &r, s->orig_str)) {
                return false;
            }
            Uint8 g = 0;
            if (!str_to_u8(p4, &g, s->orig_str)) {
                return false;
            }
            Uint8 b = 0;
            if (!str_to_u8(p5, &b, s->orig_str)) {
                return false;
            }
            if (p5 && !p6) {
                // set text color <txt_id> <r> <g> <b>
                text_set_color(s->game, txt_id, r, g, b, 255);
                return true;
            } else if (p6 && !p7) {
                // set text color <txt_id> <r> <g> <b> <a>
                Uint8 a = 0;
                if (!str_to_u8(p6, &a, s->orig_str)) {
                    return false;
                }
                text_set_color(s->game, txt_id, r, g, b, a);
                return true;
            }
        } else if (!strcmp(p1, "pos") && p3) {
            // set text pos <x> <y> {txt_id array}
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
                    int txt_id = 0;
                    if (!str_to_id(&s->game->texts, p4, &txt_id, s->orig_str)) {
                        return false;
                    }
                    text_set_pos(s->game, txt_id, x_pos, y_pos);
                    p4 = strtok(NULL, " ");
                }
                return true;
            }
        } else if (!strcmp(p1, "angle") && p3) {
            // set text angle <angle> {txt_id array}
            float angle = 0;
            if (!str_to_f(p2, &angle, s->orig_str)) {
                return false;
            }
            while (p3) {
                int txt_id = 0;
                if (!str_to_id(&s->game->texts, p3, &txt_id, s->orig_str)) {
                    return false;
                }
                s->game->texts.texts[txt_id].angle = angle;
                p3 = strtok(NULL, " ");
            }
            return true;
        } else if (!strcmp(p1, "flip") && p3) {
            // set text flip <flip> {txt_id array}
            SDL_RendererFlip flip = 0;
            if (!str_to_flip(p2, &flip, s->orig_str)) {
                return false;
            }
            while (p3) {
                int txt_id = 0;
                if (!str_to_id(&s->game->texts, p3, &txt_id, s->orig_str)) {
                    return false;
                }
                s->game->texts.texts[txt_id].flip = flip;
                p3 = strtok(NULL, " ");
            }
            return true;
        } else if (!strcmp(p1, "size") && p3) {
            // set text size <width> <height> {txt_id array}
            float width = 0;
            if (!str_to_f(p2, &width, s->orig_str)) {
                return false;
            }
            float height = 0;
            if (!str_to_f(p3, &height, s->orig_str)) {
                return false;
            }
            char *p4 = strtok(NULL, " ");
            if (p4) {
                while (p4) {
                    int txt_id = 0;
                    if (!str_to_id(&s->game->texts, p4, &txt_id, s->orig_str)) {
                        return false;
                    }
                    text_set_size(s->game, txt_id, width, height);
                    p4 = strtok(NULL, " ");
                }
                return true;
            }
        } else if (p3) {
            // set text
            // (x|l|left|y|t|top|w|width|h|height|r|right|b|bottom|cx|cy)
            // <float> {txt_id array}
            RectField field = 0;
            if (!str_to_field(p1, &field, s->orig_str)) {
                return false;
            }
            float value = 0;
            if (!str_to_f(p2, &value, s->orig_str)) {
                return false;
            }
            while (p3) {
                int txt_id = 0;
                if (!str_to_id(&s->game->texts, p3, &txt_id, s->orig_str)) {
                    return false;
                }
                text_set_rect_field(s->game, txt_id, field, value);
                p3 = strtok(NULL, " ");
            }
            return true;
        }
    } else if (!strcmp(action, "get")) {
        char *p2 = strtok(NULL, " ");
        char *p3 = strtok(NULL, " ");
        if (!strcmp(p1, "angle") && p2 && !p3) {
            // get text angle <txt_id> -> <int>
            int txt_id = 0;
            if (!str_to_id(&s->game->texts, p2, &txt_id, s->orig_str)) {
                return false;
            }
            s->msg.type = SEND_INT;
            s->msg.i_val = (int)s->game->texts.texts[txt_id].angle;
            return true;
        } else if (p2 && !p3) {
            // get text
            // (x|l|left|y|t|top|w|width|h|height|r|right|b|bottom|cx|cy)
            // <txt_id> -> <int>
            RectField field = 0;
            if (!str_to_field(p1, &field, s->orig_str)) {
                return false;
            }
            int txt_id = 0;
            if (!str_to_id(&s->game->texts, p2, &txt_id, s->orig_str)) {
                return false;
            }
            s->msg.type = SEND_INT;
            s->msg.i_val = text_get_rect_field(s->game, txt_id, field);
            return true;
        }
    } else if (!strcmp(action, "getf")) {
        char *p2 = strtok(NULL, " ");
        char *p3 = strtok(NULL, " ");
        if (!strcmp(p1, "angle") && p2 && !p3) {
            // getf text angle <txt_id> -> <float>
            int txt_id = 0;
            if (!str_to_id(&s->game->texts, p2, &txt_id, s->orig_str)) {
                return false;
            }
            s->msg.type = SEND_FLOAT;
            s->msg.f_val = s->game->texts.texts[txt_id].angle;
            return true;
        }
    } else if (!strcmp(action, "arr")) {
        s->msg.type = SEND_STRING;
        char *p2 = strtok(NULL, " ");
        if (!strcmp(p1, "pos") && p2) {
            // arr text pos <txt_id> -> {int array}
            char *p3 = strtok(NULL, " ");
            if (!p3) {
                int txt_id = 0;
                if (!str_to_id(&s->game->texts, p2, &txt_id, s->orig_str)) {
                    return false;
                }
                int value = (int)text_get_rect_field(s->game, txt_id, RECT_X);
                if (!int_to_sval(s, value)) {
                    return false;
                }
                value = (int)text_get_rect_field(s->game, txt_id, RECT_Y);
                if (!int_to_sval(s, value)) {
                    return false;
                }
                return true;
            }
        } else if (!strcmp(p1, "angle") && p2) {
            // arr text angle {txt_id array} -> {int array}
            while (p2) {
                int txt_id = 0;
                if (!str_to_id(&s->game->texts, p2, &txt_id, s->orig_str)) {
                    return false;
                }
                int value = (int)s->game->texts.texts[txt_id].angle;
                if (!int_to_sval(s, value)) {
                    return false;
                }
                p2 = strtok(NULL, " ");
            }
            return true;
        } else if (!strcmp(p1, "size") && p2) {
            // arr text size <txt_id> -> {int array}
            char *p3 = strtok(NULL, " ");
            if (!p3) {
                int txt_id = 0;
                if (!str_to_id(&s->game->texts, p2, &txt_id, s->orig_str)) {
                    return false;
                }
                int value = (int)text_get_rect_field(s->game, txt_id, RECT_W);
                if (!int_to_sval(s, value)) {
                    return false;
                }
                value = (int)text_get_rect_field(s->game, txt_id, RECT_H);
                if (!int_to_sval(s, value)) {
                    return false;
                }
                return true;
            }
        } else if (!strcmp(p1, "rect") && p2) {
            // arr text rect <txt_id> -> {int array}
            char *p3 = strtok(NULL, " ");
            if (!p3) {
                int txt_id = 0;
                if (!str_to_id(&s->game->texts, p2, &txt_id, s->orig_str)) {
                    return false;
                }
                int value = (int)text_get_rect_field(s->game, txt_id, RECT_X);
                if (!int_to_sval(s, value)) {
                    return false;
                }
                value = (int)text_get_rect_field(s->game, txt_id, RECT_Y);
                if (!int_to_sval(s, value)) {
                    return false;
                }
                value = (int)text_get_rect_field(s->game, txt_id, RECT_W);
                if (!int_to_sval(s, value)) {
                    return false;
                }
                value = (int)text_get_rect_field(s->game, txt_id, RECT_H);
                if (!int_to_sval(s, value)) {
                    return false;
                }
                value = (int)text_get_rect_field(s->game, txt_id, RECT_R);
                if (!int_to_sval(s, value)) {
                    return false;
                }
                value = (int)text_get_rect_field(s->game, txt_id, RECT_B);
                if (!int_to_sval(s, value)) {
                    return false;
                }
                value = (int)text_get_rect_field(s->game, txt_id, RECT_CX);
                if (!int_to_sval(s, value)) {
                    return false;
                }
                value = (int)text_get_rect_field(s->game, txt_id, RECT_CY);
                if (!int_to_sval(s, value)) {
                    return false;
                }
                return true;
            }
        } else if (p2) {
            // arr text
            // (x|l|left|y|t|top|w|width|h|height|r|right|b|bottom|cx|cy)
            // {txt_id array} -> {int array}
            RectField field = 0;
            if (!str_to_field(p1, &field, s->orig_str)) {
                return false;
            }
            while (p2) {
                int txt_id = 0;
                if (!str_to_id(&s->game->texts, p2, &txt_id, s->orig_str)) {
                    return false;
                }
                int value = (int)text_get_rect_field(s->game, txt_id, field);
                if (!int_to_sval(s, value)) {
                    return false;
                }
                p2 = strtok(NULL, " ");
            }
            return true;
        }
    } else if (!strcmp(action, "free") && p1) {
        // free text {txt_id array} -> {txt_id array}
        s->msg.type = SEND_STRING;
        while (p1) {
            int txt_id = 0;
            if (!str_to_id(&s->game->texts, p1, &txt_id, s->orig_str)) {
                return false;
            }
            text_free(s->game, txt_id);
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
