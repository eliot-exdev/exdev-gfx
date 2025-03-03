/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#ifndef EXDEVGFX2_HELPER_H
#define EXDEVGFX2_HELPER_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define EXDEV_TIMESTAMP long
typedef EXDEV_TIMESTAMP exdev_timestamp_t;

float deg_to_rad(float deg);

float rad_to_deg(float rad);

#define min(a, b) (a < b ? a : b)

#define max(a, b) (a > b ? a : b)

int read_line(FILE *fp, char *line, size_t size);

//float fast_sin(float f);
//
//float fast_cos(float f);
//
//float fast_sqrt(float f);
//
//float fast_acos(float f);

void swap_bytes_int(int *i);

exdev_timestamp_t now();

static inline int normalize_int(int i, const int s) {
    i = i % s;
    return i < 0 ? (s + i) : i;
}

float normalize_float(float f, float d);

#ifdef __cplusplus
}
#endif

#endif
