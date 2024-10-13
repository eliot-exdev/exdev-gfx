/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#ifndef EXDEVGFX2_VERTEX2D_H
#define EXDEVGFX2_VERTEX2D_H

typedef float Vertex2d_t[2];

void vertex2d_init(Vertex2d_t v);

void vertex2d_set(Vertex2d_t v, float x, float y);

void vertex2d_copy(const Vertex2d_t src, Vertex2d_t dst);

float vertex2d_cross(const Vertex2d_t a, const Vertex2d_t b);

void vertex2d_rotate(const Vertex2d_t v, Vertex2d_t d, float a);

#endif //EXDEVGFX2_VERTEX2D_H
