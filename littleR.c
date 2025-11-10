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
#include <omp.h>

#define BIG_VALUE 1e9
#define NBR_TOWNS 20

#ifdef OPENMP
#ifndef NUM_THREADS
#define NUM_THREADS 2
#endif
#define NUM_TASKS_PER_THREAD ((NBR_TOWNS/NUM_THREADS))
#endif 


 /* Distance matrix */
float dist[NBR_TOWNS][NBR_TOWNS];

/* next_town[i] = next town after town i */
int next_town[NBR_TOWNS];

typedef struct CycleResult {
    int index;
    int value;
    bool cycle_detected;
} CycleResult;

// typedef struct ValueIndex {
//     float value;
//     int index;
// } ValueIndex;

// ValueIndex evolution[NBR_TOWNS*NBR_TOWNS];

/* no comment */
int best_solution[NBR_TOWNS];
float best_eval = -1.0;
int nbit = 0;
// int nbevol = 0;

FILE *f = NULL;


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
 * 280 towns : (4148.110) -> Best solution ?
 * eil76:
 * 25 towns : (352.980) -> Best solution (294.016): 0 22 15 2 23 17 24 8 9 10 13 18 7 6 11 16 5 1 3 12 14 19 4 20 21
 * 50 towns : (417.722) -> Best solution (390.602): 0 21 27 20 46 35 36 19 14 12 26 44 28 4 47 29 1 5 32 15 2 43 31 39 11 16 25 3 33 45 7 6 34 18 13 10 37 9 30 38 8 24 49 17 23 48 22 40 41 42
 * 76 towns : (711.993) -> Best solution (544.369): 0 72 61 21 63 41 42 40 55 22 48 23 17 49 24 54 30 9 37 64 65 10 58 13 52 6 34 7 18 53 12 56 14 4 36 19 69 59 70 68 35 46 20 60 27 73 1 29 47 28 44 26 51 45 33 66 25 75 74 3 67 5 50 16 39 11 57 71 38 8 31 43 2 15 62 32
 * KroA100 :
 * 25 towns : (13600.286) -> Best solution (11780.631): 0 24 1 12 4 13 2 8 6 11 19 22 14 16 10 20 9 23 17 21 15 3 18 5 7
 * 50 towns : (22642.936) -> Best solution ?
 * 75 towns : (28513.000) -> Best solution ?
 * 100 towns : (26856.389) -> Best solution ?
 */

