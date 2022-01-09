#include "exdevgfx/sw_renderer_8bit.h"
#include "exdevgfx/matrix.h"
#include "exdevgfx/helper.h"

#define EXDEVGFX2_LOG_LEVEL 2

#include <exdevgfx/logger.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>

void sw_renderer_8bit_init(SWRenderer8bit_t *sw, Framebuffer8Bit_t *fb) {
    assert(sw);
    assert(fb);

    sw->fb = fb;
#ifdef LOW_RESOLUTION
    sw->pc = 100.0f;
#else
    sw->pc = 200.0f;
#endif
    sw->cx = 0.5f * (float) sw->fb->width + 0.5f;
    sw->cy = 0.5f * (float) sw->fb->height + 0.5f;
}

void sw_renderer_8bit_deinit(SWRenderer8bit_t *sw) {
    assert(sw);
    sw->fb = NULL;
}

static inline void project_vertex(const SWRenderer8bit_t *sw, const Vertex3d_t src, Vertex2d_t dst) {
    dst[0] = src[0] / src[2] * sw->pc + sw->cx;
    dst[1] = src[1] / src[2] * -sw->pc + sw->cy;
}

static inline int is_visible(const Vertex2d_t *t) {
    return (t[1][0] - t[0][0]) * (t[2][1] - t[0][1]) - (t[2][0] - t[0][0]) * (t[1][1] - t[0][1]) < 0.0f;
}

static inline int is_visible_(const Vertex2d_t v0, const Vertex2d_t v1, const Vertex2d_t v2) {
    return (v1[0] - v0[0]) * (v2[1] - v0[1]) - (v2[0] - v0[0]) * (v1[1] - v0[1]) < 0.0f;
}

void sw_renderer_8bit_fill_polygon_color_new(SWRenderer8bit_t *sw,
                                             const Vertex3d_t *triangles,
                                             int triangles_count,
                                             const Color8Bit_t *colors,
                                             const Vertex3d_t trans,
                                             const Vertex3d_t rot) {
    assert(sw);
    assert(triangles);
    assert(colors);
    assert(triangles_count > 0);

    // matrix
    MATRIX_DEFAULT(m);
    matrix_rotate(m, rot, m);

    Vertex3d_t triangle_tmp[3];

    Vertex2d_t v0;
    Vertex2d_t v1;
    Vertex2d_t v2;

    for (int i = 0; i < triangles_count; ++i) {
        // rotate triangle and normal
        matrix_mul_vector(m, triangles[i * 3 + 0], triangle_tmp[0]);
        matrix_mul_vector(m, triangles[i * 3 + 1], triangle_tmp[1]);
        matrix_mul_vector(m, triangles[i * 3 + 2], triangle_tmp[2]);

        // translate triangle
        vertex3d_add(triangle_tmp[0], trans, triangle_tmp[0]);
        vertex3d_add(triangle_tmp[1], trans, triangle_tmp[1]);
        vertex3d_add(triangle_tmp[2], trans, triangle_tmp[2]);

        // project triangle onto the screen
        v0[0] = triangle_tmp[0][0] / triangle_tmp[0][2], v0[1] = triangle_tmp[0][1] / triangle_tmp[0][2];
        v1[0] = triangle_tmp[1][0] / triangle_tmp[1][2], v1[1] = triangle_tmp[1][1] / triangle_tmp[1][2];
        v2[0] = triangle_tmp[2][0] / triangle_tmp[2][2], v2[1] = triangle_tmp[2][1] / triangle_tmp[2][2];
        // convert from screen space to NDC then raster (in one go)
        v0[0] = (1 + v0[0]) * 0.5f * sw->fb->width, v0[1] = (1 + v0[1]) * 0.5f * sw->fb->height;
        v1[0] = (1 + v1[0]) * 0.5f * sw->fb->width, v1[1] = (1 + v1[1]) * 0.5f * sw->fb->height;
        v2[0] = (1 + v2[0]) * 0.5f * sw->fb->width, v2[1] = (1 + v2[1]) * 0.5f * sw->fb->height;

        if(!is_visible_(v0,v1,v2)){
            continue;
        }

        const int xmin = min(min(v0[0], v1[0]), v2[0]);
        const int xmax = max(max(v0[0], v1[0]), v2[0]);
        const int ymin = min(min(v0[1], v1[1]), v2[1]);
        const int ymax = max(max(v0[1], v1[1]), v2[1]);

        for (int y = ymin; y <= ymax; ++y) {
            for (int x = xmin; x <= xmax; ++x) {
                const Vertex2d_t p = {(float) x, (float) y};
                const float w0 = edgeFunction(v1, v2, p);
                const float w1 = edgeFunction(v2, v0, p);
                const float w2 = edgeFunction(v0, v1, p);
                if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
                    *framebuffer_8bit_pixel_at(sw->fb, x, y) = colors[i];
                }
            }
        }
    }
}

