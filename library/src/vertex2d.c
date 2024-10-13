/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#include <exdevgfx/vertex2d.h>

#include <math.h>

void vertex2d_init(Vertex2d_t v) {
    v[0] = 0;
    v[1] = 0;
}

void vertex2d_set(Vertex2d_t v, float x, float y) {
    v[0] = x;
    v[1] = y;
}

void vertex2d_copy(const Vertex2d_t src, Vertex2d_t dst) {
    dst[0] = src[0];
    dst[1] = src[1];
}

float vertex2d_cross(const Vertex2d_t a, const Vertex2d_t b) {
    return a[0] * b[1] - a[1] * b[0];
}

void vertex2d_rotate(const Vertex2d_t v, Vertex2d_t d, const float a) {
    const float s = sin(a);
    const float c = cos(a);
    d[0] = v[0] * c - v[1] * s;
    d[1] = v[1] * c + v[0] * s;
}
