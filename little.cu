/**
* Projec : gtsp (voyageur de commerce)
*
* Date   : 07/04/2014
* Author : Olivier Grunder
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

// #define LITTLEPLUS
// #ifdef LITTLEPLUS
// #warning "LITTLEPLUS is defined"
// #else
// #warning "LITTLEPLUS is not defined"
// #endif

#define BIG_VALUE 1e9
#define NBR_TOWNS 10

/* Distance matrix */
float dist[NBR_TOWNS][NBR_TOWNS];

/* next_town[i] = next town after town i */
int next_town[NBR_TOWNS];

/* no comment */
int best_solution[NBR_TOWNS];
float best_eval = -1.0;


/**
* Berlin52 :
*  6 towns : (2608.25) -> Best solution (2315.15): 0 1 2 3 5 4
* 10 towns : (3278.83) -> Best solution (2826.50): 0 1 6 2 7 8 9 3 5 4
* 15 towns : (5075.43) -> Best solution (4967.300): 0 13 12 10 11 3 5 4 14 9 8 7 2 6 1
* 20 towns : (5637.21) -> Best solution (5270.860): 0 17 2 16 6 1 19 15 13 12 10 11 3 5 4 14 9 8 7 18
* 25 towns : (5773.28) -> Best solution (5460.939): 0 21 17 20 1 6 16 2 18 7 8 9 14 4 23 5 3 24 11 10 12 13 15 19 22
* 30 towns : (6875.88) -> Best solution ? 
* 35 towns : (6888.63) -> Best solution ?
* 40 towns : (7000.24) -> Best solution ?
* 45 towns : (7549.86) -> Best solution ?
* 50 towns : (8804.23) -> Best solution ?
* 52 towns : (8980.91) -> Best solution ?
* A280 :
* 280 towns : (4148.110) -> Best solution ?
* eil76:
* 76 towns : (711.993) -> Best solution ?
* KroA100 :
* 100 towns : (26856.389) -> Best solution ?
*/

// Berlin52
float coord[NBR_TOWNS][2] =
{
    {565.0, 575.0},
    {25.0, 185.0},
    {345.0, 750.0},
    {945.0, 685.0},
    {845.0, 655.0},
    {880.0, 660.0},
    {25.0, 230.0},
    {525.0, 1000.0},
    {580.0, 1175.0},
    {650.0, 1130.0},
    // {1605.0, 620.0}, 
    // {1220.0, 580.0},
    // {1465.0, 200.0},
    // {1530.0, 5.0},
    // {845.0, 680.0},
    // {725.0, 370.0},
    // {145.0, 665.0},
    // {415.0, 635.0},
    // {510.0, 875.0},  
    // {560.0, 365.0},
    // {300.0, 465.0},
    // {520.0, 585.0},
    // {480.0, 415.0},
    // {835.0, 625.0},
    // {975.0, 580.0},
    // {1215.0, 245.0},
    // {1320.0, 315.0},
    // {1250.0, 400.0},
    // {660.0, 180.0},
    // {410.0, 250.0},
    // {420.0, 555.0},
    // {575.0, 665.0},
    // {1150.0, 1160.0},
    // {700.0, 580.0},
    // {685.0, 595.0},
    // {685.0, 610.0},
    // {770.0, 610.0},
    // {795.0, 645.0},
    // {720.0, 635.0},
    // {760.0, 650.0},
    // {475.0, 960.0},
    // {95.0, 260.0},
    // {875.0, 920.0},
    // {700.0, 500.0},
    // {555.0, 815.0},
    // {830.0, 485.0},
    // {1170.0, 65.0},
    // {830.0, 610.0},
    // {605.0, 625.0},
    // {595.0, 360.0},
    // {1340.0, 725.0},
    // {1740.0, 245.0},
};



/**
* print a matrix
*/
void print_matrix(float d[NBR_TOWNS][NBR_TOWNS]) {
    int i, j;
    for (i = 0; i < NBR_TOWNS; i++) {
        printf("%d\t", i + 1);
        for (j = 0; j < NBR_TOWNS; j++) {
            printf("%.3f ", d[i][j]);
        }
        printf("\n");
    }
}



/**
* compute distance matrix
*/
void compute_matrix(float d[NBR_TOWNS][NBR_TOWNS]) {
    int i, j;
    for (i = 0; i < NBR_TOWNS; i++) {
        float xi = coord[i][0];
        float yi = coord[i][1];

        for (j = 0; j < NBR_TOWNS; j++) {
            if (i == j) {
                d[i][j] = -1;
                continue;
            }
            float xj = coord[j][0];
            float yj = coord[j][1];
            d[i][j] = sqrt((xi - xj) * (xi - xj) + (yi - yj) * (yi - yj));
        }
    }
}

