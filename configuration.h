#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include <ctype.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define STR_MAX_LEN 1024


typedef struct {
    char filename[STR_MAX_LEN];
    uint16_t number_of_cities;
    bool is_verbose;
} configuration_t;

configuration_t *make_configuration(configuration_t *base_configuration, char *argv[], int argc);
configuration_t *read_cfg_file(configuration_t *base_configuration, char *path_to_cfg_file);
void display_configuration(configuration_t *configuration);
bool is_configuration_valid(configuration_t *configuration);

#endif