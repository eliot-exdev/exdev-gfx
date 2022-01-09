#include <stdio.h>
#include <exdevgfx/vertex3d.h>
#include <exdevgfx/helper.h>

/*
 * compile for amiga:
 * vc -c99 -Iinclude src/vertex3d.c src/helper.c examples/test_vector3d.c -o test_vector3d -lmsoft -lauto -lamiga -O2 -speed -final
 * vc -c99 -Iinclude src/vertex3d.c src/helper.c examples/test_vector3d.c -o test_vector3d -fpu=68060-lm060 -lauto -lamiga -O2 -speed -final
 */

Vertex3d_t v1;
Vertex3d_t v2;
Vertex3d_t v3;
Vertex3d_t v4;
Vertex3d_t v5;
Vertex3d_t v6;
Vertex3d_t v7;
float angle = 0;
float distance = 0;

int main() {
    printf("begin ...\n");

    vertex3d_set(v1, 1, 2, 3);
    vertex3d_set(v2, 1, 2, 3);
    vertex3d_div(v1, 2, v3);
    vertex3d_add(v1, v2, v4);

    printf("v1=%f %f %f\n", v1[0], v1[1], v1[2]);
    printf("v2=%f %f %f\n", v2[0], v2[1], v2[2]);
    printf("v3=%f %f %f\n", v3[0], v3[1], v3[2]);
    printf("v4=%f %f %f\n", v4[0], v4[1], v4[2]);

    vertex3d_set(v5, 1, 0, 0);
    vertex3d_set(v6, 0, 1, 0);

    angle = vertex3d_angle(v5, v6);
    distance = vertex3d_distance(v5, v6);

    printf("angle=%f\n", rad_to_deg(angle));
    printf("distance=%f\n", distance);

    vertex3d_set(v7, 5, 5, 5);
    vertex3d_normalize(v7, v7);
    printf("normalized v7=%f %f %f\n", v7[0], v7[1], v7[2]);
    printf("length v7=%f\n", vertex3d_norm(v7));

    printf("... done!\n");
    return 0;
}
