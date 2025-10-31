#ifndef SHARED_H
#define SHARED_H

#define SHM_DATA_PATH "/dev/shm/sg_shared_data"
#define SHM_LOCK_PATH "/dev/shm/sg_shared_lock"

#include "game.h"

typedef struct {
        int fd_shm_data;
        int fd_shm_lock;
        volatile char *shm_data;
        volatile char *shm_lock;
        SdlServer server;
} SharedServer;

bool shared_server_run(pid_t script_pid);

#endif
