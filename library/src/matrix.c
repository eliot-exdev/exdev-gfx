#include "exdevgfx/matrix.h"
//#include "exdevgfx/helper.h"
#include <math.h>

void matrix_init(Matrix_t m) {
    m[0][0] = 1;
    m[0][1] = 0;
    m[0][2] = 0;
    m[0][3] = 0;

    m[1][0] = 0;
    m[1][1] = 1;
    m[1][2] = 0;
    m[1][3] = 0;

    m[2][0] = 0;
    m[2][1] = 0;
    m[2][2] = 1;
    m[2][3] = 0;

    m[3][0] = 0;
    m[3][1] = 0;
    m[3][2] = 0;
    m[3][3] = 1;
}

void matrix_copy(const Matrix_t src, Matrix_t dst) {
    dst[0][0] = src[0][0];
    dst[0][1] = src[0][1];
    dst[0][2] = src[0][2];
    dst[0][3] = src[0][3];

    dst[1][0] = src[1][0];
    dst[1][1] = src[1][1];
    dst[1][2] = src[1][2];
    dst[1][3] = src[1][3];

    dst[2][0] = src[2][0];
    dst[2][1] = src[2][1];
    dst[2][2] = src[2][2];
    dst[2][3] = src[2][3];

    dst[3][0] = src[3][0];
    dst[3][1] = src[3][1];
    dst[3][2] = src[3][2];
    dst[3][3] = src[3][3];
}

void matrix_mul_matrix(const Matrix_t a, const Matrix_t b, Matrix_t dst) {
    Matrix_t tm;

    for (int row = 0; row < 4; ++row) {
        for (int column = 0; column < 4; ++column) {
            tm[row][column] =
                    b[row][0] * a[0][column]
                    + b[row][1] * a[1][column]
                    + b[row][2] * a[2][column]
                    + b[row][3] * a[3][column];
        }
    }

    matrix_copy(tm, dst);
}

void matrix_mul_vector(const Matrix_t m, const Vertex3d_t vec, Vertex3d_t dst) {
    dst[0] = vec[0] * m[0][0] + vec[1] * m[0][1] + vec[2] * m[0][2] + m[0][3];
    dst[1] = vec[0] * m[1][0] + vec[1] * m[1][1] + vec[2] * m[1][2] + m[1][3];
    dst[2] = vec[0] * m[2][0] + vec[1] * m[2][1] + vec[2] * m[2][2] + m[2][3];
}

void matrix_rotate(const Matrix_t src, const Vertex3d_t vec, Matrix_t dst) {
    matrix_rotateX(src, vec[0], dst);
    matrix_rotateY(src, vec[1], dst);
    matrix_rotateZ(src, vec[2], dst);
}

void matrix_rotateX(const Matrix_t src, const float rx, Matrix_t dst) {
    const float c = cos(rx);
    const float s = sin(rx);

    MATRIX_DEFAULT(rm);
    rm[1][1] = c;
    rm[1][2] = -s;
    rm[2][1] = s;
    rm[2][2] = c;

    matrix_mul_matrix(src, rm, dst);
}

void matrix_rotateY(const Matrix_t src, const float ry, Matrix_t dst) {
    const float c = cos(ry);
    const float s = sin(ry);

    MATRIX_DEFAULT(rm);
    rm[0][0] = c;
    rm[0][2] = s;
    rm[2][0] = -s;
    rm[2][2] = c;

    matrix_mul_matrix(src, rm, dst);
}

void matrix_rotateZ(const Matrix_t src, const float rz, Matrix_t dst) {
    const float c = cos(rz);
    const float s = sin(rz);

    MATRIX_DEFAULT(rm);
    rm[0][0] = c;
    rm[0][1] = -s;
    rm[1][0] = s;
    rm[1][1] = c;

    matrix_mul_matrix(src, rm, dst);
}

void matrix_translate(const Matrix_t src, const Vertex3d_t vec, Matrix_t dst) {
    MATRIX_DEFAULT(tm);
    tm[0][3] = vec[0];
    tm[1][3] = vec[1];
    tm[2][3] = vec[2];

    matrix_mul_matrix(src, tm, dst);
}

void matrix_scale(const Matrix_t src, const Vertex3d_t vec, Matrix_t dst) {
    MATRIX_DEFAULT(sm);
    sm[0][0] = vec[0];
    sm[1][1] = vec[1];
    sm[2][2] = vec[2];

    matrix_mul_matrix(src, sm, dst);
}
