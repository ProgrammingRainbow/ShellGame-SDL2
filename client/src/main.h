#ifndef CLIENT_H
#define CLIENT_H

#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

#define SHM_DATA_PATH "/dev/shm/sg_shared_data"
#define SHM_LOCK_PATH "/dev/shm/sg_shared_lock"

#define BUFFER_SIZE 1024

typedef struct {
        int fd_shm_data;
        int fd_shm_lock;
        volatile char *shm_data;
        volatile char *shm_lock;
} SharedClient;

#endif
