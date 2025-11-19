#ifndef MAIN_H
#define MAIN_H

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define WIDTH 800
#define HALF_WIDTH (WIDTH / 2)
#define HEIGHT 600
#define PLAYER_OFFSET 48
#define PLAYER_TOP 377
#define PLAYER_Y 456
#define GROUND 535
#define FLAKES 15
#define VELOCITY 5

#define FILE_NAME "c-network"
#define THREADS 1
#define MAX_THREADS 32
#define INPUTS 15
#define HIDDEN_LAYER 15
#define MAX_LAYER_SIZE 64
#define MAX_LAYERS 5
#define OUTPUTS 2
#define POPULATION 120
#define MAX_POPULATION 500
#define SELECTION (POPULATION * 2 / 3)
#define TRIALS 2
#define MAX_TRIALS 50
#define GENERATIONS 500
#define MAX_GENERATIONS 100000
#define X_RANGE 60
#define Y_RANGE 240
#define MUT_RANGE 15
#define MAX_RANGE 500
#define MUT_RATE 5
#define SCALE 1000

enum {
    SCORE_INDEX = 0,
    TRIALS_INDEX,
    GENERATIONS_INDEX,
    MUTS_INDEX,
    LAYER_COUNT_INDEX,
    LAYER_1_INDEX,
    LAYER_2_INDEX,
    LAYER_3_INDEX,
    LAYER_4_INDEX,
    LAYER_5_INDEX,
    X_RANGE_INDEX,
    Y_RANGE_INDEX,
    NETWORK_INDEX
};

struct AIConfig {
        const char *filename;
        int threads;
        int layer_count;
        int layers[MAX_LAYERS];
        int network_size;
        int population;
        int selection;
        int trials;
        int generations;
        int mut_rate;
        int mut_range;
        int x_range;
        int y_range;
};

typedef struct {
        const struct AIConfig *config;
        int **networks;
        int population;
        int next_index;
        pthread_mutex_t lock;
} WorkQueue;

#endif