float coord[NBR_TOWNS][2] =
{
    // Berlin52
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
    {1605.0, 620.0}, 
    {1220.0, 580.0},
    {1465.0, 200.0},
    {1530.0, 5.0},
    {845.0, 680.0},
    {725.0, 370.0},
    {145.0, 665.0},
    {415.0, 635.0},
    {510.0, 875.0},  
    {560.0, 365.0},
    {300.0, 465.0},
    {520.0, 585.0},
    {480.0, 415.0},
    {835.0, 625.0},
    {975.0, 580.0},
    {1215.0, 245.0},
    {1320.0, 315.0},
    {1250.0, 400.0},
    {660.0, 180.0},
    {410.0, 250.0},
    {420.0, 555.0},
    {575.0, 665.0},
    {1150.0, 1160.0},
    {700.0, 580.0},
    {685.0, 595.0},
    {685.0, 610.0},
    {770.0, 610.0},
    {795.0, 645.0},
    {720.0, 635.0},
    {760.0, 650.0},
    {475.0, 960.0},
    {95.0, 260.0},
    {875.0, 920.0},
    {700.0, 500.0},
    {555.0, 815.0},
    {830.0, 485.0},
    {1170.0, 65.0},
    {830.0, 610.0},
    {605.0, 625.0},
    {595.0, 360.0},
    {1340.0, 725.0},
    {1740.0, 245.0},
    //eil76
    // {22, 22},
    // {36, 26},
    // {21, 45},
    // {45, 35},
    // {55, 20},
    // {33, 34},
    // {50, 50},
    // {55, 45},
    // {26, 59},
    // {40, 66},
    // {55, 65},
    // {35, 51},
    // {62, 35},
    // {62, 57},
    // {62, 24},
    // {21, 36},
    // {33, 44},
    // {9, 56},
    // {62, 48},
    // {66, 14},
    // {44, 13},
    // {26, 13},
    // {11, 28},
    // {7, 43},
    // {17, 64},
    // {41, 46},
    // {55, 34},
    // {35, 16},
    // {52, 26},
    // {43, 26},
    // {31, 76},
    // {22, 53},
    // {26, 29},
    // {50, 40},
    // {55, 50},
    // {54, 10},
    // {60, 15},
    // {47, 66},
    // {30, 60},
    // {30, 50},
    // {12, 17},
    // {15, 14},
    // {16, 19},
    // {21, 48},
    // {50, 30},
    // {51, 42},
    // {50, 15},
    // {48, 21},
    // {12, 38},
    // {15, 56},
    // {29, 39},
    // {54, 38},
    // {55, 57},
    // {67, 41},
    // {10, 70},
    // {6, 25},
    // {65, 27},
    // {40, 60},
    // {70, 64},
    // {64, 4},
    // {36, 6},
    // {30, 20},
    // {20, 30},
    // {15, 5},
    // {50, 70},
    // {57, 72},
    // {45, 42},
    // {38, 33},
    // {50, 4},
    // {66, 8},
    // {59, 5},
    // {35, 60},
    // {27, 24},
    // {40, 20},
    // {40, 37},
    // {40, 40},
    //a100
    // {1380, 939},
    // {2848, 96},
    // {3510, 1671},
    // {457, 334},
    // {3888, 666},
    // {984, 965},
    // {2721, 1482},
    // {1286, 525},
    // {2716, 1432},
    // {738, 1325},
    // {1251, 1832},
    // {2728, 1698},
    // {3815, 169},
    // {3683, 1533},
    // {1247, 1945},
    // {123, 862},
    // {1234, 1946},
    // {252, 1240},
    // {611, 673},
    // {2576, 1676},
    // {928, 1700},
    // {53, 857},
    // {1807, 1711},
    // {274, 1420},
    // {2574, 946},
    // {178, 24},
    // {2678, 1825},
    // {1795, 962},
    // {3384, 1498},
    // {3520, 1079},
    // {1256, 61},
    // {1424, 1728},
    // {3913, 192},
    // {3085, 1528},
    // {2573, 1969},
    // {463, 1670},
    // {3875, 598},
    // {298, 1513},
    // {3479, 821},
    // {2542, 236},
    // {3955, 1743},
    // {1323, 280},
    // {3447, 1830},
    // {2936, 337},
    // {1621, 1830},
    // {3373, 1646},
    // {1393, 1368},
    // {3874, 1318},
    // {938, 955},
    // {3022, 474},
    // {2482, 1183},
    // {3854, 923},
    // {376, 825},
    // {2519, 135},
    // {2945, 1622},
    // {953, 268},
    // {2628, 1479},
    // {2097, 981},
    // {890, 1846},
    // {2139, 1806},
    // {2421, 1007},
    // {2290, 1810},
    // {1115, 1052},
    // {2588, 302},
    // {327, 265},
    // {241, 341},
    // {1917, 687},
    // {2991, 792},
    // {2573, 599},
    // {19, 674},
    // {3911, 1673},
    // {872, 1559},
    // {2863, 558},
    // {929, 1766},
    // {839, 620},
    // {3893, 102},
    // {2178, 1619},
    // {3822, 899},
    // {378, 1048},
    // {1178, 100},
    // {2599, 901},
    // {3416, 143},
    // {2961, 1605},
    // {611, 1384},
    // {3113, 885},
    // {2597, 1830},
    // {2586, 1286},
    // {161, 906},
    // {1429, 134},
    // {742, 1025},
    // {1625, 1651},
    // {1187, 706},
    // {1787, 1009},
    // {22, 987},
    // {3640, 43},
    // {3756, 882},
    // {776, 392},
    // {1724, 1642},
    // {198, 1810},
    // {3950, 1558},
    // A280
    // {288, 149},
    // {288, 129},
    // {270, 133},
    // {256, 141},
    // {256, 157},
    // {246, 157},
    // {236, 169},
    // {228, 169},
    // {228, 161},
    // {220, 169},
    // {212, 169},
    // {204, 169},
    // {196, 169},
    // {188, 169},
    // {196, 161},
    // {188, 145},
    // {172, 145},
    // {164, 145},
    // {156, 145},
    // {148, 145},
    // {140, 145},
    // {148, 169},
    // {164, 169},
    // {172, 169},
    // {156, 169},
    // {140, 169},
    // {132, 169},
    // {124, 169},
    // {116, 161},
    // {104, 153},
    // {104, 161},
    // {104, 169},
    // { 90, 165},
    // { 80, 157},
    // { 64, 157},
    // { 64, 165},
    // { 56, 169},
    // { 56, 161},
    // { 56, 153},
    // { 56, 145},
    // { 56, 137},
    // { 56, 129},
    // { 56, 121},
    // { 40, 121},
    // { 40, 129},
    // { 40, 137},
    // { 40, 145},
    // { 40, 153},
    // { 40, 161},
    // { 40, 169},
    // { 32, 169},
    // { 32, 161},
    // { 32, 153},
    // { 32, 145},
    // { 32, 137},
    // { 32, 129},
    // { 32, 121},
    // { 32, 113},
    // { 40, 113},
    // { 56, 113},
    // { 56, 105},
    // { 48,  99},
    // { 40,  99},
    // { 32,  97},
    // { 32,  89},
    // { 24,  89},
    // { 16,  97},
    // { 16, 109},
    // {  8, 109},
    // {  8,  97},
    // {  8,  89},
    // {  8,  81},
    // {  8,  73},
    // {  8,  65},
    // {  8,  57},
    // { 16,  57},
    // {  8,  49},
    // {  8,  41},
    // { 24,  45},
    // { 32,  41},
    // { 32,  49},
    // { 32,  57},
    // { 32,  65},
    // { 32,  73},
    // { 32,  81},
    // { 40,  83},
    // { 40,  73},
    // { 40,  63},
    // { 40,  51},
    // { 44,  43},
    // { 44,  35},
    // { 44,  27},
    // { 32,  25},
    // { 24,  25},
    // { 16,  25},
    // { 16,  17},
    // { 24,  17},
    // { 32,  17},
    // { 44,  11},
    // { 56,   9},
    // { 56,  17},
    // { 56,  25},
    // { 56,  33},
    // { 56,  41},
    // { 64,  41},
    // { 72,  41},
    // { 72,  49},
    // { 56,  49},
    // { 48,  51},
    // { 56,  57},
    // { 56,  65},
    // { 48,  63},
    // { 48,  73},
    // { 56,  73},
    // { 56,  81},
    // { 48,  83},
    // { 56,  89},
    // { 56,  97},
    // {104,  97},
    // {104, 105},
    // {104, 113},
    // {104, 121},
    // {104, 129},
    // {104, 137},
    // {104, 145},
    // {116, 145},
    // {124, 145},
    // {132, 145},
    // {132, 137},
    // {140, 137},
    // {148, 137},
    // {156, 137},
    // {164, 137},
    // {172, 125},
    // {172, 117},
    // {172, 109},
    // {172, 101},
    // {172,  93},
    // {172,  85},
    // {180,  85},
    // {180,  77},
    // {180,  69},
    // {180,  61},
    // {180,  53},
    // {172,  53},
    // {172,  61},
    // {172,  69},
    // {172,  77},
    // {164,  81},
    // {148,  85},
    // {124,  85},
    // {124,  93},
    // {124, 109},
    // {124, 125},
    // {124, 117},
    // {124, 101},
    // {104,  89},
    // {104,  81},
    // {104,  73},
    // {104,  65},
    // {104,  49},
    // {104,  41},
    // {104,  33},
    // {104,  25},
    // {104,  17},
    // { 92,   9},
    // { 80,   9},
    // { 72,   9},
    // { 64,  21},
    // { 72,  25},
    // { 80,  25},
    // { 80,  25},
    // { 80,  41},
    // { 88,  49},
    // {104,  57},
    // {124,  69},
    // {124,  77},
    // {132,  81},
    // {140,  65},
    // {132,  61},
    // {124,  61},
    // {124,  53},
    // {124,  45},
    // {124,  37},
    // {124,  29},
    // {132,  21},
    // {124,  21},
    // {120,   9},
    // {128,   9},
    // {136,   9},
    // {148,   9},
    // {162,   9},
    // {156,  25},
    // {172,  21},
    // {180,  21},
    // {180,  29},
    // {172,  29},
    // {172,  37},
    // {172,  45},
    // {180,  45},
    // {180,  37},
    // {188,  41},
    // {196,  49},
    // {204,  57},
    // {212,  65},
    // {220,  73},
    // {228,  69},
    // {228,  77},
    // {236,  77},
    // {236,  69},
    // {236,  61},
    // {228,  61},
    // {228,  53},
    // {236,  53},
    // {236,  45},
    // {228,  45},
    // {228,  37},
    // {236,  37},
    // {236,  29},
    // {228,  29},
    // {228,  21},
    // {236,  21},
    // {252,  21},
    // {260,  29},
    // {260,  37},
    // {260,  45},
    // {260,  53},
    // {260,  61},
    // {260,  69},
    // {260,  77},
    // {276,  77},
    // {276,  69},
    // {276,  61},
    // {276,  53},
    // {284,  53},
    // {284,  61},
    // {284,  69},
    // {284,  77},
    // {284,  85},
    // {284,  93},
    // {284, 101},
    // {288, 109},
    // {280, 109},
    // {276, 101},
    // {276,  93},
    // {276,  85},
    // {268,  97},
    // {260, 109},
    // {252, 101},
    // {260,  93},
    // {260,  85},
    // {236,  85},
    // {228,  85},
    // {228,  93},
    // {236,  93},
    // {236, 101},
    // {228, 101},
    // {228, 109},
    // {228, 117},
    // {228, 125},
    // {220, 125},
    // {212, 117},
    // {204, 109},
    // {196, 101},
    // {188,  93},
    // {180,  93},
    // {180, 101},
    // {180, 109},
    // {180, 117},
    // {180, 125},
    // {196, 145},
    // {204, 145},
    // {212, 145},
    // {220, 145},
    // {228, 145},
    // {236, 145},
    // {246, 141},
    // {252, 125},
    // {260, 129},
    // {280, 133},
};



