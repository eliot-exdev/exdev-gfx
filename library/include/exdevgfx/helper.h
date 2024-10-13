/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#ifndef EXDEVGFX2_HELPER_H
#define EXDEVGFX2_HELPER_H

#include <stdio.h>

#define TIMESTAMP long

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

TIMESTAMP now();

int normalize_int(int i, int s);

float normalize_float(float f, int s);

#endif
