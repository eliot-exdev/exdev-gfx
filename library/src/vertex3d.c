/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#include "exdevgfx/vertex3d.h"
#include "exdevgfx/logger.h"

#ifndef EXDEV_FP_MATH
#include <math.h>
#endif

void vertex3d_init(Vertex3d_t v) {
    v[0] = 0;
    v[1] = 0;
    v[2] = 0;
}

void vertex3d_set(Vertex3d_t v, EXDEV_FLOAT x, EXDEV_FLOAT y, EXDEV_FLOAT z) {
    v[0] = x;
    v[1] = y;
    v[2] = z;
}

void vertex3d_copy(const Vertex3d_t src, Vertex3d_t dst) {
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
}

void vertex3d_mul(const Vertex3d_t src, const EXDEV_FLOAT f, Vertex3d_t res) {
#ifdef EXDEV_FP_MATH
    res[0] = exdev_fp_mul(src[0], f);
    res[1] = exdev_fp_mul(src[1], f);
    res[2] = exdev_fp_mul(src[2], f);
#else
    res[0] = src[0] * f;
    res[1] = src[1] * f;
    res[2] = src[2] * f;
#endif
}

void vertex3d_div(const Vertex3d_t src, const EXDEV_FLOAT f, Vertex3d_t res) {
#ifdef EXDEV_FP_MATH
    res[0] = exdev_fp_div(src[0], f);
    res[1] = exdev_fp_div(src[1], f);
    res[2] = exdev_fp_div(src[2], f);
#else
    res[0] = src[0] / f;
    res[1] = src[1] / f;
    res[2] = src[2] / f;
#endif
}

void vertex3d_add(const Vertex3d_t a, const Vertex3d_t b, Vertex3d_t res) {
#ifdef EXDEV_FP_MATH
    res[0] = exdev_fp_add(a[0], b[0]);
    res[1] = exdev_fp_add(a[1], b[1]);
    res[2] = exdev_fp_add(a[2], b[2]);
#else
    res[0] = a[0] + b[0];
    res[1] = a[1] + b[1];
    res[2] = a[2] + b[2];
#endif
}

void vertex3d_sub(const Vertex3d_t a, const Vertex3d_t b, Vertex3d_t res) {
#ifdef EXDEV_FP_MATH
    res[0] = exdev_fp_sub(a[0], b[0]);
    res[1] = exdev_fp_sub(a[1], b[1]);
    res[2] = exdev_fp_sub(a[2], b[2]);
#else
    res[0] = a[0] + b[0];
    res[1] = a[1] + b[1];
    res[2] = a[2] + b[2];
#endif
}

void vertex3d_cross(const Vertex3d_t a, const Vertex3d_t b, Vertex3d_t res) {
#ifdef EXDEV_FP_MATH
    res[0] = exdev_fp_sub(exdev_fp_mul(a[1], b[2]), exdev_fp_mul(a[2], b[1]));
    res[1] = exdev_fp_sub(exdev_fp_mul(a[2], b[0]), exdev_fp_mul(a[0], b[2]));
    res[2] = exdev_fp_sub(exdev_fp_mul(a[0], b[1]), exdev_fp_mul(a[1], b[0]));
#else
    res[0] = a[1] * b[2] - a[2] * b[1];
    res[1] = a[2] * b[0] - a[0] * b[2];
    res[2] = a[0] * b[1] - a[1] * b[0];
#endif
}

EXDEV_FLOAT vertex3d_dot(const Vertex3d_t a, const Vertex3d_t b) {
#ifdef EXDEV_FP_MATH
    return exdev_fp_add(exdev_fp_add(exdev_fp_mul(a[0], b[0]), exdev_fp_mul(a[1], b[1])), exdev_fp_mul(a[2], b[2]));
#else
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
#endif
}

EXDEV_FLOAT vertex3d_norm(const Vertex3d_t src) {
#ifdef EXDEV_FP_MATH
    return exdev_fp_sqrt(vertex3d_dot(src, src));
#else
    return sqrt(src[0] * src[0] + src[1] * src[1] + src[2] * src[2]);
#endif
}

EXDEV_FLOAT vertex3d_angle(const Vertex3d_t a, const Vertex3d_t b) {
#ifdef EXDEV_FP_MATH
    // TODO: implement me!
    (void) a;
    (void) b;
    log_warning("vertex3d_angle() not implemented yet!");
    return 0;
#else
    const float angle = acos(vertex3d_dot(a, b) / (vertex3d_norm(a) * vertex3d_norm(b)));
    if (isnan(angle) || isinf(angle)) {
        return 0.0f;
    }
    return angle;
#endif
}

EXDEV_FLOAT vertex3d_distance(const Vertex3d_t a, const Vertex3d_t b) {
    Vertex3d_t tmp;
    vertex3d_sub(a, b, tmp);
    return vertex3d_norm(tmp);
}

void vertex3d_normalize(const Vertex3d_t src, Vertex3d_t dst) {
    const EXDEV_FLOAT f = vertex3d_norm(src);
#ifdef EXDEV_FP_MATH
    dst[0] = exdev_fp_div(src[0], f);
    dst[1] = exdev_fp_div(src[1], f);
    dst[2] = exdev_fp_div(src[2], f);
#else
    dst[0] = src[0] / f;
    dst[1] = src[1] / f;
    dst[2] = src[2] / f;
#endif
}
