/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#ifndef EXDEVGFX2_VERTEX3D_H
#define EXDEVGFX2_VERTEX3D_H

#ifdef __cplusplus
extern "C" {
#endif

typedef float Vertex3d_t[3];

void vertex3d_init(Vertex3d_t v);

void vertex3d_set(Vertex3d_t v, float x, float y, float z);

void vertex3d_copy(const Vertex3d_t src, Vertex3d_t dst);

void vertex3d_mul(const Vertex3d_t src, float f, Vertex3d_t res);

void vertex3d_div(const Vertex3d_t src, float f, Vertex3d_t res);

void vertex3d_add(const Vertex3d_t a, const Vertex3d_t b, Vertex3d_t res);

void vertex3d_sub(const Vertex3d_t a, const Vertex3d_t b, Vertex3d_t res);

void vertex3d_cross(const Vertex3d_t a, const Vertex3d_t b, Vertex3d_t res);

float vertex3d_dot(const Vertex3d_t a, const Vertex3d_t b);

float vertex3d_norm(const Vertex3d_t src);

float vertex3d_angle(const Vertex3d_t a, const Vertex3d_t b);

float vertex3d_distance(const Vertex3d_t a, const Vertex3d_t b);

void vertex3d_normalize(const Vertex3d_t src, Vertex3d_t dst);

#ifdef __cplusplus
}
#endif

#endif
