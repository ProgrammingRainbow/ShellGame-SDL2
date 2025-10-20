#ifndef PIPE_H
#define PIPE_H

#define PIPE_TO_SERVER_PATH "/tmp/sg_pipe_to_server"
#define PIPE_FROM_SERVER_PATH "/tmp/sg_pipe_from_server"

#include "game.h"

typedef struct {
        FILE *in_stream;
        FILE *out_stream;
        SdlServer server;
} PipeServer;

bool pipe_server_run(pid_t script_pid);

#endif
