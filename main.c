#include <time.h>
#include <omp.h>

#include "util.h"
#include "configuration.h"
#include "little.h"



/**
 * Berlin52 :
 *  6 towns : (2608.25) -> Best solution (2315.15): 0 1 2 3 5 4
 * 10 towns : (3278.83) -> Best solution (2826.50): 0 1 6 2 7 8 9 3 5 4
 * 15 towns : (5075.43) -> Best solution (4967.300): 0 13 12 10 11 3 5 4 14 9 8 7 2 6 1
 * 20 towns : (5637.21) -> Best solution (5270.860): 0 17 2 16 6 1 19 15 13 12 10 11 3 5 4 14 9 8 7 18
 * 25 towns : (5773.28) -> Best solution (5460.939): 0 21 17 20 1 6 16 2 18 7 8 9 14 4 23 5 3 24 11 10 12 13 15 19 22
 * 30 towns : (6875.88) -> Best solution (6146.645): 0 18 7 8 9 14 4 23 5 3 24 11 10 13 12 26 27 25 15 28 19 22 29 1 6 20 16 2 17 21
 * 35 towns : (6888.63) -> Best solution (6557.121): 0 31 34 33 23 4 14 5 3 24 11 27 26 25 13 12 10 32 9 8 7 18 2 16 20 6 1 29 28 15 19 22 30 17 21
 * 40 towns : (7000.24) -> Best solution (6652.626): 0 21 17 30 22 19 15 28 29 1 6 20 16 2 18 7 8 9 32 10 12 13 25 26 27 11 24 3 5 14 4 23 37 36 39 38 33 34 35 31
 * 45 towns : (7549.86) -> Best solution (6887.371): 0 31 44 18 40 7 8 9 42 32 10 12 13 25 26 27 11 24 3 5 14 4 23 37 36 39 38 35 34 33 43 15 28 19 22 29 1 6 41 20 16 2 17 30 21
 * 50 towns : (8804.23) -> Best solution (7256.569): 0 48 31 44 18 40 7 8 9 42 32 10 12 13 46 25 26 27 11 24 3 5 14 4 23 47 37 36 39 38 35 34 33 43 45 15 28 49 19 22 29 1 6 41 20 16 2 17 30 21
 * 52 towns : (8980.91) -> Best solution (7544.366): 0 48 31 44 18 40 7 8 9 42 32 50 10 51 13 12 46 25 26 27 11 24 3 5 14 4 23 47 37 36 39 38 35 34 33 43 45 15 28 49 19 22 29 1 6 41 20 16 2 17 30 21
 * A280 :
 * 280 towns : (3148.110) -> Best solution ?
 * eil76:
 * 25 towns : (352.980) -> Best solution (294.016): 0 22 15 2 23 17 24 8 9 10 13 18 7 6 11 16 5 1 3 12 14 19 4 20 21
 * 50 towns : (417.722) -> Best solution (390.602): 0 21 27 20 46 35 36 19 14 12 26 44 28 4 47 29 1 5 32 15 2 43 31 39 11 16 25 3 33 45 7 6 34 18 13 10 37 9 30 38 8 24 49 17 23 48 22 40 41 42
 * 76 towns : (711.993) -> Best solution (544.369): 0 72 61 21 63 41 42 40 55 22 48 23 17 49 24 54 30 9 37 64 65 10 58 13 52 6 34 7 18 53 12 56 14 4 36 19 69 59 70 68 35 46 20 60 27 73 1 29 47 28 44 26 51 45 33 66 25 75 74 3 67 5 50 16 39 11 57 71 38 8 31 43 2 15 62 32
 * KroA100 :
 * 25 towns : (13600.286) -> Best solution (11780.631): 0 24 1 12 4 13 2 8 6 11 19 22 14 16 10 20 9 23 17 21 15 3 18 5 7
 * 50 towns : (22642.936) -> Best solution (16461.459): 0 46 27 24 39 1 43 49 12 32 36 4 38 29 47 40 13 2 42 45 28 33 8 6 19 11 26 34 22 44 31 10 14 16 20 9 35 37 23 17 21 15 18 3 25 30 41 7 48 5
 * 75 towns : (28513.000) -> Best solution ?
 * 100 towns : (26856.389) -> Best solution ?
 * 
 * fr_cities (119):
 * 10 towns : (2602.658) -> Best solution:(2602.658): 0 9 7 2 4 1 6 3 8 5 
 * 15 towns : (2874.698) -> Best solution:(2846.292): 0 14 10 5 8 3 6 1 11 4 13 2 7 12 9 
 * 20 towns : (3044.063) -> Best solution:(3015.657): 0 14 10 17 5 8 3 6 19 1 11 4 16 13 2 18 15 7 12 9 
 * 25 towns : (3755.859) -> Best solution:(3694.875): 0 9 12 7 15 21 13 2 18 16 4 11 1 20 19 6 3 8 5 23 10 17 24 22 14 
 * 30 towns : (4444.908) -> Best solution:(3874.178): 0 25 9 12 28 7 15 21 13 2 18 26 16 4 11 1 20 19 6 3 8 27 24 22 17 5 10 23 14 29 
 * 35 towns : (4562.645) -> Best solution:(4152.320): 0 29 33 14 23 10 5 17 22 32 24 27 8 3 30 6 19 20 1 11 4 16 26 18 2 13 21 15 31 7 28 12 9 25 34 
 * 40 towns : (4416.683) -> Best solution:(4215.666): 0 35 34 25 9 12 28 39 7 38 31 15 21 13 2 18 26 16 4 11 1 20 19 6 30 3 8 27 24 32 22 17 5 23 10 36 14 33 37 29 
 * 45 towns : (4752.446) -> Best solution:(4250.397): 0 43 44 35 34 25 9 40 41 12 28 39 7 38 31 15 21 13 2 18 26 16 4 11 1 20 19 6 30 3 8 27 24 32 22 17 5 23 10 36 14 33 37 42 29 
 * 50 towns : (4966.106)
 * 52 towns : (5225.797)
 * 55 towns : (5493.776)
 * 60 towns : (5485.392)
 * 70 towns : (5936.410)
 * 76 towns : (6547.050)
 * 80 towns : (6154.652)
 * 90 towns : (6171.761)
 * 100 towns : (6437.714)
 * 119 towns : (6857.121) -> Best solution ?
 */

