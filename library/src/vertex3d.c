#include "exdevgfx/vertex3d.h"
#include "exdevgfx/helper.h"

#include <math.h>

void vertex3d_init(Vertex3d_t v) {
    v[0] = 0;
    v[1] = 0;
    v[2] = 0;
}

void vertex3d_set(Vertex3d_t v, float x, float y, float z) {
    v[0] = x;
    v[1] = y;
    v[2] = z;
}

void vertex3d_copy(const Vertex3d_t src, Vertex3d_t dst) {
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
}

void vertex3d_mul(const Vertex3d_t src, const float f, Vertex3d_t res) {
    res[0] = src[0] * f;
    res[1] = src[1] * f;
    res[2] = src[2] * f;
}

void vertex3d_div(const Vertex3d_t src, const float f, Vertex3d_t res) {
    res[0] = src[0] / f;
    res[1] = src[1] / f;
    res[2] = src[2] / f;
}

void vertex3d_add(const Vertex3d_t a, const Vertex3d_t b, Vertex3d_t res) {
    res[0] = a[0] + b[0];
    res[1] = a[1] + b[1];
    res[2] = a[2] + b[2];
}

void vertex3d_sub(const Vertex3d_t a, const Vertex3d_t b, Vertex3d_t res) {
    res[0] = a[0] - b[0];
    res[1] = a[1] - b[1];
    res[2] = a[2] - b[2];
}

void vertex3d_cross(const Vertex3d_t a, const Vertex3d_t b, Vertex3d_t res) {
    res[0] = a[1] * b[2] - a[2] * b[1];
    res[1] = a[2] * b[0] - a[0] * b[2];
    res[2] = a[0] * b[1] - a[1] * b[0];
}

float vertex3d_dot(const Vertex3d_t a, const Vertex3d_t b) {
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

float vertex3d_norm(const Vertex3d_t src) {
    return sqrt(src[0] * src[0] + src[1] * src[1] + src[2] * src[2]);
}

float vertex3d_angle(const Vertex3d_t a, const Vertex3d_t b) {
    const float angle = acos(vertex3d_dot(a, b) / (vertex3d_norm(a) * vertex3d_norm(b)));
    if (isnan(angle)||isinf(angle)) {
        return 0.0f;
    }
    return angle;
}

float vertex3d_distance(const Vertex3d_t a, const Vertex3d_t b) {
    Vertex3d_t tmp;
    vertex3d_sub(a, b, tmp);
    return vertex3d_norm(tmp);
}

void vertex3d_normalize(const Vertex3d_t src, Vertex3d_t dst) {
    const float f = vertex3d_norm(src);
    dst[0] = src[0] / f;
    dst[1] = src[1] / f;
    dst[2] = src[2] / f;
}
