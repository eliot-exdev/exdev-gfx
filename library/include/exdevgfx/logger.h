/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#ifndef EXDEVGFX2_LOGGER_H
#define EXDEVGFX2_LOGGER_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#if EXDEVGFX2_LOG_LEVEL == 1
#define log_debug_fmt(format, ...) printf("[DEBUG] "format"\n", __VA_ARGS__)
#define log_debug(msg) printf("[DEBUG] "msg"\n")

#define log_info_fmt(format, ...) printf("[INFO] "format"\n", __VA_ARGS__)
#define log_info(msg) printf("[INFO] "msg"\n")

#define log_warning_fmt(format, ...) printf("[WARNING] "format"\n", __VA_ARGS__)
#define log_warning(msg) printf("[WARNING] "msg"\n")

#elif EXDEVGFX2_LOG_LEVEL == 2
#define log_debug_fmt(format, ...)
#define log_debug(msg)

#define log_info_fmt(format, ...) printf("[INFO] "format"\n", __VA_ARGS__)
#define log_info(msg) printf("[INFO] "msg"\n")

#define log_warning_fmt(format, ...) printf("[WARNING] "format"\n", __VA_ARGS__)
#define log_warning(msg) printf("[WARNING] "msg"\n")

#else
#define log_debug_fmt(format, ...)
#define log_debug(msg)

#define log_info_fmt(format, ...)
#define log_info(msg)

#define log_warning_fmt(format, ...) printf("[WARNING] "format"\n", __VA_ARGS__)
#define log_warning(msg) printf("[WARNING] "msg"\n")
#endif

#ifdef __cplusplus
}
#endif

#endif //EXDEVGFX2_LOGGER_H
