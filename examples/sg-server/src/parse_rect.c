#include "parse_rect.h"

bool parse_rect(SdlServer *s, char *action) {
    char *p1 = strtok(NULL, " ");
    if (!strcmp(action, "new")) {
        if (!p1) {
            // new rect -> <rect_id>
            int rect_id = 0;
            if (!rect_new(s->game, &rect_id, 0, 0, 0, 0)) {
                return false;
            }
            s->msg.type = SEND_INT;
            s->msg.i_val = rect_id;
            return true;
        } else if (!strcmp(p1, "image")) {
            // new rect image <img_id> -> <rect_id>
            char *p2 = strtok(NULL, " ");
            char *p3 = strtok(NULL, " ");
            if (p2 && !p3) {
                int img_id = 0;
                if (!str_to_id(&s->game->images, p2, &img_id, s->orig_str)) {
                    return false;
                }
                int rect_id = 0;
                if (!rect_from_image(s->game, &rect_id, img_id)) {
                    return false;
                }
                s->msg.type = SEND_INT;
                s->msg.i_val = rect_id;
                return true;
            }
        } else {
            // new rect <x> <y> <w> <h> -> <rect_id>
            char *p2 = strtok(NULL, " ");
            char *p3 = strtok(NULL, " ");
            char *p4 = strtok(NULL, " ");
            char *p5 = strtok(NULL, " ");
            if (p4 && !p5) {
                int x = 0;
                if (!str_to_i(p1, &x, s->orig_str)) {
                    return false;
                }
                int y = 0;
                if (!str_to_i(p2, &y, s->orig_str)) {
                    return false;
                }
                int w = 0;
                if (!str_to_i(p3, &w, s->orig_str)) {
                    return false;
                }
                int h = 0;
                if (!str_to_i(p4, &h, s->orig_str)) {
                    return false;
                }
                int rect_id = 0;
                if (!rect_new(s->game, &rect_id, x, y, w, h)) {
                    return false;
                }
                s->msg.type = SEND_INT;
                s->msg.i_val = rect_id;
                return true;
            }
        }
    } else if (!strcmp(action, "update")) {
        char *p2 = strtok(NULL, " ");
        char *p3 = strtok(NULL, " ");
        char *p4 = strtok(NULL, " ");
        if (!strcmp(p1, "pos") && p4) {
            // update rect pos <x_vel> <y_vel> {rect_id array}
            float x_vel = 0;
            if (!str_to_f(p2, &x_vel, s->orig_str)) {
                return false;
            }
            float y_vel = 0;
            if (!str_to_f(p3, &y_vel, s->orig_str)) {
                return false;
            }
            while (p4) {
                int rect_id = 0;
                if (!str_to_id(&s->game->rects, p4, &rect_id, s->orig_str)) {
                    return false;
                }
                rect_update_pos(s->game, rect_id, x_vel, y_vel);
                p4 = strtok(NULL, " ");
            }
            return true;
        } else if (!strcmp(p1, "angle") && p3) {
            // update rect angle <angle> {rect_id array}
            float angle = 0;
            if (!str_to_f(p2, &angle, s->orig_str)) {
                return false;
            }
            while (p3) {
                int rect_id = 0;
                if (!str_to_id(&s->game->rects, p3, &rect_id, s->orig_str)) {
                    return false;
                }
                rect_update_angle(s->game, rect_id, angle);
                p3 = strtok(NULL, " ");
            }
            return true;
        } else if (p3) {
            // update rect (x|y) <float> {rect_id array}
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
                    int rect_id = 0;
                    if (!str_to_id(&s->game->rects, p3, &rect_id,
                                   s->orig_str)) {
                        return false;
                    }
                    if (field == RECT_X) {
                        rect_update_pos(s->game, rect_id, value, 0);
                    } else {
                        rect_update_pos(s->game, rect_id, 0, value);
                    }
                    p3 = strtok(NULL, " ");
                }
                return true;
            }
        }
    } else if (!strcmp(action, "set") && p1) {
        char *p2 = strtok(NULL, " ");
        char *p3 = strtok(NULL, " ");
        if (!strcmp(p1, "pos") && p3) {
            // set rect pos <x> <y> {rect_id array}
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
                    int rect_id = 0;
                    if (!str_to_id(&s->game->rects, p4, &rect_id,
                                   s->orig_str)) {
                        return false;
                    }
                    rect_set_pos(s->game, rect_id, x_pos, y_pos);
                    p4 = strtok(NULL, " ");
                }
                return true;
            }
        } else if (!strcmp(p1, "angle") && p3) {
            // set rect angle <angle> {rect_id array}
            float angle = 0;
            if (!str_to_f(p2, &angle, s->orig_str)) {
                return false;
            }
            while (p3) {
                int rect_id = 0;
                if (!str_to_id(&s->game->rects, p3, &rect_id, s->orig_str)) {
                    return false;
                }
                s->game->rects.rects[rect_id].angle = angle;
                p3 = strtok(NULL, " ");
            }
            return true;
        } else if (!strcmp(p1, "flip") && p3) {
            // set rect flip <flip> {rect_id array}
            SDL_RendererFlip flip = 0;
            if (!str_to_flip(p2, &flip, s->orig_str)) {
                return false;
            }
            while (p3) {
                int rect_id = 0;
                if (!str_to_id(&s->game->rects, p3, &rect_id, s->orig_str)) {
                    return false;
                }
                s->game->rects.rects[rect_id].flip = flip;
                p3 = strtok(NULL, " ");
            }
            return true;
        } else if (!strcmp(p1, "size") && p3) {
            // set rect size <width> <height> {rect_id array}
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
                    int rect_id = 0;
                    if (!str_to_id(&s->game->rects, p4, &rect_id,
                                   s->orig_str)) {
                        return false;
                    }
                    rect_set_size(s->game, rect_id, width, height);
                    p4 = strtok(NULL, " ");
                }
                return true;
            }
        } else if (p3) {
            // set rect
            // (x|l|left|y|t|top|w|width|h|height|r|right|b|bottom|cx|cy)
            // <float> {rect_id array}
            RectField field = 0;
            if (!str_to_field(p1, &field, s->orig_str)) {
                return false;
            }
            float value = 0;
            if (!str_to_f(p2, &value, s->orig_str)) {
                return false;
            }
            while (p3) {
                int rect_id = 0;
                if (!str_to_id(&s->game->rects, p3, &rect_id, s->orig_str)) {
                    return false;
                }
                rect_set_rect_field(s->game, rect_id, field, value);
                p3 = strtok(NULL, " ");
            }
            return true;
        }
    } else if (!strcmp(action, "get")) {
        char *p2 = strtok(NULL, " ");
        char *p3 = strtok(NULL, " ");
        if (!strcmp(p1, "angle") && p2 && !p3) {
            // get rect angle <rect_id> -> <int>
            int rect_id = 0;
            if (!str_to_id(&s->game->rects, p2, &rect_id, s->orig_str)) {
                return false;
            }
            s->msg.type = SEND_INT;
            s->msg.i_val = (int)s->game->rects.rects[rect_id].angle;
            return true;
        } else if (p2 && !p3) {
            // get rect
            // (x|l|left|y|t|top|w|width|h|height|r|right|b|bottom|cx|cy)
            // <rect_id> -> <int>
            RectField field = 0;
            if (!str_to_field(p1, &field, s->orig_str)) {
                return false;
            }
            int rect_id = 0;
            if (!str_to_id(&s->game->rects, p2, &rect_id, s->orig_str)) {
                return false;
            }
            s->msg.type = SEND_INT;
            s->msg.i_val = rect_get_rect_field(s->game, rect_id, field);
            return true;
        }
    } else if (!strcmp(action, "getf")) {
        char *p2 = strtok(NULL, " ");
        char *p3 = strtok(NULL, " ");
        if (!strcmp(p1, "angle") && p2 && !p3) {
            // getf rect angle <rect_id> -> <float>
            int rect_id = 0;
            if (!str_to_id(&s->game->rects, p2, &rect_id, s->orig_str)) {
                return false;
            }
            s->msg.type = SEND_FLOAT;
            s->msg.f_val = s->game->rects.rects[rect_id].angle;
            return true;
        }
    } else if (!strcmp(action, "arr")) {
        s->msg.type = SEND_STRING;
        char *p2 = strtok(NULL, " ");
        if (!strcmp(p1, "pos") && p2) {
            // arr rect pos <rect_id> -> {int array}
            char *p3 = strtok(NULL, " ");
            if (!p3) {
                int rect_id = 0;
                if (!str_to_id(&s->game->rects, p2, &rect_id, s->orig_str)) {
                    return false;
                }
                int value = (int)rect_get_rect_field(s->game, rect_id, RECT_X);
                if (!int_to_sval(s, value)) {
                    return false;
                }
                value = (int)rect_get_rect_field(s->game, rect_id, RECT_Y);
                if (!int_to_sval(s, value)) {
                    return false;
                }
                return true;
            }
        } else if (!strcmp(p1, "angle") && p2) {
            // arr rect angle {rect_id array} -> {int array}
            while (p2) {
                int rect_id = 0;
                if (!str_to_id(&s->game->rects, p2, &rect_id, s->orig_str)) {
                    return false;
                }
                int value = (int)s->game->rects.rects[rect_id].angle;
                if (!int_to_sval(s, value)) {
                    return false;
                }
                p2 = strtok(NULL, " ");
            }
            return true;
        } else if (!strcmp(p1, "size") && p2) {
            // arr rect size <rect_id> -> {int array}
            char *p3 = strtok(NULL, " ");
            if (!p3) {
                int sprt_id = 0;
                if (!str_to_id(&s->game->rects, p2, &sprt_id, s->orig_str)) {
                    return false;
                }
                int value = (int)rect_get_rect_field(s->game, sprt_id, RECT_W);
                if (!int_to_sval(s, value)) {
                    return false;
                }
                value = (int)rect_get_rect_field(s->game, sprt_id, RECT_H);
                if (!int_to_sval(s, value)) {
                    return false;
                }
                return true;
            }
        } else if (!strcmp(p1, "rect") && p2) {
            // arr rect rect <rect_id> -> {int array}
            char *p3 = strtok(NULL, " ");
            if (!p3) {
                int rect_id = 0;
                if (!str_to_id(&s->game->rects, p2, &rect_id, s->orig_str)) {
                    return false;
                }
                int value = (int)rect_get_rect_field(s->game, rect_id, RECT_X);
                if (!int_to_sval(s, value)) {
                    return false;
                }
                value = (int)rect_get_rect_field(s->game, rect_id, RECT_Y);
                if (!int_to_sval(s, value)) {
                    return false;
                }
                value = (int)rect_get_rect_field(s->game, rect_id, RECT_W);
                if (!int_to_sval(s, value)) {
                    return false;
                }
                value = (int)rect_get_rect_field(s->game, rect_id, RECT_H);
                if (!int_to_sval(s, value)) {
                    return false;
                }
                value = (int)rect_get_rect_field(s->game, rect_id, RECT_R);
                if (!int_to_sval(s, value)) {
                    return false;
                }
                value = (int)rect_get_rect_field(s->game, rect_id, RECT_B);
                if (!int_to_sval(s, value)) {
                    return false;
                }
                value = (int)rect_get_rect_field(s->game, rect_id, RECT_CX);
                if (!int_to_sval(s, value)) {
                    return false;
                }
                value = (int)rect_get_rect_field(s->game, rect_id, RECT_CY);
                if (!int_to_sval(s, value)) {
                    return false;
                }
                return true;
            }
        } else if (p2) {
            // arr rect
            // (x|l|left|y|t|top|w|width|h|height|r|right|b|bottom|cx|cy)
            // {rect_id array} -> {int array}
            RectField field = 0;
            if (!str_to_field(p1, &field, s->orig_str)) {
                return false;
            }
            while (p2) {
                int rect_id = 0;
                if (!str_to_id(&s->game->rects, p2, &rect_id, s->orig_str)) {
                    return false;
                }
                int value = (int)rect_get_rect_field(s->game, rect_id, field);
                if (!int_to_sval(s, value)) {
                    return false;
                }
                p2 = strtok(NULL, " ");
            }
            return true;
        }
    } else if (!strcmp(action, "free") && p1) {
        // free rect {rect_id array} -> {rect_id array}
        s->msg.type = SEND_STRING;
        while (p1) {
            int rect_id = 0;
            if (!str_to_id(&s->game->rects, p1, &rect_id, s->orig_str)) {
                return false;
            }
            rect_free(s->game, rect_id);
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
