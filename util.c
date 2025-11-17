#include "util.h"

void readTSPFile_coords(char *filename, int size, float **coords) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open file %s\n", filename);
        exit(1);
    }

    char line[128];
    int i = 0;

    // skip the first lines that are not coordinates
    while (fgets(line, sizeof(line), file) != NULL) {
        if (strncmp(line, "NODE_COORD_SECTION", 18) == 0) {
            break;
    }  
    }

    *coords = (float *)malloc((size) * (2) * sizeof(float));
    float* d = *coords;

    while (fgets(line, sizeof(line), file) != NULL && i <= size) {
        if (sscanf(line, "%d %f %f", &i, &d[0], &d[1]) == 3) {
            d += 2;
            i++;    
        }
    }

    fclose(file);
}

void readTSPFile_lowerMatrix(char *filename, int size, float *dists) {
// NAME: hk48
// TYPE: TSP
// COMMENT: 48-city problem (Held/Karp)
// DIMENSION: 48
// EDGE_WEIGHT_TYPE: EXPLICIT
// EDGE_WEIGHT_FORMAT: LOWER_DIAG_ROW 
// EDGE_WEIGHT_SECTION
// 0  273    0 1272  999    0  744  809 1519    0 ....
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open file %s\n", filename);
        exit(1);
    }

    char line[STR_MAX_LEN];

    // extract dimension
    int file_size = -1;
    while (fgets(line, sizeof(line), file) != NULL) {
        if (strncmp(line, "DIMENSION:", 10) == 0) {
            file_size = atoi(line + 10);
        }
    }

    if (file_size == -1) {
        fprintf(stderr, "Error: Could not find DIMENSION in file %s\n", filename);
        fclose(file);
        exit(1);
    }

    // On se place au début du fichier pour la suite de la lecture
    rewind(file);

    float* temp_dist_matrix = dists;
    if (file_size != size) {
        temp_dist_matrix = (float *)malloc(file_size * file_size * sizeof(float));
        if (temp_dist_matrix == NULL) {
            fprintf(stderr, "Error: Memory allocation failed for temp_dist_matrix\n");
            fclose(file);
            exit(1);
        }
    }

    // skip the nexts lines that are not coordinates
    while (fgets(line, sizeof(line), file) != NULL) {
        if (strncmp(line, "EDGE_WEIGHT_SECTION", 19) == 0) {
            break;
        }
    }

    // Read the lower diagonal row matrix
    for (int i = 0; i < file_size; i++) {
        for (int j = 0; j <= i; j++) {
            int value;
            if (fscanf(file, "%d", &value) == 1) {
                temp_dist_matrix[i * file_size + j] = (float)value;
                temp_dist_matrix[j * file_size + i] = (float)value; // Since it's a symmetric matrix
            }
        }
    }

    // If the requested size is different, crop the matrix
    if (file_size != size) {
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                if (i == j) {
                    dists[i * size + j] = -1;
                    continue;
                }
                dists[i * size + j] = temp_dist_matrix[i * file_size + j];
            }
        }
        free(temp_dist_matrix);
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
#pragma omp parallel for private(j) if(size >= 1000) schedule(dynamic, 10)
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
#pragma omp parallel for private(j) schedule(dynamic, 1)
        for (i = 1; i < size; i++) {
            for (j = i + 1; j < size; j++) {
                if ((j != size - 1) && (j != size - 2)) {
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