/**
 * print a matrix
 */
void print_matrix(float d[NBR_TOWNS][NBR_TOWNS]) {
    printf("\n");
    int i, j;
    for (i = 0; i < NBR_TOWNS; i++) {
        printf("%d\t", i + 1);
        for (j = 0; j < NBR_TOWNS; j++) {
            printf("%.0f ", d[i][j]);
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
float evaluation_solution(int* sol, float dist[NBR_TOWNS][NBR_TOWNS]) {
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

    eval = evaluation_solution(sol, dist);
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
bool build_solution(int idx, int depth) {
    int i, solution[NBR_TOWNS];

    int indiceCour = 0;
    int villeCour = 0;

    for (indiceCour = 0; indiceCour < NBR_TOWNS; indiceCour++) {

        solution[indiceCour] = villeCour;

        /* Test si le cycle est hamiltonien */
        for (i = 0; i < indiceCour; i++) {
            if (solution[i] == villeCour) {
                fprintf(f, "node%d [label=\"S%d (depth=%d)\\nNon-Hamiltonian\", color=red, style=filled, fillcolor=pink];\n", idx, idx, depth);
                return false;
            }
        }
        /* Recherche de la ville suivante */
        villeCour = next_town[villeCour];
    }

    float eval = evaluation_solution(solution, dist);

    if (best_eval < 0 || eval < best_eval) {
        best_eval = eval;
        for (i = 0; i < NBR_TOWNS; i++)
            best_solution[i] = solution[i];

        printf("New best solution: ");
        print_solution(solution, best_eval);
        fprintf(f, "node%d [label=\"S%d (depth=%d)\\n(%.2f)\\nBEST!\", color=limegreen, style=filled, fillcolor=lightgreen];\n", idx, idx, depth, eval);
        return true;
    }
    fprintf(f, "node%d [label=\"S%d (depth=%d)\\n(%.2f)\", color=lightblue, style=filled, fillcolor=lightcyan];\n", idx, idx, depth, eval);
    return false;
}

CycleResult detectCycles() {
    bool visited[NBR_TOWNS] = {false};
    bool stack[NBR_TOWNS] = {false};

    for (int start = 0; start < NBR_TOWNS; start++) {
        if (visited[start]) continue;

        int current = start;
        while (current != -1 && !visited[current]) {
            visited[current] = true;
            stack[current] = true;
            int next = next_town[current];

            if (next != -1) {
                if (!visited[next]) {
                    current = next;
                } else if (stack[next]) {
                    // Cycle detected
                    return (CycleResult) { .index = current, .value = next_town[current], .cycle_detected = true };
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
    return (CycleResult) { .index = -1, .value = -1, .cycle_detected = false };
}

bool createsSubTour(int start_index, int start_value) {
    bool visited[NBR_TOWNS] = {false};
    int current = start_index;
    
    while (current != -1 && !visited[current]) {
        visited[current] = true;
        int next = next_town[current];
        
        // Check if the current position forms a loop back to start_value
        if (next == start_value) {
            return true;
        }
        
        current = next;
    }
    
    // Check if adding start_value directly creates a new sub-tour
    if (next_town[start_index] == -1 && start_value != start_index && start_value == next_town[start_value]) {
        return true;
    }
    
    return false;
}


float min_rows(float d[NBR_TOWNS][NBR_TOWNS]) {
    float current_eval = 0.0;
    int i, j;

#ifdef OPENMP
#pragma omp parallel
#endif
    {
        float local_eval = 0.0;
#ifdef OPENMP
#pragma omp for private(i) schedule(dynamic, NUM_TASKS_PER_THREAD)
#endif
        for (i = 0; i < NBR_TOWNS; i++) {
            float minl = -1;
            for (j = 0; j < NBR_TOWNS; j++) {
                float val = d[i][j];
                if (minl < 0 || (val >= 0 && val < minl)) {
                    minl = val;
                }
            }
            if (minl >= 0) {
                local_eval += minl;
                for (j = 0; j < NBR_TOWNS; j++) {
                    if (d[i][j] >= 0) {
                        d[i][j] -= minl;
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

float min_col(float d[NBR_TOWNS][NBR_TOWNS]) {
    float current_eval = 0.0;
    int i, j;

#ifdef OPENMP
#pragma omp parallel 
#endif
    {
        float local_eval = 0.0;
#ifdef OPENMP
#pragma omp for private(j) schedule(dynamic, NUM_TASKS_PER_THREAD)
#endif
        for (j = 0; j < NBR_TOWNS; j++) {
            float minc = -1;
            for (i = 0; i < NBR_TOWNS; i++) {
                float val = d[i][j];
                if (minc < 0 || (val >= 0 && val < minc)) {
                    minc = val;
                }
            }
            if (minc >= 0) {
                local_eval += minc;
                for (i = 0; i < NBR_TOWNS; i++) {
                    if (d[i][j] >= 0) {
                        d[i][j] -= minc;
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
int little_algorithm(float d0[NBR_TOWNS][NBR_TOWNS], int iteration, float eval_node_parent, int depth) {
    int myIteration = nbit++;
    if (iteration == NBR_TOWNS) {
        bool best = build_solution(myIteration, depth);
        return myIteration;
    }

    CycleResult cycle = detectCycles();
    if (cycle.cycle_detected) {
        fprintf(f, "node%d [label=\"S%d (depth=%d)\\nCycle detected\", color=red, style=filled, fillcolor=pink];\n", myIteration, myIteration, depth);
        return myIteration;
    }

    /* Do the modification on a copy of the distance matrix */
    float d[NBR_TOWNS][NBR_TOWNS];
    memcpy(d, d0, NBR_TOWNS * NBR_TOWNS * sizeof(float));

    float eval_node_child = eval_node_parent;
 
    eval_node_child += min_rows(d);
    eval_node_child += min_col(d);

    /* Cut : stop the exploration of this node */
    if (best_eval >= 0 && eval_node_child >= best_eval) {
        fprintf(f, "node%d [label=\"S%d (depth=%d)\\n(%.2f)\\nCUT\", color=red, style=filled, fillcolor=lightcoral];\n", myIteration, myIteration, depth, eval_node_child);
        return myIteration;
    }

    /* row and column of the zero with the max penalty */
    int izero = -1, jzero = -1;
    float max_penalty = -1;
    int i,j,k;

#ifdef OPENMP
#pragma omp parallel for collapse(2) private(i, j) shared(max_penalty, izero, jzero) schedule(dynamic, NUM_TASKS_PER_THREAD)
#endif
    for (i = 0; i < NBR_TOWNS; i++) {
        for (j = 0; j < NBR_TOWNS; j++) {
            if (d[i][j] == 0 && !createsSubTour(i, j)) {
                float min_row = -1;
                float min_col = -1;
#ifdef OPENMP
// #pragma omp parallel for private(k) schedule(dynamic, NUM_TASKS_PER_THREAD)
#endif
                for (k = 0; k < NBR_TOWNS; k++) {
                    float valik = d[i][k];
                    float valkj = d[k][j];

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
        fprintf(f, "node%d [label=\"S%d (depth=%d)\\n(%.2f)\\nNo zero found\", color=orange, style=filled, fillcolor=lightyellow];\n", myIteration, myIteration, depth, eval_node_child);
        return myIteration;
    }
    fprintf(f, "node%d [label=\"S%d (depth=%d)\\n(%.2f)\\nPenalty: %.2f\", style=filled, fillcolor=lightgray];\n", myIteration, myIteration, depth, eval_node_child, max_penalty);

    next_town[izero] = jzero;
  
    /* Do the modification on a copy of the distance matrix */
    float d2[NBR_TOWNS][NBR_TOWNS];
    memcpy(d2, d, NBR_TOWNS * NBR_TOWNS * sizeof(float));

#ifdef OPENMP
#pragma omp parallel for private(k) schedule(dynamic, NUM_TASKS_PER_THREAD)
#endif
    for (k = 0; k < NBR_TOWNS; k++) {
        d2[izero][k] = -1;
        d2[k][jzero] = -1;
    }
    d2[jzero][izero] = -1; // si B -> A alors A -> B interdit

    /* Explore left child node according to given choice */
    
    int choice = little_algorithm(d2, iteration + 1, eval_node_child, depth + 1);
    fprintf(f, "node%d -> node%d [label=\"%d → %d\", color=blue, penwidth=2];\n", myIteration, choice, izero, jzero);

    /* Do the modification on a copy of the distance matrix */
    memcpy(d2, d, NBR_TOWNS * NBR_TOWNS * sizeof(float));

    next_town[izero] = -1;
    d2[izero][jzero] = -1;

    int nochoice = little_algorithm(d2, iteration, eval_node_child, depth + 1);
    fprintf(f, "node%d -> node%d [label=\"%d ✗ %d\", color=red, style=dashed];\n", myIteration, nochoice, izero, jzero);

    return myIteration;
}


int main(int argc, char* argv[]) {
#ifdef OPENMP
    omp_set_num_threads(NUM_THREADS);
#endif
    printf("Little+ Algorithm\n");
#ifdef OPENMP
    printf("Running with %d threads.\n", NUM_THREADS);
    printf("Running with %d tasks per threads.\n", NUM_TASKS_PER_THREAD);
#endif
    printf("Number of towns: %d\n", NBR_TOWNS);
    for (int i = 0; i < NBR_TOWNS; i++)
        next_town[i] = -1;
    
    best_eval = -1;

    compute_matrix(dist);

    f = fopen("little.dot", "w");
    fprintf(f, "digraph G {\n");
    fprintf(f, "rankdir=TB;\n");
    fprintf(f, "node [shape=box, style=\"filled,rounded\", fontname=\"Arial\"];\n");
    fprintf(f, "edge [fontname=\"Arial\", fontsize=10];\n");
        

    /* initial solution */
    // float initial_value = initial_solution();
    // evolution[0] = (ValueIndex) { .value = initial_value, .index = 0 };

    /** Little : uncomment when needed
    */
    int iteration = 0;
    float lowerbound = 0.0;
    int initial_depth = 0;

    time_t start = time(NULL);
    (void)little_algorithm(dist, iteration, lowerbound, initial_depth);
    time_t end = time(NULL);
    fprintf(f, "}\n");
    fclose(f);


    printf("Best solution:");
    print_solution(best_solution, best_eval);

    printf("Time: %f seconds\n", difftime(end, start));
    printf("Number of visited nodes: %d\n", nbit-NBR_TOWNS+2);
    // printf("\nGraphViz file generated: little.dot\n");
    // printf("To visualize: dot -Tpng little.dot -o little.png\n");
    // printf("Or use: dot -Tsvg little.dot -o little.svg\n");

    // FILE *f = fopen("evolution.csv", "w");
    // fprintf(f, "index, value\n");
    // for (int i = 0; i < NBR_TOWNS*NBR_TOWNS; i++) {
    //     if (evolution[i].value > 0) {
    //         fprintf(f, "%d, %.2f\n", evolution[i].index, evolution[i].value);
    //     }
    // }
    // fclose(f);
    return 0;
}
