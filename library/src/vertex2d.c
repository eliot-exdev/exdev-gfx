/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#include <exdevgfx/vertex2d.h>

#ifndef EXDEV_FP_MATH
#include <math.h>
#endif

void vertex2d_init(Vertex2d_t v) {
    v[0] = 0;
    v[1] = 0;
}

void vertex2d_set(Vertex2d_t v, const FLOAT x, const FLOAT y) {
    v[0] = x;
    v[1] = y;
}

void vertex2d_copy(const Vertex2d_t src, Vertex2d_t dst) {
    dst[0] = src[0];
    dst[1] = src[1];
}

FLOAT vertex2d_cross(const Vertex2d_t a, const Vertex2d_t b) {
#ifdef EXDEV_FP_MATH
    return exdev_fp_sub(exdev_fp_mul(a[0], b[1]), exdev_fp_mul(a[1], b[0]));
#else
    return a[0] * b[1] - a[1] * b[0];
#endif
}

void vertex2d_rotate(const Vertex2d_t src, Vertex2d_t dst, const FLOAT a) {
#ifdef EXDEV_FP_MATH
    const FLOAT s = exdev_fp_sin(a);
    const FLOAT c = exdev_fp_cos(a);
    dst[0] = exdev_fp_sub(exdev_fp_mul(src[0], c), exdev_fp_mul(src[1], s));
    dst[1] = exdev_fp_sub(exdev_fp_mul(src[1], c), exdev_fp_mul(src[0], s));
#else
    const FLOAT s = sin(a);
    const FLOAT c = cos(a);
    dst[0] = src[0] * c - src[1] * s;
    dst[1] = src[1] * c + src[0] * s;
#endif
}
