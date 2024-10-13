/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#include <exdevgfx/args.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int args_find_short_option_pos(const int argc,  char **argv, const char o) {
    char needle[3];
    needle[0] = '-';
    needle[1] = o;
    needle[2] = '\0';
    for (int i = 0; i < argc; ++i) {
        if (strcmp(argv[i], needle) == 0) {
            return i;
        }
    }
    return 0;
}

int args_find_long_option_pos(const int argc, char **argv, const char *o) {
    const size_t size = strlen(o) + 3;
    char *needle = (char *) malloc(size);
    sprintf(needle, "--%s", o);

    for (int i = 0; i < argc; ++i) {
        if (strcmp(argv[i], needle) == 0) {
            free(needle);
            return i;
        }
    }
    free(needle);
    return 0;
}

const char *args_find_short_option(const int argc, char **argv, const char o) {
    const int pos = args_find_short_option_pos(argc, argv, o);
    if (pos > 0) {
        return argv[pos];
    }
    return NULL;
}

const char *args_find_long_option(const int argc, char **argv, const char *o) {
    const int pos = args_find_long_option_pos(argc, argv, o);
    if (pos > 0) {
        return argv[pos];
    }
    return NULL;
}

const char *args_find_option(const int argc, char **argv,  char so, const char *lo) {
    const char *res = args_find_short_option(argc, argv, so);
    if (res)
        return res;
    return args_find_long_option(argc, argv, lo);
}

const char *args_get_short_option_parameter(int argc,  char **argv, char o) {
    const int pos = args_find_short_option_pos(argc, argv, o);
    if (pos > 0 && pos < argc - 1) {
        return argv[pos + 1];
    }
    return NULL;
}

const char *args_get_long_option_parameter(int argc,  char **argv, const char *o) {
    const int pos = args_find_long_option_pos(argc, argv, o);
    if (pos > 0 && pos < argc - 1) {
        return argv[pos + 1];
    }
    return NULL;
}

const char *args_get_option_parameter(int argc, char **argv, char so, const char *lo) {
    const char *res = args_get_short_option_parameter(argc, argv, so);
    if (res)
        return res;
    return args_get_long_option_parameter(argc, argv, lo);
}




