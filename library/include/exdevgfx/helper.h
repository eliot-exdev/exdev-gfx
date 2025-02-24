/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#ifndef EXDEVGFX2_HELPER_H
#define EXDEVGFX2_HELPER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <exdevgfx/types.h>

#include <stdio.h>

EXDEV_FLOAT deg_to_rad(EXDEV_FLOAT deg);

EXDEV_FLOAT rad_to_deg(EXDEV_FLOAT rad);

#define min(a, b) (a < b ? a : b)

#define max(a, b) (a > b ? a : b)

int read_line(FILE *fp, char *line, size_t size);

void swap_bytes_int(int *i);

EXDEV_TIMESTAMP now();

static inline int normalize_int(int i, const int s) {
    i = i % s;
    return i < 0 ? (s + i) : i;
}

EXDEV_FLOAT normalize_float(EXDEV_FLOAT f, EXDEV_FLOAT d);

#ifdef __cplusplus
}
#endif

#endif
