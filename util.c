#include "util.h"

void readTSPFile(char *filename, int size, float **coords) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open file %s\n", filename);
        exit(1);
    }

    char line[128];
    int i;

    // skip the first 6 lines
    for (i = 0; i < 6; i++) {
        char* c = fgets(line, sizeof(line), file);
        (void)c;
    }  
    *coords = (float *)malloc((size) * (2) * sizeof(float));
    float* d = *coords;

    i = 0;
    while (fgets(line, sizeof(line), file) != NULL && i <= size) {
        if (sscanf(line, "%d %f %f", &i, &d[0], &d[1]) == 3) {
            d += 2;
            i++;    
        }
    }

    fclose(file);
}

bool path_to_file_exists(char *path) {
    if (!path) return false;
    char path_copy[STR_MAX_LEN];
    strcpy(path_copy, path);
    char *dir_path = dirname(path_copy);
    bool exists = directory_exists(dir_path);
    if (exists) {
        // open file to check if it exists
        FILE* f = fopen(path, "r");
        if (f) {
            fclose(f);
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

bool directory_exists(char *path) {
    if (!path) return false;
    DIR *dir = opendir(path);
    if (dir) {
        closedir(dir);
        return true;
    } else {
        return false;
    }
    
}

void compute_matrix(int size, float** coord, float* d) {
    int i, j;
#ifdef OPENMP
#pragma omp parallel for private(j) if(size > 10000)
#endif
    for (i = 0; i < size; i++) {
        float xi = coord[i][0];
        float yi = coord[i][1];

        for (j = 0; j < size; j++) {
            if (i == j) {
                d[i * size + j] = -1;
                continue;
            }
            float xj = coord[j][0];
            float yj = coord[j][1];
            d[i * size + j] = sqrt((xi - xj) * (xi - xj) + (yi - yj) * (yi - yj));
        }
    }
}

int find_nearest_unvisited(int current_town, int size, bool* visited, float* dist) {
    int nearest_town = -1;
    float min_distance = -1;

    for (int j = 0; j < size; j++) {
        if (!visited[j]) {
            float distance = dist[current_town * size + j];
            if (min_distance < 0 || distance < min_distance) {
                min_distance = distance;
                nearest_town = j;
            }
        }
    }
    return nearest_town;
}

void reverse(int size, int* solution, int i, int j) {
    while (i < j) {
        int temp = solution[i];
        solution[i] = solution[j];
        solution[j] = temp;
        i++;
        j--;
    }
}

static void _2opt_sequential(int size, int* sol, float* dist, float* eval) {
    for (int i = 1; i < size; i++) {
        for (int j = i + 1; j < size; j++) {
            if (((i != 0) || (j != size - 1)) && ((i != 0) || (j != size - 2))) {
                reverse(size, sol, i, j);
                
                float new_eval = evaluation_solution(size, sol, dist);
                if (new_eval < *eval) {
                    *eval = new_eval;
                } else {
                    reverse(size, sol, i, j);
                }
            }
        }
    }
}

/**
 * initial solution
 */
float initial_solution(int size, float* dist, int* best_solution, float *best_eval) {
    /* solution of the nearest neighbour */
    int* sol = (int *)calloc(size, sizeof(int));
    bool* visited = calloc(size, sizeof(bool));
    float eval = 0.0;

    int i, j;

    sol[0] = 0;
    visited[0] = true;

    for (i = 1; i < size; i++) {
        int current_town = sol[i - 1];
        int next_town = find_nearest_unvisited(current_town, size, visited, dist);

        sol[i] = next_town;
        visited[next_town] = true;
    }

    eval = evaluation_solution(size, sol, dist);

#ifdef OPENMP
    if (size >= 1000) {
#pragma omp parallel for private(j)
        for (i = 1; i < size; i++) {
            for (j = i + 1; j < size; j++) {
                if (((i != 0) || (j != size - 1)) && ((i != 0) || (j != size - 2))) {
                    int* private_sol = (int*)calloc(size, sizeof(int));
                    memcpy(private_sol, sol, size * sizeof(int));
                    reverse(size, private_sol, i, j);
                    
                    float new_eval = evaluation_solution(size, private_sol, dist);
#pragma omp critical
                    {
                        if (new_eval < eval) {
                            eval = new_eval;
                            memcpy(sol, private_sol, size * sizeof(int));
                        }
                    }
                    free(private_sol);
                }
            }
        }
    } else {
        _2opt_sequential(size, sol, dist, &eval);
    }
#else
    _2opt_sequential(size, sol, dist, &eval);
#endif


    printf("Initial solution ");
    print_solution(size, sol, eval);

    /* initialize best solution with initial solution */
    for (i = 0; i < size; i++)
        best_solution[i] = sol[i];
    *best_eval = eval;

    free(visited);
    free(sol);
    return eval;
}

float evaluation_solution(int size, int* sol, float* dist) {
    float eval = 0;
    int i;
    int start, end;
    for (i = 0; i < size; i++) {
        start = sol[i];
        end = sol[(i + 1) % size];
        eval += dist[start * size + end];
    }
    return eval;
}

void print_solution(int size, int* sol, float eval) {
    int i;
    printf("(%.3f): ", eval);
    for (i = 0; i < size; i++)
        printf("%d ", sol[i]);
    printf("\n");
}

bool build_solution(int size, float* dist, int* next_town, int* best_solution, float* best_eval, configuration_t config) {
    int i;
    int* solution = (int *)calloc(size, sizeof(int));

    int idx = 0;
    int villeCour = 0;

    for (idx = 0; idx < size; idx++) {

        solution[idx] = villeCour;

        /* Test si le cycle est hamiltonien */
        for (i = 0; i < idx; i++) {
            if (solution[i] == villeCour) {
                return false;
            }
        }
        /* Recherche de la ville suivante */
        villeCour = next_town[villeCour];
    }

    float eval = evaluation_solution(size, solution, dist);
    bool found_best_solution = false;

#ifdef OPENMP
#pragma omp critical
#endif
    {

        if (*best_eval < 0 || eval < *best_eval) {
            *best_eval = eval;
            for (i = 0; i < size; i++)
            best_solution[i] = solution[i];
            
            if (config.is_verbose) {
                printf("New best solution: ");
                print_solution(size, solution, *best_eval);
                found_best_solution = true;
            }
        }
    }
    free(solution);
    return found_best_solution;
}