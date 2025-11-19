#include "main.h"

bool mutate_rate(int mut_rate);
int random_int(int mut_range);
bool is_number(const char *str);
bool is_filename(const char *str);
bool parse_arguments(int argc, char *argv[], struct AIConfig *config);
void trials_run(const struct AIConfig *config, int *network, unsigned *seed);
void *worker(void *arg);
void run_parallel(const struct AIConfig *config, int **networks);
bool train_networks(const struct AIConfig *config);

bool mutate_rate(int mut_rate) { return (rand() % SCALE) < mut_rate; }

int random_int(int mut_range) {
    return (rand() % (mut_range * 2 + 1)) - mut_range;
}

bool is_number(const char *str) {
    if (*str == '+' || *str == '-') {
        str++;
    }
    do {
        if (*str < '0' || *str > '9') {
            return false;
        }
        str++;
    } while (*str != '\0');

    return true;
}

bool is_filename(const char *str) {
    while (*str != '\0') {
        if (*str < 'a' || *str > 'z') {
            if (*str < 'A' || *str > 'Z') {
                if (*str < '0' || *str > '9') {
                    if (*str != '-' && *str != '_') {
                        return false;
                    }
                }
            }
        }
        str++;
    }

    return true;
}

bool parse_arguments(int argc, char *argv[], struct AIConfig *config) {
    config->filename = FILE_NAME;
    config->threads = THREADS;
    config->layer_count = 1;
    config->layers[0] = HIDDEN_LAYER;
    config->population = POPULATION;
    config->selection = SELECTION;
    config->trials = TRIALS;
    config->generations = GENERATIONS;
    config->mut_rate = MUT_RATE;
    config->mut_range = MUT_RANGE;
    config->x_range = X_RANGE;
    config->y_range = Y_RANGE;

    int argument = 1;
    while (argument < argc) {
        if (strcmp(argv[argument], "--filename") == 0) {
            argument++;
            if (argument < argc) {
                if (!is_filename(argv[argument])) {
                    fprintf(stderr, "--filename %s is not valid.\n",
                            argv[argument]);
                    return false;
                }
                config->filename = argv[argument];
            }
        } else if (strcmp(argv[argument], "--threads") == 0) {
            argument++;
            if (argument < argc) {
                int num = atoi(argv[argument]);
                if (!is_number(argv[argument]) || num < 1 ||
                    num > MAX_THREADS) {
                    fprintf(stderr, "--threads %s is not valid.\n",
                            argv[argument]);
                    return false;
                }
                config->threads = num;
            }
        } else if (strcmp(argv[argument], "--population") == 0) {
            argument++;
            if (argument < argc) {
                int num = atoi(argv[argument]);
                if (!is_number(argv[argument]) || num < 1 ||
                    num > MAX_POPULATION) {
                    fprintf(stderr, "--population %s is not valid.\n",
                            argv[argument]);
                    return false;
                }
                config->population = num;
            }
        } else if (strcmp(argv[argument], "--selection") == 0) {
            argument++;
            if (argument < argc) {
                int num = atoi(argv[argument]);
                if (!is_number(argv[argument]) || num < 1 ||
                    num > MAX_POPULATION) {
                    fprintf(stderr, "--selection %s is not valid.\n",
                            argv[argument]);
                    return false;
                }
                config->selection = num;
            }
        } else if (strcmp(argv[argument], "--trials") == 0) {
            argument++;
            if (argument < argc) {
                int num = atoi(argv[argument]);
                if (!is_number(argv[argument]) || num < 1 || num > MAX_TRIALS) {
                    fprintf(stderr, "--trials %s is not valid.\n",
                            argv[argument]);
                    return false;
                }
                config->trials = num;
            }
        } else if (strcmp(argv[argument], "--generations") == 0) {
            argument++;
            if (argument < argc) {
                int num = atoi(argv[argument]);
                if (!is_number(argv[argument]) || num < 1 ||
                    num > MAX_GENERATIONS) {
                    fprintf(stderr, "--generations %s is not valid.\n",
                            argv[argument]);
                    return false;
                }
                config->generations = num;
            }
        } else if (strcmp(argv[argument], "--mut_rate") == 0) {
            argument++;
            if (argument < argc) {
                int num = atoi(argv[argument]);
                if (!is_number(argv[argument]) || num < 1 || num > MAX_RANGE) {
                    fprintf(stderr, "--mut_rate %s is not valid.\n",
                            argv[argument]);
                    return false;
                }
                config->mut_rate = num;
            }
        } else if (strcmp(argv[argument], "--mut_range") == 0) {
            argument++;
            if (argument < argc) {
                int num = atoi(argv[argument]);
                if (!is_number(argv[argument]) || num < 1 || num > MAX_RANGE) {
                    fprintf(stderr, "--mut_range %s is not valid.\n",
                            argv[argument]);
                    return false;
                }
                config->mut_range = num;
            }
        } else if (strcmp(argv[argument], "--x_range") == 0) {
            argument++;
            if (argument < argc) {
                int num = atoi(argv[argument]);
                if (!is_number(argv[argument]) || num < 1 || num > MAX_RANGE) {
                    fprintf(stderr, "--x_range %s is not valid.\n",
                            argv[argument]);
                    return false;
                }
                config->x_range = num;
            }
        } else if (strcmp(argv[argument], "--y_range") == 0) {
            argument++;
            if (argument < argc) {
                int num = atoi(argv[argument]);
                if (!is_number(argv[argument]) || num < 1 || num > MAX_RANGE) {
                    fprintf(stderr, "--y_range %s is not valid.\n",
                            argv[argument]);
                    return false;
                }
                config->y_range = num;
            }
        } else if (strcmp(argv[argument], "--layers") == 0) {
            config->layer_count = 0;
            while (argument + 1 < argc && config->layer_count <= MAX_LAYERS &&
                   is_number(argv[argument + 1])) {
                argument++;
                int num = atoi(argv[argument]);
                if (num < OUTPUTS || num > MAX_LAYER_SIZE) {
                    fprintf(stderr, "layer size of %s is not valid.\n",
                            argv[argument]);
                    return false;
                }

                config->layers[config->layer_count] = num;
                config->layer_count++;
            }

            if (config->layer_count == 0) {
                fprintf(stderr, "no suitable layers were set.\n");
                return false;
            }
        }
        argument++;
    }

    config->network_size = (INPUTS + 1) * config->layers[0];
    for (int i = 1; i < config->layer_count; i++) {
        config->network_size += (config->layers[i - 1] + 1) * config->layers[i];
    }
    config->network_size +=
        (config->layers[config->layer_count - 1] + 1) * OUTPUTS + NETWORK_INDEX;

    if (config->selection > config->population) {
        fprintf(
            stderr,
            "Error: Top Variants %d cannot be larger than top_variants %d.\n",
            config->selection, config->population);
        return false;
    }

    if (config->layer_count) {
        printf("Generating new network with layers:");
        for (int i = 0; i < config->layer_count; i++) {
            printf(" %i", config->layers[i]);
        }
        printf("\n");
    }

    printf("\n");
    printf("filename: %s\n", config->filename);
    printf("threads: %d\n", config->threads);
    printf("layer_count: %d\n", config->layer_count);
    printf("layers:");
    for (int i = 0; i < config->layer_count; i++) {
        printf(" %d", config->layers[i]);
    }
    printf("\n");
    printf("population: %d\n", config->population);
    printf("selection: %d\n", config->selection);
    printf("trials: %d\n", config->trials);
    printf("generations: %d\n", config->generations);
    printf("mut_rate: %d\n", config->mut_rate);
    printf("mut_range: %d\n", config->mut_range);
    printf("x_range: %d\n", config->x_range);
    printf("y_range: %d\n", config->y_range);
    printf("\n");

    sleep(5);

    return true;
}