/**
* print a solution
*/
void print_solution(int* sol, float eval) {
    int i;
    printf("(%.3f): ", eval);
    for (i = 0; i < NBR_TOWNS; i++)
        printf("%d ", sol[i]);
    printf("\n");
}


/**
* evaluation of a solution
*/
float evaluation_solution(int* sol) {
    float eval = 0;
    int i;
    int start, end;
    for (i = 0; i < NBR_TOWNS; i++) {
        start = sol[i];
        end = sol[(i + 1) % NBR_TOWNS];
        eval += dist[start][end];
    }

    return eval;
}

int find_nearest_unvisited(int current_town, bool visited[NBR_TOWNS]) {
    int nearest_town = -1;
    float min_distance = -1;

    for (int j = 0; j < NBR_TOWNS; j++) {
        if (!visited[j]) {
            float distance = dist[current_town][j];
            if (min_distance < 0 || distance < min_distance) {
                min_distance = distance;
                nearest_town = j;
            }
        }
    }
    return nearest_town;
}


/**
* initial solution
*/
float initial_solution() {
    /* solution of the nearest neighbour */
    int sol[NBR_TOWNS];
    bool visited[NBR_TOWNS] = { false }; // Initialize all towns as unvisited
    float eval = 0.0;

    sol[0] = 0;
    visited[0] = true;

    for (int i = 1; i < NBR_TOWNS; i++) {
        int current_town = sol[i - 1];
        int next_town = find_nearest_unvisited(current_town, visited);

        sol[i] = next_town;
        visited[next_town] = true;
    }

    eval = evaluation_solution(sol);
    printf("Initial solution ");
    print_solution(sol, eval);

    /* initialize best solution with initial solution */
    for (int i = 0; i < NBR_TOWNS; i++)
        best_solution[i] = sol[i];
    best_eval = eval;

    return eval;
}




/**
*  Build final solution
*/
void build_solution() {
    int i, solution[NBR_TOWNS];

    int indiceCour = 0;
    int villeCour = 0;

    for (indiceCour = 0; indiceCour < NBR_TOWNS; indiceCour++) {

        solution[indiceCour] = villeCour;

        /* Test si le cycle est hamiltonien */
        for (i = 0; i < indiceCour; i++) {
            if (solution[i] == villeCour) {
                // printf ("cycle non hamiltonien\n");
                return;
            }
        }
        /* Recherche de la ville suivante */
        villeCour = next_town[villeCour];
    }

    float eval = evaluation_solution(solution);

    if (best_eval < 0 || eval < best_eval) {
        best_eval = eval;
        for (i = 0; i < NBR_TOWNS; i++)
            best_solution[i] = solution[i];

        printf("New best solution: ");
        print_solution(solution, best_eval);
    }
    return;
}



/* Check if adding an edge creates a sub-tour */
bool creates_subtour(int start, int end) {
    int tmp = next_town[start];
    int i = 0;

    while (tmp != end && tmp != -1 && i < NBR_TOWNS) {
        i++;
        tmp = next_town[tmp];
        // if (tmp == end)
        //     return true;
    }

    if (tmp == start && i < NBR_TOWNS) {
        return true;
    }
    return false;
}

__global__ void min_rows(float d[NBR_TOWNS*NBR_TOWNS], float eval_node) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int j;

    float minl = -1;
    for (j = 0; j < NBR_TOWNS; j++) {
        float val = d[i * NBR_TOWNS + j];
        if (minl < 0 || (val >= 0 && val < minl)) {
            minl = val;
        }
    }
    if (minl >= 0) {
        eval_node += minl;
        for (j = 0; j < NBR_TOWNS; j++) {
            if (d[i * NBR_TOWNS + j] >= 0) {
                d[i * NBR_TOWNS + j] -= minl;
            }
        }
    }
}

__global__ void min_col(float d[NBR_TOWNS * NBR_TOWNS], float eval_node) {
    int j = blockIdx.x * blockDim.x + threadIdx.x;
    int i;

    float minc = -1;
    for (i = 0; i < NBR_TOWNS; i++) {
        float val = d[i * NBR_TOWNS + j];
        if (minc < 0 || (val >= 0 && val < minc)) {
            minc = val;
        }
    }
    if (minc >= 0) {
        eval_node += minc;
        for (i = 0; i < NBR_TOWNS; i++) {
            if (d[i * NBR_TOWNS + j] >= 0) {
                d[i * NBR_TOWNS + j] -= minc;
            }
        }
    }
    
}

