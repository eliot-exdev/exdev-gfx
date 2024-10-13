/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#ifndef EXDEVGFX2_ARGS_H
#define EXDEVGFX2_ARGS_H

#ifdef __cplusplus
extern "C" {
#endif

const char *args_find_short_option(int argc,  char **argv, char so);

const char *args_find_long_option(int argc,  char **argv, const char *lo);

const char *args_find_option(int argc,  char **argv, char so,const char *lo);

const char *args_get_short_option_parameter(int argc,  char **argv, char o);

const char *args_get_long_option_parameter(int argc,  char **argv, const char *o);

const char *args_get_option_parameter(int argc,  char **argv, char so,const char *lo);

#ifdef __cplusplus
}
#endif

#endif //EXDEVGFX2_ARGS_H
