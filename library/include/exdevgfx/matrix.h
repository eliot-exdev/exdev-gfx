/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#ifndef EXDEVGFX2_MATRIX_H
#define EXDEVGFX2_MATRIX_H

#include <exdevgfx/vertex3d.h>

typedef float Matrix_t[4][4];

#define MATRIX_DEFAULT(name) Matrix_t name = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}}

void matrix_init(Matrix_t m);

void matrix_copy(const Matrix_t src, Matrix_t dst);

void matrix_mul_matrix(const Matrix_t a, const Matrix_t b, Matrix_t dst);

void matrix_mul_vector(const Matrix_t m, const Vertex3d_t vec, Vertex3d_t dst);

void matrix_rotate(const Matrix_t src, const Vertex3d_t vec, Matrix_t dst);

void matrix_rotateX(const Matrix_t src, float rx, Matrix_t dst);

void matrix_rotateY(const Matrix_t src, float ry, Matrix_t dst);

void matrix_rotateZ(const Matrix_t src, float rz, Matrix_t dst);

void matrix_translate(const Matrix_t src, const Vertex3d_t vec, Matrix_t dst);

void matrix_scale(const Matrix_t src, const Vertex3d_t vec, Matrix_t dst);

#endif
