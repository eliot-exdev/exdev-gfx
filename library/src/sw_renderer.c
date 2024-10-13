/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#include "exdevgfx/sw_renderer.h"
#include "exdevgfx/matrix.h"
#include "exdevgfx/helper.h"

#define EXDEVGFX2_LOG_LEVEL 2

#include <exdevgfx/logger.h>

#include <assert.h>
#include <math.h>

void sw_renderer_init(SWRenderer_t *sw, Framebuffer_t *fb) {
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
    vertex3d_set(sw->light, 0, 0, 1);
}

void sw_renderer_deinit(SWRenderer_t *sw) {
    assert(sw);
    sw->fb = NULL;
}

static inline void project_triangle(SWRenderer_t *sw, const Vertex3d_t src, Vertex2d_t dst) {
    dst[0] = src[0] / src[2] * sw->pc + sw->cx;
    dst[1] = src[1] / src[2] * -sw->pc + sw->cy;
}

//static void flat_shading(const SWRenderer_t *sw, const Vertex3d_t normal, const ColorRGB_t *orig, ColorRGB_t *new) {
//    const float angle = rad_to_deg(vertex3d_angle(normal, sw->light)) / 180.0f;
//    color_rgb_set(new,
//                  orig->r * angle,
//                  orig->g * angle,
//                  orig->b * angle
//    );
////    log_debug_fmt("angle=%f %f", rad_to_deg(angle), angle);
//}

static int is_visible(const Vertex2d_t *t) {
    return (t[1][0] - t[0][0]) * (t[2][1] - t[0][1]) - (t[2][0] - t[0][0]) * (t[1][1] - t[0][1]) < 0.0f;
}

void sw_renderer_fill_polygon_rgb(SWRenderer_t *sw,
                                  const Vertex3d_t *triangles,
                                  const Vertex3d_t *normals,
                                  const ColorRGB_t *colors,
                                  const int triangles_count,
                                  const Vertex3d_t trans,
                                  const Vertex3d_t rot) {
    log_debug("--> sw_renderer_fill_triangle_rgb()");
    assert(sw);
    assert(triangles);
    assert(normals);
    assert(colors);
    assert(triangles_count > 0);

    Vertex3d_t triangle_tmp[3];
    Vertex3d_t normal_tmp;
    Vertex2d_t triangle_2d[3];
//    ColorRGB_t color_tmp;
    // matrix
    MATRIX_DEFAULT(m);
    matrix_rotate(m, rot, m);

    for (int i = 0; i < triangles_count; ++i) {
        // rotate triangle and normal
        matrix_mul_vector(m, triangles[i * 3 + 0], triangle_tmp[0]);
        matrix_mul_vector(m, triangles[i * 3 + 1], triangle_tmp[1]);
        matrix_mul_vector(m, triangles[i * 3 + 2], triangle_tmp[2]);
        matrix_mul_vector(m, normals[i], normal_tmp);

        // translate triangle
        vertex3d_add(triangle_tmp[0], trans, triangle_tmp[0]);
        vertex3d_add(triangle_tmp[1], trans, triangle_tmp[1]);
        vertex3d_add(triangle_tmp[2], trans, triangle_tmp[2]);

        // project triangle 2D -> 2d
        project_triangle(sw, triangle_tmp[0], triangle_2d[0]);
        project_triangle(sw, triangle_tmp[1], triangle_2d[1]);
        project_triangle(sw, triangle_tmp[2], triangle_2d[2]);

        if (!is_visible(triangle_2d)) {
            continue;
        }

        // flat shading
//        flat_shading(sw, normal_tmp, &colors[i], &color_tmp);

        // draw triangle
        framebuffer_fill_triangle_rgb(sw->fb, triangle_2d, &colors[i]);
    }

    log_debug("<-- sw_renderer_fill_triangle_rgb()");
}
