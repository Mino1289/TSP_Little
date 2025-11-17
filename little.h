#ifndef __LITTLE_H__
#define __LITTLE_H__

#include "util.h"

#define BIG_VALUE 1e9


bool detectCycles(int size, int* next_town);

bool createsSubTour(int size, int* next_town, int start_index, int start_value);

float min_rows(int size, float* d);

float min_cols(int size, float* d);

void little_algorithm(int size, float* dist, float* baseDist, int iteration, float eval_node_parent, int* best_solution, float* best_eval, int* next_town, configuration_t config);

#endif