void trials_run(const struct AIConfig *config, int *network, unsigned *seed) {
    bool playing = true;
    int score = 0;
    int player_x = HALF_WIDTH;
    int flakes_x[FLAKES] = {0};
    int flakes_y[FLAKES] = {0};
    int buffer[MAX_LAYERS + 2][MAX_LAYER_SIZE] = {0};

    for (int t = 0; t < config->trials; t++) {
        for (int f = 0; f < FLAKES; f++) {
            flakes_x[f] = rand_r(seed) % WIDTH;
            flakes_y[f] = -(rand_r(seed) % (WIDTH * 2));
        }

        playing = true;
        while (playing) {
            int *inputs = buffer[0];
            int *outputs = buffer[1];
            for (int f = 0; f < FLAKES; f++) {
                int rel_x = (flakes_x[f] - player_x);
                if (rel_x > HALF_WIDTH) {
                    rel_x -= WIDTH;
                } else if (rel_x < -HALF_WIDTH) {
                    rel_x += WIDTH;
                }
                if (rel_x > 0) {
                    if (rel_x < config->x_range) {
                        rel_x = (config->x_range - rel_x);
                    } else {
                        rel_x = 0;
                    }
                } else {
                    if (rel_x > -config->x_range) {
                        rel_x = (-config->x_range + rel_x);
                    } else {
                        rel_x = 0;
                    }
                }
                int rel_y = abs(PLAYER_Y - flakes_y[f]);
                if (rel_y < config->y_range) {
                    rel_y = (config->y_range - rel_y);
                } else {
                    rel_y = 0;
                }

                inputs[f] = rel_x * rel_y * 200;
            }

            int input_count = INPUTS;
            int index = NETWORK_INDEX;
            for (int l = 0; l < config->layer_count; l++) {
                for (int n = 0; n < config->layers[l]; n++) {
                    for (int i = 0; i < input_count; i++) {
                        outputs[n] += inputs[i] * network[index];
                        index++;
                    }
                    outputs[n] /= SCALE;
                    outputs[n] += network[index];
                    index++;
                    outputs[n] = (outputs[n] > 0) ? outputs[n] : 0;
                }
                input_count = config->layers[l];
                inputs = buffer[l + 1];
                outputs = buffer[l + 2];
            }

            for (int o = 0; o < OUTPUTS; o++) {
                for (int h = 0; h < input_count; h++) {
                    outputs[o] += inputs[h] * network[index];
                    index++;
                }
                outputs[o] /= SCALE;
                outputs[o] += network[index];
                index++;
                outputs[o] = (outputs[o] > 0) ? outputs[o] : 0;
            }

            if (outputs[0] > outputs[1]) {
                player_x -= VELOCITY;
                if (player_x < 0) {
                    player_x += WIDTH;
                }
            } else if (outputs[1] > outputs[0]) {
                player_x += VELOCITY;
                if (player_x > WIDTH) {
                    player_x -= WIDTH;
                }
            }

            for (int f = 0; f < FLAKES; f++) {
                flakes_y[f] += VELOCITY;
                if (flakes_y[f] > PLAYER_TOP) {
                    if (flakes_y[f] > GROUND) {
                        flakes_x[f] = rand_r(seed) % WIDTH;
                        flakes_y[f] = -(rand_r(seed) % HEIGHT);
                    } else if (flakes_x[f] > player_x - PLAYER_OFFSET) {
                        if (flakes_x[f] < player_x + PLAYER_OFFSET) {
                            if (f < 5) {
                                playing = 0;
                            } else {
                                score++;
                                flakes_x[f] = rand_r(seed) % WIDTH;
                                flakes_y[f] = -(rand_r(seed) % HEIGHT);
                            }
                        }
                    }
                }
            }
        }
    }
    network[SCORE_INDEX] = score;
}

