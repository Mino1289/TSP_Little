#ifndef __UTIL_H__
#define __UTIL_H__

#include <dirent.h>
#include <libgen.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "configuration.h"


void readTSPFile(char *filename, int size, float** coords);

bool path_to_file_exists(char *path);

bool directory_exists(char *path);

void compute_matrix(int size, float** coord, float* d);

int find_nearest_unvisited(int current_town, int size, bool* visited, float* dist);

float initial_solution(int size, float* dist, int* best_solution, float *best_eval);

float evaluation_solution(int size, int* sol, float* dist);

void print_solution(int size, int* sol, float eval);

bool build_solution(int size, float* dist, int* next_town, int* best_solution, float* best_eval, configuration_t config);

#endif