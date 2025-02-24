/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#ifndef EXDEVGFX2_MATRIX_H
#define EXDEVGFX2_MATRIX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <exdevgfx/vertex3d.h>

typedef EXDEV_FLOAT Matrix_t[4][4];

#define MATRIX_DEFAULT(name) Matrix_t name; matrix_init(name);

void matrix_init(Matrix_t m);

void matrix_copy(const Matrix_t src, Matrix_t dst);

void matrix_mul_matrix(const Matrix_t a, const Matrix_t b, Matrix_t dst);

void matrix_mul_vector(const Matrix_t m, const Vertex3d_t vec, Vertex3d_t dst);

void matrix_rotate(const Matrix_t src, const Vertex3d_t vec, Matrix_t dst);

void matrix_rotateX(const Matrix_t src, EXDEV_FLOAT rx, Matrix_t dst);

void matrix_rotateY(const Matrix_t src, EXDEV_FLOAT ry, Matrix_t dst);

void matrix_rotateZ(const Matrix_t src, EXDEV_FLOAT rz, Matrix_t dst);

void matrix_translate(const Matrix_t src, const Vertex3d_t vec, Matrix_t dst);

void matrix_scale(const Matrix_t src, const Vertex3d_t vec, Matrix_t dst);

#ifdef __cplusplus
}
#endif

#endif
