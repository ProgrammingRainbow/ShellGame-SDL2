#include "pipe.h"
#include "parser.h"

#include <signal.h>
#include <sys/stat.h>
#include <unistd.h>

void pipe_server_free(PipeServer *p);
PipeServer *pipe_server_new(void);
void pipe_server_send(PipeServer *p);

void pipe_server_free(PipeServer *p) {
    if (p) {
        if (p->server.game) {
            game_free(p->server.game);
            p->server.game = NULL;
        }

        // Close input stream if it was opened.
        if (p->in_stream) {
            fclose(p->in_stream);
            p->in_stream = NULL;
        }

        // Close output stream if it was opened.
        if (p->out_stream) {
            fclose(p->out_stream);
            p->out_stream = NULL;
        }

        // Remove the named pipes from the filesystem.
        unlink(PIPE_TO_SERVER_PATH);
        unlink(PIPE_FROM_SERVER_PATH);

        memset(p->server.orig_str, 0, BUFFER_SIZE);
        memset(p->server.token_str, 0, BUFFER_SIZE);

        free(p);

        printf("[SERVER] Pipe Server Shutdown.\n");
    }
}

PipeServer *pipe_server_new(void) {
    PipeServer *p = calloc(1, sizeof(PipeServer));
    if (p == NULL) {
        fprintf(stderr, "Error in Calloc of new Server.");
        return NULL;
    }

    // Create named pipe for input.
    mkfifo(PIPE_TO_SERVER_PATH, 0666);

    // Open input pipe for reading.
    p->in_stream = fopen(PIPE_TO_SERVER_PATH, "r");
    if (p->in_stream == NULL) {
        perror("[SERVER] Error: Opening Named Pipe in_stream.");
        pipe_server_free(p);
        return NULL;
    }

    // Create named pipe for output.
    mkfifo(PIPE_FROM_SERVER_PATH, 0666);

    // Open output pipe for reading.
    p->out_stream = fopen(PIPE_FROM_SERVER_PATH, "w");
    if (p->out_stream == NULL) {
        perror("[SERVER] Error: Opening Named Pipe out_stream.");
        pipe_server_free(p);
        return NULL;
    }

    p->server.game = game_new();
    if (p->server.game == NULL) {
        pipe_server_free(p);
        return NULL;
    }

    p->server.running = true;

    return p;
}

void pipe_server_send(PipeServer *p) {
    if (p->server.msg.enable) {
        switch (p->server.msg.type) {
        case SEND_NONE:
            fprintf(p->out_stream, "NULL\n");
            break;
        case SEND_BOOL:
            fprintf(p->out_stream, "%i\n", p->server.msg.b_val);
            break;
        case SEND_INT:
            fprintf(p->out_stream, "%i\n", p->server.msg.i_val);
            break;
        case SEND_FLOAT:
            fprintf(p->out_stream, "%f\n", p->server.msg.f_val);
            break;
        case SEND_STRING:
            fprintf(p->out_stream, "%s\n", p->server.msg.s_val);
            break;
        default:
            fprintf(p->out_stream, "%i\n", p->server.msg.i_val);
            break;
        }

        p->server.msg.enable = false;
        p->server.msg.type = SEND_NONE;
        p->server.msg.i_val = 0;

        memset(p->server.msg.s_val, 0, BUFFER_SIZE);
        fflush(p->out_stream);
    }
}

bool pipe_server_run(pid_t script_pid) {
    printf("[SERVER] Starting pipe server.\n");
    PipeServer *p = pipe_server_new();
    if (p == NULL) {
        return false;
    }

    printf("[SERVER] Listening on pipe.\n");

    while (fgets(p->server.orig_str, BUFFER_SIZE, p->in_stream) &&
           p->server.running) {
        size_t len = strlen(p->server.orig_str);
        if (len > 0 && p->server.orig_str[len - 1] == '\n') {
            p->server.orig_str[len - 1] = '\0';
        }

        memcpy(p->server.token_str, p->server.orig_str, BUFFER_SIZE);

        if (!parse_line(&p->server)) {
            pipe_server_send(p);
            pipe_server_free(p);
            if (kill(script_pid, 0) == 0) {
                kill(script_pid, SIGUSR2);
            }
            return false;
        }

        pipe_server_send(p);
    }

    pipe_server_free(p);
    if (kill(script_pid, 0) == 0) {
        kill(script_pid, SIGUSR1);
    }

    return true;
}
