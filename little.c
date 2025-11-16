#include "little.h"


bool detectCycles(int size, int* next_town) {
    bool cycle = false;
    {
        bool* visited = calloc(size, sizeof(bool));
        bool* stack = calloc(size, sizeof(bool));

        for (int start = 0; start < size; start++) {
            if (visited[start]) continue;

            int current = start;
            while (current != -1 && !visited[current] && !cycle) {
                visited[current] = true;
                stack[current] = true;
                int next = next_town[current];

                if (next != -1) {
                    if (!visited[next]) {
                        current = next;
                    } else if (stack[next]) {
                        // Cycle detected
                        cycle = true;
                    }
                } else {
                    current = next;
                }
            }

            // Clear stack
            current = start;
            while (current != -1 && stack[current]) {
                stack[current] = false;
                current = next_town[current];
            }
        }
        free(stack);
        free(visited);
    }
    return cycle;
}

// making it work correctly.
bool createsSubTour(int size, int* next_town, int start_index, int start_value) {
    bool* visited = calloc(size, sizeof(bool));
    int current = start_value;

    while (current != -1 && !visited[current]) {
        if (current == start_index || visited[current]) {
            // Cycle detected
            free(visited);
            return true;
        }
        visited[current] = true;
        current = next_town[current];
    }

    free(visited);
    return false;
}


float min_rows(int size, float* d) {
    float current_eval = 0.0;
    int i, j;

    {
        float local_eval = 0.0;
        for (i = 0; i < size; i++) {
            float minl = -1;
            for (j = 0; j < size; j++) {
                float val = d[i * size + j];
                if (minl < 0 || (val >= 0 && val < minl)) {
                    minl = val;
                }
            }
            if (minl >= 0) {
                local_eval += minl;
                for (j = 0; j < size; j++) {
                    if (d[i * size + j] >= 0) {
                        d[i * size + j] -= minl;
                    }
                }
            }
        }
        current_eval += local_eval;
    }
    return current_eval;
}

float min_cols(int size, float* d) {
    float current_eval = 0.0;
    int i, j;

    {
        float local_eval = 0.0;
        for (j = 0; j < size; j++) {
            float minc = -1;
            for (i = 0; i < size; i++) {
                float val = d[i * size + j];
                if (minc < 0 || (val >= 0 && val < minc)) {
                    minc = val;
                }
            }
            if (minc >= 0) {
                local_eval += minc;
                for (i = 0; i < size; i++) {
                    if (d[i * size + j] >= 0) {
                        d[i * size + j] -= minc;
                    }
                }
            }
        }
        current_eval += local_eval;
    }

    return current_eval;
}

/**
 *  Little Algorithm
 */
void little_algorithm(int size, float* dist, float* baseDist, int iteration, float eval_node_parent, int* best_solution, float* best_eval, int* next_town, configuration_t config) {
    if (iteration == size) {
        // bool best = 
        build_solution(size, baseDist, next_town, best_solution, best_eval, config);
        free(dist);
        free(next_town);
        return;
    }

    bool cycle = detectCycles(size, next_town);
    if (cycle) {
        free(dist);
        free(next_town);
        return;
    }

    /* Do the modification on a copy of the distance matrix */
    float* d = malloc(size * size * sizeof(float));
    memcpy(d, dist, size * size * sizeof(float));
    free(dist);

    float eval_node_child = eval_node_parent;

    eval_node_child += min_rows(size, d);
    eval_node_child += min_cols(size, d);

    /* Cut : stop the exploration of this node */
    if (*best_eval >= 0 && eval_node_child > *best_eval) {
        free(d);
        free(next_town);
        return;
    }

    /* row and column of the zero with the max penalty */
    int izero = -1, jzero = -1;
    float max_penalty = -1;
    int i, j, k;

    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            if (d[i * size + j] == 0) {
                float min_row = -1;
                float min_col = -1;

                for (k = 0; k < size; k++) {
                    float valik = d[i * size + k];
                    float valkj = d[k * size + j];

                    if (k != j && valik >= 0 && (valik < min_row || min_row < 0)) {
                        min_row = valik;
                    }
                    if (k != i && valkj >= 0 && (valkj < min_col || min_col < 0)) {
                        min_col = valkj;
                    }
                }
                if (min_row < 0 || min_col < 0) {
                    min_row = BIG_VALUE;
                    min_col = BIG_VALUE;
                }
                float penalty = min_row + min_col;
                {
                    if (penalty > max_penalty) {
                        max_penalty = penalty;
                        izero = i;
                        jzero = j;
                    }
                }
            }
        }
    }

    if (izero < 0 || jzero < 0) {
        free(d);
        free(next_town);
        return;
    }


    float* d2 = malloc(size * size * sizeof(float));
    memcpy(d2, d, size * size * sizeof(float));

    float* d3 = malloc(size * size * sizeof(float));
    memcpy(d3, d, size * size * sizeof(float));

    free(d);
    // D2
    for (k = 0; k < size; k++) {
        d2[izero * size + k] = -1;
        d2[k * size + jzero] = -1;
    }
    d2[jzero * size + izero] = -1;
    // D3
    d3[izero * size + jzero] = -1;

    int* next_town_child1 = malloc(size * sizeof(int));
    memcpy(next_town_child1, next_town, size * sizeof(int));
    next_town_child1[izero] = jzero;

    int* next_town_child2 = malloc(size * sizeof(int));
    memcpy(next_town_child2, next_town, size * sizeof(int));
    next_town_child2[izero] = -1;

    free(next_town);

#ifdef OPENMP
#pragma omp task if (iteration < size)
#endif
    {
        little_algorithm(size, d3, baseDist, iteration, eval_node_child, best_solution, best_eval, next_town_child2, config);
    }

    {
        little_algorithm(size, d2, baseDist, iteration + 1, eval_node_child, best_solution, best_eval, next_town_child1, config);
    }

    return;
}