/** comp seq / omp
 * eil76 50 towns seq: 37s vs omp: N2: 54s N4: 26s N8: 21s N16: 13s N24: 15s N32: 10s
 * eil76 55 towns seq: 447s vs omp: N2: N4: 689s N8: 58s N16: 145s
 * eil76 60 towns seq:  vs omp: N8: 
 * 
 * kroA100 40 towns test case seq: 130s vs omp:N8: 246s N16: 44s N24: 76s
 * a280 35 towns test case seq: 73s vs omp: N4: 146s N8: 126s N16: 94s N24: 76s N32: 84s
 * 
 * NO THREADS DEPTH LIMIT
 * berlin52 30 towns test case seq: 553s vs omp: N4: N8: 413s N16: 245s N24: 326s N32: 362s
 */

#ifndef OPENMP

#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 1
#endif

double omp_get_wtime() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}
#endif

int main(int argc, char *argv[]) {

    configuration_t config = {
        .filename = "",
        .number_of_cities = 0,
        .is_verbose = false,
        .init = false
    };

    make_configuration(&config, argv, argc);

    if (!is_configuration_valid(&config)) {
        printf("\nUsage: %s -r <filename> -n <number_of_cities> -t <num_threads> [-v] [-i] -f <config-file>\n", argv[0]);
        display_configuration(&config);
        printf("\nExiting\n");
        return -1;
    }

#ifdef OPENMP
#ifdef NUM_THREADS
    omp_set_num_threads(NUM_THREADS);
#else
    omp_set_num_threads(config.num_threads);
#endif
#endif

    if (config.is_verbose) {
        display_configuration(&config);
    }


    int size = config.number_of_cities;
    float* coordsList;
    readTSPFile(config.filename, size, &coordsList);

    float** coord = (float **)malloc(size * sizeof(float *));
    float* dist = (float *)malloc(size * size * sizeof(float ));
    float* distNoModif = (float *)malloc(size * size * sizeof(float ));

    int* next_town = (int *)malloc(size * sizeof(int));

    for (int i = 0; i < size; i++) {
        coord[i] = (float *)malloc(2 * sizeof(float));
        coord[i][0] = coordsList[i * 2];
        coord[i][1] = coordsList[i * 2 + 1];

        next_town[i] = -1;
    }

    int* best_solution = (int *)malloc(size * sizeof(int));
    float best_eval = 0.0;

    double time;
    time = -omp_get_wtime();
    compute_matrix(size, coord, dist);
    time += omp_get_wtime();
    if (config.is_verbose) {
        printf("Distance matrix computed in %f seconds.\n", time);
    }
    for (int i = 0; i < size * size; i++) {
        distNoModif[i] = dist[i];
    }

    time = -omp_get_wtime();
    initial_solution(size, dist, best_solution, &best_eval);
    time += omp_get_wtime();
    if (config.is_verbose) {
        printf("Initial solution computed in %f seconds.\n", time);
    }

    if (config.init) {
        return 0;
    }

    int iteration = 0;
    float lowerbound = 0.0;

    time = -omp_get_wtime();
#ifdef OPENMP
#pragma omp parallel
#endif
    {
#ifdef OPENMP
#pragma omp single
#endif
    little_algorithm(size, dist, distNoModif, iteration, lowerbound, best_solution, &best_eval, next_town, config);
    }
    time += omp_get_wtime();

    // fprintf(f, "}\n");
    // fclose(f);


    printf("Best solution:");
    print_solution(size, best_solution, best_eval);

    printf("Time: %f seconds\n", time);
    // printf("Number of visited nodes: %d\n", nbit-NBR_TOWNS+2);

    free(best_solution);
    // free(next_town);
    for (int i = 0; i < size; i++) {
        free(coord[i]);
    }
    free(coord);
    // free(dist);
    free(distNoModif);
    free(coordsList);

    return 0;
}
