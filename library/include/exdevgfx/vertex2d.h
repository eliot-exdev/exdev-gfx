/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#ifndef EXDEVGFX2_VERTEX2D_H
#define EXDEVGFX2_VERTEX2D_H


#ifdef __cplusplus
extern "C" {
#endif

#include <exdevgfx/types.h>

typedef FLOAT Vertex2d_t[2];

void vertex2d_init(Vertex2d_t v);

void vertex2d_set(Vertex2d_t v, FLOAT x, FLOAT y);

void vertex2d_copy(const Vertex2d_t src, Vertex2d_t dst);

FLOAT vertex2d_cross(const Vertex2d_t a, const Vertex2d_t b);

void vertex2d_rotate(const Vertex2d_t src, Vertex2d_t dst, FLOAT a);

#ifdef __cplusplus
}
#endif

#endif //EXDEVGFX2_VERTEX2D_H
