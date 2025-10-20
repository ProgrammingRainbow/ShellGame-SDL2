#include "parse_sound.h"

bool parse_sound(SdlServer *s, char *action) {
    char *p1 = strtok(NULL, " ");
    if (!strcmp(action, "new") && p1) {
        // new sound <filename> -> <snd_id>
        char *p2 = strtok(NULL, " ");
        if (!p2) {
            int snd_id = 0;
            if (!sound_new(s->game, &snd_id, p1)) {
                return false;
            }
            s->msg.type = SEND_INT;
            s->msg.i_val = snd_id;
            return true;
        }
    } else if (!strcmp(action, "play") && p1) {
        // play sound {snd_id array}
        while (p1) {
            int snd_id = 0;
            if (!str_to_id(&s->game->sounds, p1, &snd_id, s->orig_str)) {
                return false;
            }
            sound_play(s->game, snd_id, 0);
            p1 = strtok(NULL, " ");
        }
        return true;
    } else if (!strcmp(action, "update")) {
        // update sound <filename> {snd_id array}
        char *p2 = strtok(NULL, " ");
        if (p2) {
            while (p2) {
                int snd_id = 0;
                if (!str_to_id(&s->game->sounds, p2, &snd_id, s->orig_str)) {
                    return false;
                }
                if (!sound_update(s->game, snd_id, p1)) {
                    return false;
                }
                p2 = strtok(NULL, " ");
            }
            return true;
        }
    } else if (!strcmp(action, "free") && p1) {
        // free sound {snd_id array} -> {snd_id array}
        s->msg.type = SEND_STRING;
        while (p1) {
            int snd_id = 0;
            if (!str_to_id(&s->game->sounds, p1, &snd_id, s->orig_str)) {
                return false;
            }
            sound_free(s->game, snd_id);
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
