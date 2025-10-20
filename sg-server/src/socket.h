#ifndef SOCKET_H
#define SOCKET_H

#define SOCKET_PATH "/tmp/sg_socket"

#include "game.h"
#include <sys/un.h>

typedef struct {
        int fd_server;
        int fd_client;
        FILE *stream;
        struct sockaddr_un addr;
        SdlServer server;
} SocketServer;

bool socket_server_run(pid_t script_pid);

#endif
