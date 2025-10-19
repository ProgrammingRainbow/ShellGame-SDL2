#include "shared.h"
#include "parser.h"

#include <fcntl.h>
#include <signal.h>
#include <sys/mman.h>
#include <unistd.h>

void shared_server_free(SharedServer *s);
SharedServer *shared_server_new(void);
void shared_server_send(SharedServer *s);

void shared_server_free(SharedServer *s) {
    if (s) {
        if (s->server.game) {
            game_free(s->server.game);
            s->server.game = NULL;
        }

        // Unmap the shared data memory region.
        if (s->shm_data) {
            munmap((void *)s->shm_data, BUFFER_SIZE);
            s->shm_data = NULL;
        }

        // Unmap the shared lock memory region.
        if (s->shm_lock) {
            munmap((void *)s->shm_lock, BUFFER_SIZE);
            s->shm_lock = NULL;
        }

        // Close the file descriptor for shared data.
        if (s->fd_shm_data >= 0) {
            close(s->fd_shm_data);
            s->fd_shm_data = -1;
        }

        // Close the file descriptor for shared lock.
        if (s->fd_shm_lock >= 0) {
            close(s->fd_shm_lock);
            s->fd_shm_lock = -1;
        }

        // Remove shared memory from the filesystem.
        unlink(SHM_DATA_PATH);
        unlink(SHM_LOCK_PATH);

        free(s);

        printf("[SERVER] Shared Server Shutdown.\n");
    }
}

SharedServer *shared_server_new(void) {
    SharedServer *s = calloc(1, sizeof(SharedServer));
    if (!s) {
        fprintf(stderr, "Error allocating SharedServer.\n");
        return NULL;
    }

    // Open the shared memory data file for reading and writing.
    s->fd_shm_data = open(SHM_DATA_PATH, O_RDWR);
    if (s->fd_shm_data < 0) {
        perror("[SERVER] Error: Opening shared memory file.");
        shared_server_free(s);
        return NULL;
    }

    // Resize the shared memory data file to match the buffer size.
    if (ftruncate(s->fd_shm_data, BUFFER_SIZE) == -1) {
        perror("[SERVER] Error: Ftruncating shared memory data file.");
        shared_server_free(s);
        return NULL;
    }

    // Open the shared memory lock file for reading and writing.
    s->fd_shm_lock = open(SHM_LOCK_PATH, O_RDWR);
    if (s->fd_shm_lock < 0) {
        perror("[SERVER] Error: Opening shared memory file.");
        shared_server_free(s);
        return NULL;
    }

    // Resize the shared memory lock file to match the buffer size.
    if (ftruncate(s->fd_shm_lock, BUFFER_SIZE) == -1) {
        perror("[SERVER] Error: Ftruncating shared memory lock file.");
        shared_server_free(s);
        return NULL;
    }

    // Map the shared data file into memory for read/write access
    s->shm_data = mmap(NULL, BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
                       s->fd_shm_data, 0);
    if (s->shm_data == MAP_FAILED) {
        perror("[SERVER] Error: Mmaping shared memory data file.");
        shared_server_free(s);
        return NULL;
    }

    // Map the shared lock file into memory for read/write access
    s->shm_lock = mmap(NULL, BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
                       s->fd_shm_lock, 0);
    if (s->shm_lock == MAP_FAILED) {
        perror("[SERVER] Error: Mmaping shared memory lock file.");
        shared_server_free(s);
        return NULL;
    }

    // Initialize both shared memory regions to zero.
    memset((void *)s->shm_data, 0, BUFFER_SIZE);
    memset((void *)s->shm_lock, 0, BUFFER_SIZE);

    s->server.game = game_new();
    if (!s->server.game) {
        shared_server_free(s);
        return NULL;
    }

    s->server.running = true;
    return s;
}

void shared_server_send(SharedServer *s) {
    if (s->server.msg.enable) {
        memset((void *)s->shm_data, 0, BUFFER_SIZE);
        switch (s->server.msg.type) {
        case SEND_NONE:
            snprintf((char *)s->shm_data, BUFFER_SIZE, "NULL");
            break;
        case SEND_BOOL:
            snprintf((char *)s->shm_data, BUFFER_SIZE, "%i",
                     s->server.msg.b_val);
            break;
        case SEND_INT:
            snprintf((char *)s->shm_data, BUFFER_SIZE, "%i",
                     s->server.msg.i_val);
            break;
        case SEND_FLOAT:
            snprintf((char *)s->shm_data, BUFFER_SIZE, "%f",
                     s->server.msg.f_val);
            break;
        case SEND_STRING:
            snprintf((char *)s->shm_data, BUFFER_SIZE, "%s",
                     s->server.msg.s_val);
            break;
        default:
            snprintf((char *)s->shm_data, BUFFER_SIZE, "%i",
                     s->server.msg.i_val);
            break;
        }

        s->shm_lock[0] = 2;

        s->server.msg.enable = false;
        s->server.msg.type = SEND_NONE;
        s->server.msg.i_val = 0;

        memset(s->server.msg.s_val, 0, BUFFER_SIZE);
    } else {
        s->shm_lock[0] = 0;
    }
}

bool shared_server_run(pid_t script_pid) {
    printf("[SERVER] Starting shared server.\n");
    SharedServer *s = shared_server_new();
    if (!s) {
        return false;
    }

    struct timespec ts = {0, 1000};

    printf("[SERVER] Waiting for shared data.\n");
    while (s->server.running) {
        if (s->shm_lock[0] == 1) {
            // printf("recived string \"%s\"\n", s->server.orig_str);
            memcpy(s->server.orig_str, (char *)s->shm_data, BUFFER_SIZE);

            memcpy(s->server.token_str, s->server.orig_str, BUFFER_SIZE);

            if (!parse_line(&s->server)) {
                shared_server_send(s);
                shared_server_free(s);
                if (kill(script_pid, 0) == 0)
                    kill(script_pid, SIGUSR2);
                return false;
            }

            shared_server_send(s);
        }
        nanosleep(&ts, NULL);
    }

    shared_server_free(s);
    if (kill(script_pid, 0) == 0)
        kill(script_pid, SIGUSR1);
    return true;
}
