#include "socket.h"
#include "parser.h"

#include <signal.h>
#include <sys/socket.h>
#include <unistd.h>

void socket_server_free(SocketServer *server);
SocketServer *socket_server_new(void);
void socket_server_send(SocketServer *s);

void socket_server_free(SocketServer *s) {
    if (s) {
        if (s->server.game) {
            game_free(s->server.game);
            s->server.game = NULL;
        }

        // Close stream wrapper (also closes the client socket).
        if (s->stream != NULL) {
            fclose(s->stream);
            s->stream = NULL;
            s->fd_client = -1;
        }

        // Close the client socket.
        if (s->fd_client >= 0) {
            close(s->fd_client);
            s->fd_client = -1;
        }

        // Close the server socket.
        if (s->fd_server >= 0) {
            close(s->fd_server);
            s->fd_server = -1;
        }

        // Remove the socket from the filesystem
        unlink(SOCKET_PATH);

        // Clear the socket address structure
        memset(&s->addr, 0, sizeof(s->addr));

        memset(s->server.orig_str, 0, BUFFER_SIZE);
        memset(s->server.token_str, 0, BUFFER_SIZE);

        free(s);

        printf("[SERVER] Socket Server Shutdown.\n");
    }
}

SocketServer *socket_server_new(void) {
    SocketServer *s = calloc(1, sizeof(SocketServer));
    if (s == NULL) {
        fprintf(stderr, "Error in Calloc of new Server.");
        return NULL;
    }

    // Remove any existing socket file.
    unlink(SOCKET_PATH);

    // Create a Unix domain socket.
    s->fd_server = socket(AF_UNIX, SOCK_STREAM, 0);
    if (s->fd_server == -1) {
        perror("[SERVER] Error: Creating a Unix Domain Socket.");
        socket_server_free(s);
        return NULL;
    }

    // Set up the socket address structure.
    memset(&s->addr, 0, sizeof(struct sockaddr_un));
    s->addr.sun_family = AF_UNIX;
    strncpy(s->addr.sun_path, SOCKET_PATH, sizeof(s->addr.sun_path) - 1);

    // Bind the socket to the file path.
    if (bind(s->fd_server, (struct sockaddr *)&s->addr,
             sizeof(struct sockaddr_un)) == -1) {
        perror("[SERVER] Error: Binding the socket to file path.");
        socket_server_free(s);
        return NULL;
    }

    // Start listening for connections.
    if (listen(s->fd_server, 5) == -1) {
        perror("[SERVER] Error: Listening for connections.");
        socket_server_free(s);
        return NULL;
    }

    s->server.game = game_new();
    if (s->server.game == NULL) {
        socket_server_free(s);
        return NULL;
    }

    s->server.running = true;

    return s;
}

void socket_server_send(SocketServer *s) {
    if (s->server.msg.enable) {
        switch (s->server.msg.type) {
        case SEND_NONE:
            fprintf(s->stream, "NULL\n");
            break;
        case SEND_BOOL:
            fprintf(s->stream, "%i\n", s->server.msg.b_val);
            break;
        case SEND_INT:
            fprintf(s->stream, "%i\n", s->server.msg.i_val);
            break;
        case SEND_FLOAT:
            fprintf(s->stream, "%f\n", s->server.msg.f_val);
            break;
        case SEND_STRING:
            fprintf(s->stream, "%s\n", s->server.msg.s_val);
            break;
        default:
            fprintf(s->stream, "%i\n", s->server.msg.i_val);
            break;
        }

        s->server.msg.enable = false;
        s->server.msg.type = SEND_NONE;
        s->server.msg.i_val = 0;

        memset(s->server.msg.s_val, 0, BUFFER_SIZE);
        fflush(s->stream);
    }
}

bool socket_server_run(pid_t script_pid) {
    printf("[SERVER] Starting socket server.\n");
    SocketServer *s = socket_server_new();
    if (s == NULL) {
        return false;
    }

    printf("[SERVER] Waiting for connection.\n");
    while (s->server.running) {
        // Accept a new client connection on the server socket.
        s->fd_client = accept(s->fd_server, NULL, NULL);
        if (s->fd_client == -1) {
            perror("[SERVER] Error: Accepting client connection.");
            continue;
        }

        // Wrap the client socket in a stream for buffered I/O.
        s->stream = fdopen(s->fd_client, "r+");
        if (s->stream == NULL) {
            perror("[SERVER] Error: Wrapping client socket with fdopen.");
            close(s->fd_client);
            continue;
        }

        while (fgets(s->server.orig_str, BUFFER_SIZE, s->stream) &&
               s->server.running) {
            size_t len = strlen(s->server.orig_str);
            if (len > 0 && s->server.orig_str[len - 1] == '\n') {
                s->server.orig_str[len - 1] = '\0';
            }

            memcpy(s->server.token_str, s->server.orig_str, BUFFER_SIZE);

            if (!parse_line(&s->server)) {
                socket_server_send(s);
                socket_server_free(s);
                if (kill(script_pid, 0) == 0) {
                    kill(script_pid, SIGUSR2);
                }

                return false;
            }

            socket_server_send(s);
        }
    }

    socket_server_free(s);
    if (kill(script_pid, 0) == 0) {
        kill(script_pid, SIGUSR1);
    }

    return true;
}