__global__ void find_zeros(float d[NBR_TOWNS * NBR_TOWNS], float* penaltys) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int j = blockIdx.y * blockDim.y + threadIdx.y;

    int k;

    if (d[i * NBR_TOWNS + j] == 0) {
        float min_row = -1;
        float min_col = -1;

        for (k = 0; k < NBR_TOWNS; k++) {
            float valik = d[i * NBR_TOWNS + k];
            float valkj = d[k * NBR_TOWNS + j];

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
        penaltys[i * NBR_TOWNS + j] = min_row + min_col;
    }
}


/**
*  Little Algorithm
*/
void little_algorithm(float d0[NBR_TOWNS][NBR_TOWNS], int iteration, float eval_node_parent) {
    if (iteration == NBR_TOWNS) {
        build_solution();
        return;
    }

    /* Do the modification on a copy of the distance matrix */
    // float d[NBR_TOWNS][NBR_TOWNS];
    float *d;
    cudaMallocManaged(&d, NBR_TOWNS * NBR_TOWNS * sizeof(float));
    memcpy(d, d0, NBR_TOWNS * NBR_TOWNS * sizeof(float));

    float eval_node_child = eval_node_parent;

    dim3 threads_per_block (4, 4, 1); // A 16 x 1 block threads
    dim3 number_of_blocks ((NBR_TOWNS / threads_per_block.x) + 1, 1, 1);


    min_rows<<< number_of_blocks, threads_per_block >>>(d, eval_node_child);
    cudaDeviceSynchronize(); // Wait for the GPU to finish before proceeding
    min_col<<< number_of_blocks, threads_per_block >>>(d, eval_node_child);
    cudaDeviceSynchronize(); // Wait for the GPU to finish before proceeding


    /* Cut : stop the exploration of this node */
    if (best_eval >= 0 && eval_node_child >= best_eval) {
        return;
    }

    /* row and column of the zero with the max penalty */
    int izero = -1, jzero = -1;
    float max_penalty = -1;
    int i,j,k;


    float *penaltys;
    cudaMallocManaged(&penaltys, NBR_TOWNS * NBR_TOWNS * sizeof(float));

    dim3 threads_per_block2 (4, 4, 1); // A 16 x 16 block threads
    dim3 number_of_blocks2 ((NBR_TOWNS / threads_per_block2.x) + 1, (NBR_TOWNS / threads_per_block2.y) + 1, 1);

    find_zeros<<< number_of_blocks2, threads_per_block2 >>>(d, penaltys);
    cudaDeviceSynchronize(); // Wait for the GPU to finish before proceeding

    max_penalty = -1;
    for (i = 0; i < NBR_TOWNS; i++) {
        for (j = 0; j < NBR_TOWNS; j++) {
            if (d[i * NBR_TOWNS + j] == 0) {
                if (penaltys[i * NBR_TOWNS + j] > max_penalty) {
                    max_penalty = penaltys[i * NBR_TOWNS + j];
                    izero = i;
                    jzero = j;
                }
            }
        
        }
    }
    // printf("debug\n");

    if (izero < 0 || jzero < 0) {
        return;
    }
    next_town[izero] = jzero;

    /* Do the modification on a copy of the distance matrix */
    float d2[NBR_TOWNS][NBR_TOWNS];
    memcpy(d2, d, NBR_TOWNS * NBR_TOWNS * sizeof(float));

    for (k = 0; k < NBR_TOWNS; k++) {
        d2[izero][k] = -1;
        d2[k][jzero] = -1;
    }
    d2[jzero][izero] = -1; // si B -> A alors A -> B interdit

    /* Explore left child node according to given choice */

    little_algorithm(d2, iteration + 1, eval_node_child);

    /* Do the modification on a copy of the distance matrix */
    memcpy(d2, d, NBR_TOWNS * NBR_TOWNS * sizeof(float));

    // next_town[izero] = -1;
    d2[izero][jzero] = -1;

    little_algorithm(d2, iteration, eval_node_child + max_penalty);

    cudaFree(d);
}


int main(int argc, char* argv[]) {
    printf("Little Algorithm\n");
#ifdef LITTLEPLUS
    printf("LITTLEPLUS is defined\n");
#else
    printf("LITTLEPLUS is not defined\n");
#endif
    printf("Number of towns: %d\n", NBR_TOWNS);
    for (int i = 0; i < NBR_TOWNS; i++)
        next_town[i] = -1;
    
    best_eval = -1;

    compute_matrix(dist);

    /* initial solution */
    float initial_value = initial_solution();
    (void)initial_value;

    /** Little : uncomment when needed
    */
    int iteration = 0;
    float lowerbound = 0.0;

    time_t start = time(NULL);
    little_algorithm(dist, iteration, lowerbound);
    time_t end = time(NULL);


    printf("Best solution:");
    print_solution(best_solution, best_eval);

    printf("Time: %f seconds\n", difftime(end, start));
    return 0;
}
