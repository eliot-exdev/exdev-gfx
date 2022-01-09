#include <exdevgfx/vertex2d.h>

void vertex2d_init(Vertex2d_t v) {
    v[0] = 0;
    v[1] = 0;
}

void vertex2d_set(Vertex2d_t v, float x, float y) {
    v[0] = x;
    v[1] = y;
}

void vertex2d_copy(const Vertex2d_t src, Vertex2d_t dst) {
    dst[0] = src[0];
    dst[1] = src[1];
}

float vertex2d_cross(const Vertex2d_t a, const Vertex2d_t b) {
    return a[0] * b[1] - a[1] * b[0];
}
