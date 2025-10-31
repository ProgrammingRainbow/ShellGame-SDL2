#include "stdinout.h"
#include "parser.h"

#include <signal.h>

void stdinout_server_free(StdInOutServer *s);
StdInOutServer *stdinout_server_new(void);
void stdinout_server_send(StdInOutServer *s);

void stdinout_server_free(StdInOutServer *s) {
    if (s) {
        if (s->server.game) {
            game_free(s->server.game);
            s->server.game = NULL;
        }

        free(s);

        fprintf(stderr, "[SERVER] Stdin Server Shutdown.\n");
    }
}

StdInOutServer *stdinout_server_new(void) {
    StdInOutServer *s = calloc(1, sizeof(StdInOutServer));
    if (s == NULL) {
        fprintf(stderr, "Error in Calloc of new Server.");
        return NULL;
    }

    s->server.game = game_new();
    if (s->server.game == NULL) {
        stdinout_server_free(s);
        return NULL;
    }

    s->server.running = true;

    return s;
}

void stdinout_server_send(StdInOutServer *s) {
    if (s->server.msg.enable) {
        switch (s->server.msg.type) {
        case SEND_NONE:
            fprintf(stdout, "NULL\n");
            break;
        case SEND_BOOL:
            fprintf(stdout, "%i\n", s->server.msg.b_val);
            break;
        case SEND_INT:
            fprintf(stdout, "%i\n", s->server.msg.i_val);
            break;
        case SEND_FLOAT:
            fprintf(stdout, "%f\n", s->server.msg.f_val);
            break;
        case SEND_STRING:
            fprintf(stdout, "%s\n", s->server.msg.s_val);
            break;
        default:
            fprintf(stdout, "%i\n", s->server.msg.i_val);
            break;
        }

        s->server.msg.enable = false;
        s->server.msg.type = SEND_NONE;
        s->server.msg.i_val = 0;

        memset(s->server.msg.s_val, 0, BUFFER_SIZE);
        fflush(stdout);
    }
}

bool stdinout_server_run(pid_t script_pid) {
    fprintf(stderr, "[SERVER] Starting stdin server.\n");
    StdInOutServer *s = stdinout_server_new();
    if (s == NULL) {
        return false;
    }

    fprintf(stderr, "[SERVER] Listening on stdin.\n");

    while (fgets(s->server.orig_str, BUFFER_SIZE, stdin) && s->server.running) {
        size_t len = strlen(s->server.orig_str);
        if (len > 0 && s->server.orig_str[len - 1] == '\n') {
            s->server.orig_str[len - 1] = '\0';
        }

        memcpy(s->server.token_str, s->server.orig_str, BUFFER_SIZE);

        if (!parse_line(&s->server)) {
            stdinout_server_send(s);
            stdinout_server_free(s);
            if (kill(script_pid, 0) == 0) {
                kill(script_pid, SIGUSR2);
            }
            return false;
        }

        stdinout_server_send(s);
    }

    stdinout_server_free(s);
    if (kill(script_pid, 0) == 0) {
        kill(script_pid, SIGUSR1);
    }

    return true;
}
