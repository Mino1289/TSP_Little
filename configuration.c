#include "configuration.h"

configuration_t *make_configuration(configuration_t *base_configuration, char *argv[], int argc) {
    struct option my_opts[] = {
        {.name="verbose",.has_arg=0,.flag=0,.val='v'},
        {.name="init",.has_arg=0,.flag=0,.val='i'},
        {.name="read-file",.has_arg=1,.flag=0,.val='r'},
        {.name="cities",.has_arg=1,.flag=0,.val='n'},
        {.name=0,.has_arg=0,.flag=0,.val=0},
    };
    int opt;

    while ((opt = getopt_long(argc, argv, "vir:n:", my_opts, NULL)) != EOF) {
        switch (opt) {
            case 'v':
                base_configuration->is_verbose = true;
                break;
            
            case 'i':
                base_configuration->init = true;
                break;
            
            case 'r':
                strncpy(base_configuration->filename, optarg, STR_MAX_LEN);
                break;
            
            case 'n':
                base_configuration->number_of_cities = strtoul(optarg, NULL, 10);
                break;

            default:
                break;
        }
    }
    return base_configuration;
}

char *skip_spaces(char *str) {
    while (isspace(*str)) {
        str++;
    }
    return str;
}

char *check_equal(char *str) {
    str = skip_spaces(str);
    if (*str == '=') {
        ++str;
        str = skip_spaces(str);
        return str;
    } else {
        return NULL;
    }
}

char *get_word(char *source, char *target) {
    while (*source != '\0' && !isspace(*source)) {
        *target = *source;
        ++target;
        ++source;
    }
    *target = '\0';
    return source;
}


void display_configuration(configuration_t *configuration) {
    printf("Configuration:\n");
    printf("filename: %s\n", configuration->filename);
    printf("number_of_cities: %d\n", configuration->number_of_cities);
    printf("is_verbose: %s\n", configuration->is_verbose ? "true" : "false");
    printf("init_only: %s\n", configuration->init ? "true" : "false");
}

bool is_configuration_valid(configuration_t *configuration) {
    if (configuration->number_of_cities <= 0 ||
        configuration->filename[0] == '\0' ||
        !path_to_file_exists(configuration->filename)) {
            return false;
        }
    return true;
}