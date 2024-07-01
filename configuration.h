#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include <ctype.h>
#include <getopt.h>
#include <stdint.h>

#include "util.h"


typedef struct {
    char filename[STR_MAX_LEN];
    uint8_t number_of_cities;
    bool parallel;
    uint8_t number_of_threads;
    uint8_t number_of_tasks_per_thread;
    bool is_verbose;
} configuration_t;

configuration_t *make_configuration(configuration_t *base_configuration, char *argv[], int argc);
configuration_t *read_cfg_file(configuration_t *base_configuration, char *path_to_cfg_file);
void display_configuration(configuration_t *configuration);
bool is_configuration_valid(configuration_t *configuration);

#endif