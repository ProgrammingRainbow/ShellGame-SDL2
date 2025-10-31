#include "parse_music.h"

bool parse_music(SdlServer *s, char *action) {
    char *p1 = strtok(NULL, " ");
    if (!strcmp(action, "new") && p1) {
        // new music <filename> -> <msc_id>
        char *p2 = strtok(NULL, " ");
        if (!p2) {
            int msc_id = 0;
            if (!music_new(s->game, &msc_id, p1)) {
                return false;
            }
            s->msg.type = SEND_INT;
            s->msg.i_val = msc_id;
            return true;
        }
    } else if (!strcmp(action, "play") && p1) {
        // play music <msc_id>
        char *p2 = strtok(NULL, " ");
        if (!p2) {
            int msc_id = 0;
            if (!str_to_id(&s->game->music, p1, &msc_id, s->orig_str)) {
                return false;
            }
            if (!music_play(s->game, msc_id, -1)) {
                return false;
            }
            p1 = strtok(NULL, " ");
        }
        return true;
    } else if (!strcmp(action, "set") && p1) {
        char *p2 = strtok(NULL, " ");
        if (!p2) {
            if (!strcmp(p1, "pause")) {
                // set music pause
                Mix_PauseMusic();
                return true;
            } else if (!strcmp(p1, "resume")) {
                // set music resume
                Mix_ResumeMusic();
                return true;
            } else if (!strcmp(p1, "halt")) {
                // set music halt
                Mix_HaltMusic();
                return true;
            }
        } else {
            // set music <filename> {msc_id array}
            while (p2) {
                int msc_id = 0;
                if (!str_to_id(&s->game->music, p2, &msc_id, s->orig_str)) {
                    return false;
                }
                if (!music_update(s->game, msc_id, p1)) {
                    return false;
                }
                p2 = strtok(NULL, " ");
            }
            return true;
        }
    } else if (!strcmp(action, "get") && p1) {
        char *p2 = strtok(NULL, " ");
        if (!p2) {
            s->msg.type = SEND_INT;
            if (!strcmp(p1, "paused")) {
                // get music paused -> <bool>
                bool value = Mix_PausedMusic() ? true : false;
                s->msg.i_val = value;
                return true;
            } else if (!strcmp(p1, "playing")) {
                // get music playing -> <bool>
                bool value = Mix_PlayingMusic() ? true : false;
                s->msg.i_val = value;
                return true;
            }
        }
    } else if (!strcmp(action, "free") && p1) {
        // free music {msc_id array} -> {msc_id array}
        s->msg.type = SEND_STRING;
        while (p1) {
            int msc_id = 0;
            if (!str_to_id(&s->game->music, p1, &msc_id, s->orig_str)) {
                return false;
            }
            music_free(s->game, msc_id);
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
