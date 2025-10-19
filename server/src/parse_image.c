#include "parse_image.h"

bool parse_image(SdlServer *s, char *action) {
    char *p1 = strtok(NULL, " ");
    if (!strcmp(action, "new") && p1) {
        // new image <filename> -> <img_id>
        char *p2 = strtok(NULL, " ");
        if (!p2) {
            int img_id = 0;
            if (!image_new(s->game, &img_id, p1)) {
                return false;
            }
            s->msg.type = SEND_INT;
            s->msg.i_val = img_id;
            return true;
        }
    } else if (!strcmp(action, "draw")) {
        // draw image <src_id> <dest_id> {img_id array}
        char *p2 = strtok(NULL, " ");
        char *p3 = strtok(NULL, " ");
        if (p3) {
            int src_id = 0;
            if (!strcmp(p1, "NULL")) {
                src_id = -1;
            } else {
                if (!str_to_id(&s->game->rects, p1, &src_id, s->orig_str)) {
                    return false;
                }
            }
            int dest_id = 0;
            if (!str_to_id(&s->game->rects, p2, &dest_id, s->orig_str)) {
                return false;
            }
            while (p3) {
                int img_id = 0;
                if (!str_to_id(&s->game->images, p3, &img_id, s->orig_str)) {
                    return false;
                }
                image_draw(s->game, img_id, src_id, dest_id);
                p3 = strtok(NULL, " ");
            }
            return true;
        }
    } else if (!strcmp(action, "update")) {
        // update image <filename> {img_id array}
        char *p2 = strtok(NULL, " ");
        if (p2) {
            while (p2) {
                int img_id = 0;
                if (!str_to_id(&s->game->images, p2, &img_id, s->orig_str)) {
                    return false;
                }
                if (!image_update_image(s->game, img_id, p1)) {
                    return false;
                }
                p2 = strtok(NULL, " ");
            }
            return true;
        }
    } else if (!strcmp(action, "get")) {
        // get image (w|width|h|height) <img_id> -> <int>
        char *p2 = strtok(NULL, " ");
        char *p3 = strtok(NULL, " ");
        if (p2 && !p3) {
            RectField field = 0;
            if (!str_to_pos(p1, &field, s->orig_str)) {
                return false;
            }
            int img_id = 0;
            if (!str_to_id(&s->game->images, p2, &img_id, s->orig_str)) {
                return false;
            }
            int w = 0;
            int h = 0;
            if (!image_get_size(s->game, img_id, &w, &h)) {
                return false;
            }
            s->msg.type = SEND_INT;
            if (field == RECT_W) {
                s->msg.i_val = w;
            } else {
                s->msg.i_val = h;
            }
            return true;
        }
    } else if (!strcmp(action, "arr")) {
        s->msg.type = SEND_STRING;
        char *p2 = strtok(NULL, " ");
        if (!strcmp(p1, "size") && p2) {
            // arr image size <img_id> -> {int array}
            char *p3 = strtok(NULL, " ");
            if (!p3) {
                int img_id = 0;
                if (!str_to_id(&s->game->images, p2, &img_id, s->orig_str)) {
                    return false;
                }
                int w = 0;
                int h = 0;
                if (!image_get_size(s->game, img_id, &w, &h)) {
                    return false;
                }
                if (!int_to_sval(s, w)) {
                    return false;
                }
                if (!int_to_sval(s, h)) {
                    return false;
                }
                return true;
            }
        } else if (p2) {
            // arr image (w|width|h|height) {img_id array} -> {int array}
            RectField field = 0;
            if (!str_to_pos(p1, &field, s->orig_str)) {
                return false;
            }
            while (p2) {
                int img_id = 0;
                if (!str_to_id(&s->game->images, p2, &img_id, s->orig_str)) {
                    return false;
                }
                int w = 0;
                int h = 0;
                if (!image_get_size(s->game, img_id, &w, &h)) {
                    return false;
                }
                if (field == RECT_W) {
                    if (!int_to_sval(s, w)) {
                        return false;
                    }
                } else {
                    if (!int_to_sval(s, h)) {
                        return false;
                    }
                }
            }
            return true;
        }
    } else if (!strcmp(action, "free") && p1) {
        // free image {img_id array} -> {img_id array}
        s->msg.type = SEND_STRING;
        while (p1) {
            int img_id = 0;
            if (!str_to_id(&s->game->images, p1, &img_id, s->orig_str)) {
                return false;
            }
            image_free(s->game, img_id);
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
