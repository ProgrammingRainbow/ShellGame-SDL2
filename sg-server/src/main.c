#include "pipe.h"
#include "shared.h"
#include "socket.h"
#include "stdinout.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Error: %s requires an argument.\n", argv[0]);
        return EXIT_FAILURE;
    }

    pid_t script_pid = (pid_t)atoi(argv[2]);

    if (strcmp(argv[1], "--pipe") == 0) {
        if (!pipe_server_run(script_pid)) {
            return EXIT_FAILURE;
        }
    } else if (strcmp(argv[1], "--stdinout") == 0) {
        if (!stdinout_server_run(script_pid)) {
            return EXIT_FAILURE;
        }
    } else if (strcmp(argv[1], "--socket") == 0) {
        if (!socket_server_run(script_pid)) {
            return EXIT_FAILURE;
        }
    } else if (strcmp(argv[1], "--shared") == 0) {
        if (!shared_server_run(script_pid)) {
            return EXIT_FAILURE;
        }
    } else {
        fprintf(stderr, "Error: Argument %s is not valid.\n", argv[1]);
    }

    return EXIT_SUCCESS;
}
