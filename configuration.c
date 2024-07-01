#include "configuration.h"

configuration_t *make_configuration(configuration_t *base_configuration, char *argv[], int argc) {
    struct option my_opts[] = {
        {.name="verbose",.has_arg=0,.flag=0,.val='v'},
        {.name="parallel",.has_arg=1,.flag=0,.val='p'},
        {.name="read",.has_arg=1,.flag=0,.val='r'},
        {.name="threads",.has_arg=1,.flag=0,.val='t'},
        {.name="tasks",.has_arg=1,.flag=0,.val='k'},
        {.name="cities",.has_arg=1,.flag=0,.val='n'},
        {.name="config-file",.has_arg=1,.flag=0,.val='f'},
        {.name=0,.has_arg=0,.flag=0,.val=0},
    };
    int opt;

    while ((opt = getopt_long(argc, argv, "vpr:t:k:n:f:", my_opts, NULL)) != EOF) {
        switch (opt) {
            case 'v':
                base_configuration->is_verbose = true;
                break;
            
            case 'p':
                base_configuration->parallel = true;
                break;

            case 'r':
                strncpy(base_configuration->filename, optarg, STR_MAX_LEN);
                break;
            
            case 't':
                base_configuration->number_of_threads = strtoul(optarg, NULL, 10);
                break;
            
            case 'k':
                base_configuration->number_of_tasks_per_thread = strtoul(optarg, NULL, 10);
                break;

            case 'n':
                base_configuration->number_of_cities = strtoul(optarg, NULL, 10);
                break;
            case 'f':
                base_configuration = read_cfg_file(base_configuration, optarg);
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


configuration_t *read_cfg_file(configuration_t *base_configuration, char *path_to_cfg_file) {
    if (base_configuration == NULL || !path_to_file_exists(path_to_cfg_file)) {
        return NULL;
    }
    FILE *cfg_file = fopen(path_to_cfg_file, "r");
    if (cfg_file == NULL) {
        return base_configuration;
    }
    
    while (!feof(cfg_file)) {
        char line[STR_MAX_LEN], key[STR_MAX_LEN], value[STR_MAX_LEN];
        char* c = fgets(line, STR_MAX_LEN, cfg_file);
        (void)c;
        char* tmpl = skip_spaces(line);
        tmpl = get_word(tmpl, key);
        tmpl = check_equal(tmpl);
        tmpl = skip_spaces(tmpl);
        tmpl = get_word(tmpl, value);

        if (strcmp(key, "filename") == 0) {
            strncpy(base_configuration->filename, value, STR_MAX_LEN);
        } else if (strcmp(key, "cities") == 0) {
            base_configuration->number_of_cities = strtoul(value, NULL, 10);
        } else if (strcmp(key, "threads") == 0) {
            base_configuration->number_of_threads = strtoul(value, NULL, 10);
        } else if (strcmp(key, "tasks") == 0) {
            base_configuration->number_of_tasks_per_thread = strtoul(value, NULL, 10);
        } else if (strcmp(key, "verbose") == 0) {
            if (strcmp(value, "true") == 0 || strcmp(value, "1") == 0 || strcmp(value, "yes") == 0) {
                base_configuration->is_verbose = true;
            } else {
                base_configuration->is_verbose = false;
            }
        } else if (strcmp(key, "parallel")) {
            if (strcmp(value, "true") == 0 || strcmp(value, "1") == 0 || strcmp(value, "yes") == 0) {
                base_configuration->parallel = true;
            } else {
                base_configuration->parallel = false;
            }
        } else {
            printf("Unknown key: %s\n", key);
        }
    }
    fclose(cfg_file);
    return base_configuration;
}

void display_configuration(configuration_t *configuration) {
    printf("Configuration:\n");
    printf("filename: %s\n", configuration->filename);
    printf("number_of_cities: %d\n", configuration->number_of_cities);
    printf("parallel: %s\n", configuration->parallel ? "true" : "false");
    printf("number_of_threads: %d\n", configuration->number_of_threads);
    printf("number_of_tasks_per_thread: %d\n", configuration->number_of_tasks_per_thread);
    printf("is_verbose: %s\n", configuration->is_verbose ? "true" : "false");
}

bool is_configuration_valid(configuration_t *configuration) {
    if (configuration->number_of_cities <= 0 ||
        configuration->filename[0] == '\0' ||
        !path_to_file_exists(configuration->filename) ||
        (configuration->parallel &&
        configuration->number_of_threads <= 0 &&
        configuration->number_of_tasks_per_thread <= 0)) {
            return false;
        }
    return true;
}