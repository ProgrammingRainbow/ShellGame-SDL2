#ifndef STDINOUT_H
#define STDINOUT_H

#include "game.h"

typedef struct {
        SdlServer server;
} StdInOutServer;

bool stdinout_server_run(pid_t script_pid);

#endif