void *worker(void *arg) {
    WorkQueue *queue = (WorkQueue *)arg;
    unsigned seed = (unsigned)time(NULL) ^ (unsigned)pthread_self();

    while (true) {
        int index;

        pthread_mutex_lock(&queue->lock);
        if (queue->next_index >= queue->population) {
            pthread_mutex_unlock(&queue->lock);
            break;
        }
        index = queue->next_index++;
        pthread_mutex_unlock(&queue->lock);

        trials_run(queue->config, queue->networks[index], &seed);
    }
    return NULL;
}

void run_parallel(const struct AIConfig *config, int **networks) {
    pthread_t threads[config->threads];

    WorkQueue queue = {.config = config,
                       .networks = networks,
                       .population = config->population,
                       .next_index = 0,
                       .lock = PTHREAD_MUTEX_INITIALIZER};

    for (int i = 0; i < config->threads; i++) {
        pthread_create(&threads[i], NULL, worker, &queue);
    }

    for (int i = 0; i < config->threads; i++) {
        pthread_join(threads[i], NULL);
    }
}

bool train_networks(const struct AIConfig *config) {
    int *networks[config->population];
    for (int p = 0; p < config->population; p++) {
        networks[p] = calloc(1, sizeof(int) * (unsigned)config->network_size);
        if (!networks[p]) {
            fprintf(stderr, "Memory allocation failed for network %d\n", p);
            return false;
        }
    }

    for (int p = 0; p < config->population; p++) {
        networks[p][SCORE_INDEX] = 0;
        networks[p][TRIALS_INDEX] = config->trials;
        networks[p][GENERATIONS_INDEX] = 0;
        networks[p][MUTS_INDEX] = 0;
        networks[p][LAYER_COUNT_INDEX] = config->layer_count;
        networks[p][LAYER_1_INDEX] = config->layers[0];
        networks[p][LAYER_2_INDEX] = config->layers[1];
        networks[p][LAYER_3_INDEX] = config->layers[2];
        networks[p][LAYER_4_INDEX] = config->layers[3];
        networks[p][LAYER_5_INDEX] = config->layers[4];
        networks[p][X_RANGE_INDEX] = config->x_range;
        networks[p][Y_RANGE_INDEX] = config->y_range;

        for (int i = NETWORK_INDEX; i < config->network_size; i++) {
            networks[p][i] = random_int(config->mut_range);
        }
    }

    for (int g = 0; g < config->generations; g++) {
        // Old sequential version
        // for (int p = 0; p < config->population; p++) {
        //     trials_run(config, networks[p]);
        // }

        // New parallel version
        run_parallel(config, networks);

        int *sorted[config->population];
        for (int i = 0; i < config->population; i++) {
            sorted[i] = networks[i];
        }

        for (int i = 0; i < config->population - 1; i++) {
            for (int j = i + 1; j < config->population; j++) {
                if (sorted[j][SCORE_INDEX] > sorted[i][SCORE_INDEX]) {
                    int *temp = sorted[i];
                    sorted[i] = sorted[j];
                    sorted[j] = temp;
                }
            }
        }

        for (int i = 0; i < 20; i++) {
            printf("Rank: %d, Fitness: %d, Generation: %d, Mutations: %d\n", i,
                   sorted[i][SCORE_INDEX] / sorted[i][TRIALS_INDEX],
                   sorted[i][GENERATIONS_INDEX], sorted[i][MUTS_INDEX]);
        }

        for (int i = 0; i < config->population; i++) {
            int selected = i % config->selection;
            memcpy(networks[i], sorted[selected],
                   sizeof(int) * (unsigned)config->network_size);
        }

        for (int p = 0; p < config->population; p++) {
            networks[p][GENERATIONS_INDEX]++;
            if (p >= config->selection) {
                networks[p][MUTS_INDEX]++;

                for (int i = NETWORK_INDEX; i < config->network_size; i++) {
                    if (mutate_rate(config->mut_rate)) {
                        networks[p][i] = random_int(config->mut_range);
                    }
                }
            }
        }
    }

    FILE *fp = fopen(config->filename, "w");
    for (int n = 0; n < config->network_size; n++) {
        fprintf(fp, "%d ", networks[0][n]);
    }
    fclose(fp);

    for (int i = 0; i < config->population; i++) {
        free(networks[i]);
    }

    return true;
}

int main(int argc, char *argv[]) {
    // srand((unsigned)time(NULL));

    struct AIConfig ai_config = {0};

    if (!parse_arguments(argc, argv, &ai_config)) {
        return EXIT_FAILURE;
    }

    if (!train_networks(&ai_config)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
