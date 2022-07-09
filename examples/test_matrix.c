#include <stdio.h>
#include <exdevgfx/matrix.h>
#include <exdevgfx/helper.h>

/*
 * compile for amiga:
 * vc -c99 -Iinclude src/vertex3d.c src/matrix.c src/helper.c examples/test_matrix.c -o test_matrix -lmsoft -lauto -lamiga -O2 -speed -final
 * vc -c99 -Iinclude src/vertex3d.c src/matrix.c src/helper.c examples/test_matrix.c -o test_matrix -fpu=68060 -lm060 -lauto -lamiga -O2 -speed -final
 */

int main() {
    Matrix_t m;
    Vertex3d_t v;

    printf("begin ...\n");

    matrix_init(m);
    vertex3d_set(v, 1, 0, 0);
    printf("vector before rotation=%f %f %f\n", v[0], v[1], v[2]);
    matrix_rotateZ(m, deg_to_rad(180), m);
    matrix_mul_vector(m, v, v);
    printf("vector after rotation of 180 degree around Z axis=%f %f %f\n", v[0], v[1], v[2]);

    matrix_init(m);
    vertex3d_set(v, 0, 1, 0);
    printf("vector before rotation=%f %f %f\n", v[0], v[1], v[2]);
    matrix_rotateZ(m, deg_to_rad(180), m);
    matrix_mul_vector(m, v, v);
    printf("vector after rotation of 180 degree around Z axis=%f %f %f\n", v[0], v[1], v[2]);

    matrix_init(m);
    vertex3d_set(v, 0, 0, 1);
    printf("vector before rotation=%f %f %f\n", v[0], v[1], v[2]);
    matrix_rotateX(m, deg_to_rad(180), m);
    matrix_mul_vector(m, v, v);
    printf("vector after rotation of 180 degree around X axis=%f %f %f\n", v[0], v[1], v[2]);

    printf("... done!\n");
    return 0;
}