void sw_renderer_8bit_fill_polygon_color(SWRenderer8bit_t *sw,
                                         const Vertex3d_t *triangles,
                                         const int triangles_count,
                                         const Color8Bit_t *colors,
                                         const Vertex3d_t trans,
                                         const Vertex3d_t rot) {
    log_debug("--> sw_renderer_fill_triangle_rgb()");
    assert(sw);
    assert(triangles);
//    assert(normals);
    assert(colors);
    assert(triangles_count > 0);

    Vertex3d_t triangle_tmp[3];
//    Vertex3d_t normal_tmp;
    Vertex2d_t triangle_2d[3];

    // matrix
    MATRIX_DEFAULT(m);
    matrix_rotate(m, rot, m);

    for (int i = 0; i < triangles_count; ++i) {
        // rotate triangle and normal
        matrix_mul_vector(m, triangles[i * 3 + 0], triangle_tmp[0]);
        matrix_mul_vector(m, triangles[i * 3 + 1], triangle_tmp[1]);
        matrix_mul_vector(m, triangles[i * 3 + 2], triangle_tmp[2]);
//        matrix_mul_vector(m, normals[i], normal_tmp);

        // translate triangle
        vertex3d_add(triangle_tmp[0], trans, triangle_tmp[0]);
        vertex3d_add(triangle_tmp[1], trans, triangle_tmp[1]);
        vertex3d_add(triangle_tmp[2], trans, triangle_tmp[2]);

        // project triangle 2D -> 2d
        project_vertex(sw, triangle_tmp[0], triangle_2d[0]);
        project_vertex(sw, triangle_tmp[1], triangle_2d[1]);
        project_vertex(sw, triangle_tmp[2], triangle_2d[2]);

        if (!is_visible(triangle_2d)) {
            continue;
        }

        // draw triangle
        framebuffer_8bit_fill_triangle(sw->fb, triangle_2d, colors[i]);
    }
    log_debug("<-- sw_renderer_fill_triangle_rgb()");
}

static void barycentric(const Vertex3d_t a, const Vertex3d_t b, const Vertex3d_t c, const Vertex3d_t p, Vertex3d_t res) {
    const Vertex3d_t s0 = {c[0] - a[0], b[0] - a[0], a[0] - p[0]};
    const Vertex3d_t s1 = {c[1] - a[1], b[1] - a[1], a[1] - p[1]};
    Vertex3d_t u;
    vertex3d_cross(s0, s1, u);

    if (fabsf(u[2]) < 1.0f) {
        res[0] = -1;
        res[1] = 1;
        res[2] = 1;
        return;
    }

    res[0] = 1.0f - (u[0] + u[1]) / u[2];
    res[1] = u[1] / u[2];
    res[2] = u[0] / u[2];

}

void sw_renderer_8bit_fill_polygon_texture(SWRenderer8bit_t *sw,
                                           const Vertex3d_t *triangles,
                                           const Vertex2d_t *uv_coordinates,
                                           int triangles_count,
                                           const Framebuffer8Bit_t *texture,
                                           const Vertex3d_t trans,
                                           const Vertex3d_t rot) {
    assert(sw);
    assert(triangles);
    assert(texture);
    assert(triangles_count > 0);

    Vertex3d_t triangle_tmp[3];
    Vertex2d_t triangle_2d[3];

    // matrix
    MATRIX_DEFAULT(m);
    matrix_rotate(m, rot, m);

    for (int ti = 0; ti < triangles_count; ++ti) {
        // rotate triangle and normal
        matrix_mul_vector(m, triangles[ti * 3 + 0], triangle_tmp[0]);
        matrix_mul_vector(m, triangles[ti * 3 + 1], triangle_tmp[1]);
        matrix_mul_vector(m, triangles[ti * 3 + 2], triangle_tmp[2]);

        // translate triangle
        vertex3d_add(triangle_tmp[0], trans, triangle_tmp[0]);
        vertex3d_add(triangle_tmp[1], trans, triangle_tmp[1]);
        vertex3d_add(triangle_tmp[2], trans, triangle_tmp[2]);

        // project triangle 2D -> 2d
        project_vertex(sw, triangle_tmp[0], triangle_2d[0]);
        project_vertex(sw, triangle_tmp[1], triangle_2d[1]);
        project_vertex(sw, triangle_tmp[2], triangle_2d[2]);

        if (!is_visible(triangle_2d)) {
            continue;
        }

        // draw triangle
        int xmin = min(min(triangle_2d[0][0], triangle_2d[1][0]), triangle_2d[2][0]);
        int xmax = max(max(triangle_2d[0][0], triangle_2d[1][0]), triangle_2d[2][0]);
        int ymin = min(min(triangle_2d[0][1], triangle_2d[1][1]), triangle_2d[2][1]);
        int ymax = max(max(triangle_2d[0][1], triangle_2d[1][1]), triangle_2d[2][1]);

        if (xmax <= 0 || xmin >= sw->fb->width) {
            continue;
        }
        if (ymax <= 0 || ymin >= sw->fb->height) {
            continue;
        }

        if (xmin < 0) {
            xmin = 0;
        }
        if (xmax >= sw->fb->width) {
            xmax = sw->fb->width - 1;
        }
        if (ymin < 0) {
            ymin = 0;
        }
        if (ymax >= sw->fb->height) {
            ymax = sw->fb->height - 1;
        }

        float w0, w1, w2;
        Vertex2d_t p;
        for (int j = ymin; j <= ymax; ++j) {
            for (int i = xmin; i <= xmax; ++i) {
                p[0] = (float) i;
                p[1] = (float) j;
                w0 = edgeFunction(triangle_2d[1], triangle_2d[2], p);
                w1 = edgeFunction(triangle_2d[2], triangle_2d[0], p);
                w2 = edgeFunction(triangle_2d[0], triangle_2d[1], p);
                if (w0 > 0 && w1 > 0 && w2 > 0) {
                    *framebuffer_8bit_pixel_at(sw->fb, i, j) = *framebuffer_8bit_pixel_at(
                            texture,
                            ((i - xmin) / (float) (xmax - xmin)) * (texture->width - 1),
                            ((j - ymin) / (float) (ymax - ymin)) * (texture->height - 1)
                    );
                }
            }
        }


    }
}
