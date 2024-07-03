#include "little.h"


bool detectCycles(int size, int* next_town) {
    bool* visited = calloc(size, sizeof(bool));
    bool* stack = calloc(size, sizeof(bool));
    bool cycle = false;

    for (int start = 0; start < size && !cycle; start++) {
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
    return cycle;
}

bool createsSubTour(int size, int* next_town, int start_index, int start_value) {
    bool* visited = calloc(size, sizeof(bool));
    int current = start_index;
    
    while (current != -1 && !visited[current]) {
        visited[current] = true;
        int next = next_town[current];
        
        // Check if the current position forms a loop back to start_value
        if (next == start_value) {
            free(visited);
            return true;
        }
        
        current = next;
    }
    
    // Check if adding start_value directly creates a new sub-tour
    if (next_town[start_index] == -1 && start_value != start_index && start_value == next_town[start_value]) {
        free(visited);
        return true;
    }

    free(visited);
    return false;
}


float min_rows(int size, float* d) {
    float current_eval = 0.0;
    int i, j;

#ifdef OPENMP
#pragma omp parallel
#endif
    {
        float local_eval = 0.0;
#ifdef OPENMP
#pragma omp for private(i) schedule(static, NUM_TASKS_PER_THREAD(size))
#endif
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
#ifdef OPENMP
#pragma omp atomic
#endif
        current_eval += local_eval;
    }
#ifdef OPENMP
// #pragma omp barrier
#endif
    return current_eval;
}

float min_cols(int size, float* d) {
    float current_eval = 0.0;
    int i, j;

#ifdef OPENMP
#pragma omp parallel 
#endif
    {
        float local_eval = 0.0;
#ifdef OPENMP
#pragma omp for private(j) schedule(static, NUM_TASKS_PER_THREAD(size))
#endif
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
#ifdef OPENMP
#pragma omp atomic
#endif
        current_eval += local_eval;
    }

#ifdef OPENMP
// #pragma omp barrier
#endif
    return current_eval;
}

/**
 *  Little Algorithm
 */
void little_algorithm(int size, float* dist, float* baseDist, int iteration, float eval_node_parent, int* best_solution, float* best_eval, int* next_town, configuration_t config) {
    // int myIteration = nbit++;
    if (iteration == size) {
        // bool best = 
        build_solution(size, baseDist, next_town, best_solution, best_eval, config);
        // fprintf(f, "node%d [label=\"S%d-%d\n(%.2f)\", %s];\n", myIteration, myIteration, iteration, eval_node_parent, best ? "color=limegreen" : "color=lightblue");
        // return myIteration;
        free(dist);
        return;
    }

    bool cycle = detectCycles(size, next_town);
    if (cycle) {
        // fprintf(f, "node%d [label=\"S%d-%d\n(%.2f)\" color=red];\n", myIteration, myIteration, iteration, eval_node_child);
        // return myIteration;
        free(dist);
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
    if (*best_eval >= 0 && eval_node_child >= *best_eval) {
        // fprintf(f, "node%d [label=\"S%d-%d\n(%.2f)\" color=red];\n", myIteration, myIteration, iteration, eval_node_child);
        // return myIteration;
        free(d);
        return;
    }

    /* row and column of the zero with the max penalty */
    int izero = -1, jzero = -1;
    float max_penalty = -1;
    int i,j,k;

#ifdef OPENMP
#pragma omp parallel for collapse(2) private(i, j) shared(max_penalty, izero, jzero) schedule(static, NUM_TASKS_PER_THREAD(size))
#endif
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            if (d[i * size + j] == 0 && !createsSubTour(size, next_town, i, j)) {
                float min_row = -1;
                float min_col = -1;
#ifdef OPENMP
// #pragma omp parallel for private(k) schedule(static, NUM_TASKS_PER_THREAD(size))
#endif
                for (k = 0; k < size; k++) {
                    float valik = d[i * size + k];
                    float valkj = d[k * size + j];

                    if (k != j && valik >= 0 && (min_row < 0 || valik < min_row)) {
                        min_row = valik;
                    }
                    if (k != i && valkj >= 0 && (min_col < 0 || valkj < min_col)) {
                        min_col = valkj;
                    }
                }
                if (min_row < 0 || min_col < 0) {
                    min_row = BIG_VALUE;
                    min_col = BIG_VALUE;
                }
                float penalty = min_row + min_col;
#ifdef OPENMP
#pragma omp critical
#endif 
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

#ifdef OPENMP
// #pragma omp barrier
#endif
    if (izero < 0 || jzero < 0) {
        // fprintf(f, "node%d [label=\"S%d-%d\n(%.2f)\" color=red];\n", myIteration, myIteration, iteration, eval_node_child);
        // return myIteration;
        free(d);
        return;
    }
    // fprintf(f, "node%d [label=\"S%d-%d\n(%.2f)\"];\n", myIteration, myIteration, iteration, eval_node_child);

    next_town[izero] = jzero;
  
    /* Do the modification on a copy of the distance matrix */
    float* d2 = malloc(size * size * sizeof(float));
    memcpy(d2, d, size * size * sizeof(float));
    
    float* d3 = malloc(size * size * sizeof(float));
    memcpy(d3, d, size * size * sizeof(float));

    free(d);

#ifdef OPENMP
#pragma omp parallel for private(k) schedule(static, NUM_TASKS_PER_THREAD(size))
#endif
    for (k = 0; k < size; k++) {
        d2[izero * size + k] = -1;
        d2[k * size + jzero] = -1;
    }
    d2[jzero * size + izero] = -1; // si B -> A alors A -> B interdit

    /* Explore left child node according to given choice */
    
    // int choice = 
    little_algorithm(size, d2, baseDist, iteration + 1, eval_node_child, best_solution, best_eval, next_town, config);
    // fprintf(f, "node%d -> node%d;\n", myIteration, choice);


    next_town[izero] = -1;
    d3[izero * size + jzero] = -1;

    // int nochoice = 
    little_algorithm(size, d3, baseDist, iteration, eval_node_child, best_solution, best_eval, next_town, config);
    // fprintf(f, "node%d -> node%d;\n", myIteration, nochoice);

    // return myIteration;
    return;
}