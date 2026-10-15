/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#include "exdevgfx/sw_renderer_8bit.h"
#include "exdevgfx/matrix.h"
#include "exdevgfx/helper.h"

#define EXDEVGFX2_LOG_LEVEL 2

#include <exdevgfx/logger.h>
#include <assert.h>

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

static void project_vertex(const SWRenderer8bit_t *sw, const Vertex3d_t src, Vertex2d_t dst) {
    dst[0] = src[0] / src[2] * sw->pc + sw->cx;
    dst[1] = src[1] / src[2] * -sw->pc + sw->cy;
}

static int is_visible(const Vertex2d_t *t) {
    return (t[1][0] - t[0][0]) * (t[2][1] - t[0][1]) - (t[2][0] - t[0][0]) * (t[1][1] - t[0][1]) < 0.0f;
}

// static int is_visible_(const Vertex2d_t v0, const Vertex2d_t v1, const Vertex2d_t v2) {
//     return (v1[0] - v0[0]) * (v2[1] - v0[1]) - (v2[0] - v0[0]) * (v1[1] - v0[1]) < 0.0f;
// }

// void sw_renderer_8bit_fill_polygon_color_new(SWRenderer8bit_t *sw,
//                                              const Vertex3d_t *triangles,
//                                              int triangles_count,
//                                              const Color8Bit_t *colors,
//                                              const Vertex3d_t trans,
//                                              const Vertex3d_t rot) {
//     assert(sw);
//     assert(triangles);
//     assert(colors);
//     assert(triangles_count > 0);
//
//     // matrix
//     MATRIX_DEFAULT(m);
//     matrix_rotate(m, rot, m);
//
//     Vertex3d_t triangle_tmp[3];
//
//     Vertex2d_t v0;
//     Vertex2d_t v1;
//     Vertex2d_t v2;
//
//     int xmin, xmax;
//     int ymin, ymax;
//     log_info("draw");
//     for (int i = 0; i < triangles_count; ++i) {
//         // rotate triangle and normal
//         matrix_mul_vector(m, triangles[i * 3 + 0], triangle_tmp[0]);
//         matrix_mul_vector(m, triangles[i * 3 + 1], triangle_tmp[1]);
//         matrix_mul_vector(m, triangles[i * 3 + 2], triangle_tmp[2]);
//
//         // translate triangle
//         vertex3d_add(triangle_tmp[0], trans, triangle_tmp[0]);
//         vertex3d_add(triangle_tmp[1], trans, triangle_tmp[1]);
//         vertex3d_add(triangle_tmp[2], trans, triangle_tmp[2]);
//
//         // project triangle onto the screen
//         v0[0] = triangle_tmp[0][0] / triangle_tmp[0][2];
//         v0[1] = triangle_tmp[0][1] / triangle_tmp[0][2];
//
//         v1[0] = triangle_tmp[1][0] / triangle_tmp[1][2];
//         v1[1] = triangle_tmp[1][1] / triangle_tmp[1][2];
//
//         v2[0] = triangle_tmp[2][0] / triangle_tmp[2][2];
//         v2[1] = triangle_tmp[2][1] / triangle_tmp[2][2];
//
//         // convert from screen space to NDC then raster (in one go)
//         v0[0] = (1 + v0[0]) * 0.5f * sw->fb->width;
//         v0[1] = (1 + v0[1]) * 0.5f * sw->fb->height;
//
//         v1[0] = (1 + v1[0]) * 0.5f * sw->fb->width;
//         v1[1] = (1 + v1[1]) * 0.5f * sw->fb->height;
//
//         v2[0] = (1 + v2[0]) * 0.5f * sw->fb->width;
//         v2[1] = (1 + v2[1]) * 0.5f * sw->fb->height;
//
//         if (!is_visible_(v0, v1, v2)) {
//             log_info("not visible");
//             continue;
//         }
//
//         xmin = max(min(min(v0[0], v1[0]), v2[0]), 0);
//         xmax = min(max(max(v0[0], v1[0]), v2[0]), sw->fb->width - 1);
//         ymin = max(min(min(v0[1], v1[1]), v2[1]), 0);
//         ymax = min(max(max(v0[1], v1[1]), v2[1]), sw->fb->height - 1);
//
//         for (int y = ymin; y <= ymax; ++y) {
//             for (int x = xmin; x <= xmax; ++x) {
//                 const Vertex2d_t p = {(float) x, (float) y};
//                 const float w0 = edgeFunction(v1, v2, p);
//                 const float w1 = edgeFunction(v2, v0, p);
//                 const float w2 = edgeFunction(v0, v1, p);
//                 if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
//                     *framebuffer_8bit_pixel_at(sw->fb, x, y) = colors[i];
//                 }
//             }
//         }
//     }
// }

void sw_renderer_8bit_draw_triangles(SWRenderer8bit_t *sw,
                                     const Vertex3d_t *triangles,
                                     const int triangles_count,
                                     const Color8Bit_t *colors,
                                     const Matrix_t matrix) {
    assert(sw);
    assert(triangles);
    assert(colors);
    assert(triangles_count > 0);

    Vertex3d_t triangle_tmp[3];
    Vertex2d_t triangle_2d[3];

    for (int i = 0; i < triangles_count; ++i) {
        // rotate and translate
        matrix_mul_vector(matrix, triangles[i * 3 + 0], triangle_tmp[0]);
        matrix_mul_vector(matrix, triangles[i * 3 + 1], triangle_tmp[1]);
        matrix_mul_vector(matrix, triangles[i * 3 + 2], triangle_tmp[2]);

        // project triangle 3D -> 2D
        project_vertex(sw, triangle_tmp[0], triangle_2d[0]);
        project_vertex(sw, triangle_tmp[1], triangle_2d[1]);
        project_vertex(sw, triangle_tmp[2], triangle_2d[2]);

        if (!is_visible(triangle_2d)) {
            continue;
        }

        // draw triangle
        framebuffer_8bit_draw_line(sw->fb, triangle_2d[0], triangle_2d[1], colors[i]);
        framebuffer_8bit_draw_line(sw->fb, triangle_2d[1], triangle_2d[2], colors[i]);
        framebuffer_8bit_draw_line(sw->fb, triangle_2d[2], triangle_2d[0], colors[i]);
    }
}


void sw_renderer_8bit_fill_triangles(SWRenderer8bit_t *sw,
                                     const Vertex3d_t *triangles,
                                     const int triangles_count,
                                     const Color8Bit_t *colors,
                                     const Matrix_t matrix) {
    assert(sw);
    assert(triangles);
    assert(colors);
    assert(triangles_count > 0);

    Vertex3d_t triangle_tmp[3];
    Vertex2d_t triangle_2d[3];

    for (int i = 0; i < triangles_count; ++i) {
        // rotate and translate
        matrix_mul_vector(matrix, triangles[i * 3 + 0], triangle_tmp[0]);
        matrix_mul_vector(matrix, triangles[i * 3 + 1], triangle_tmp[1]);
        matrix_mul_vector(matrix, triangles[i * 3 + 2], triangle_tmp[2]);

        // project triangle 3D -> 2D
        project_vertex(sw, triangle_tmp[0], triangle_2d[0]);
        project_vertex(sw, triangle_tmp[1], triangle_2d[1]);
        project_vertex(sw, triangle_tmp[2], triangle_2d[2]);

        if (!is_visible(triangle_2d)) {
            continue;
        }

        // fill triangle
        framebuffer_8bit_fill_triangle(sw->fb, triangle_2d, colors[i]);
    }
}

void sw_renderer_8bit_fill_triangles_texture(SWRenderer8bit_t *sw,
                                             const Vertex3d_t *triangles,
                                             int triangles_count,
                                             const Framebuffer8Bit_t *texture,
                                             const TextureCoordinates_t *texture_coords,
                                             const Matrix_t matrix) {
    assert(sw);
    assert(triangles);
    assert(texture);
    assert(texture_coords);
    assert(triangles_count > 0);

    Vertex3d_t triangle_tmp[3];
    Vertex2d_t triangle_2d[3];

    const int tex_width = texture->width;
    const Color8Bit_t *tex = texture->buffer;

    for (int i = 0; i < triangles_count; ++i) {
        // rotate and translate
        matrix_mul_vector(matrix, triangles[i * 3 + 0], triangle_tmp[0]);
        matrix_mul_vector(matrix, triangles[i * 3 + 1], triangle_tmp[1]);
        matrix_mul_vector(matrix, triangles[i * 3 + 2], triangle_tmp[2]);

        // project triangle 3D -> 2D
        project_vertex(sw, triangle_tmp[0], triangle_2d[0]);
        project_vertex(sw, triangle_tmp[1], triangle_2d[1]);
        project_vertex(sw, triangle_tmp[2], triangle_2d[2]);

        if (!is_visible(triangle_2d)) {
            continue;
        }

        int xmin = min(min(triangle_2d[0][0], triangle_2d[1][0]), triangle_2d[2][0]);
        int xmax = max(max(triangle_2d[0][0], triangle_2d[1][0]), triangle_2d[2][0]);
        int ymin = min(min(triangle_2d[0][1], triangle_2d[1][1]), triangle_2d[2][1]);
        int ymax = max(max(triangle_2d[0][1], triangle_2d[1][1]), triangle_2d[2][1]);

        // outside of framebuffer
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

        const float area = edgeFunction(triangle_2d[0], triangle_2d[1], triangle_2d[2]);
        if (area <= 0.0f) {
            continue;
        }
        const TextureCoordinates_t *uv = &texture_coords[i * 3];

        // edge functions are linear in x and y -> step them incrementally
        const float e0dx = triangle_2d[2][1] - triangle_2d[1][1];
        const float e0dy = triangle_2d[1][0] - triangle_2d[2][0];
        const float e1dx = triangle_2d[0][1] - triangle_2d[2][1];
        const float e1dy = triangle_2d[2][0] - triangle_2d[0][0];
        const float e2dx = triangle_2d[1][1] - triangle_2d[0][1];
        const float e2dy = triangle_2d[0][0] - triangle_2d[1][0];

        // texture coordinates (in texels) are linear in x and y as well
        const float inv_area = 1.0f / area;
        const float su = (float) (texture->width - 1) * inv_area;
        const float sv = (float) (texture->height - 1) * inv_area;
        const float u0 = uv[0].u * su, u1 = uv[1].u * su, u2 = uv[2].u * su;
        const float v0 = uv[0].v * sv, v1 = uv[1].v * sv, v2 = uv[2].v * sv;
        const float dudx = e0dx * u0 + e1dx * u1 + e2dx * u2;
        const float dudy = e0dy * u0 + e1dy * u1 + e2dy * u2;
        const float dvdx = e0dx * v0 + e1dx * v1 + e2dx * v2;
        const float dvdy = e0dy * v0 + e1dy * v1 + e2dy * v2;

        // values at the first pixel center
        const Vertex2d_t p = {(float) xmin + 0.5f, (float) ymin + 0.5f};
        float w0_row = edgeFunction(triangle_2d[1], triangle_2d[2], p);
        float w1_row = edgeFunction(triangle_2d[2], triangle_2d[0], p);
        float w2_row = edgeFunction(triangle_2d[0], triangle_2d[1], p);
        float u_row = w0_row * u0 + w1_row * u1 + w2_row * u2;
        float v_row = w0_row * v0 + w1_row * v1 + w2_row * v2;

        Color8Bit_t *dst_row = sw->fb->buffer + ymin * sw->fb->width + xmin;

        for (int y = ymin; y <= ymax; ++y) {
            float w0 = w0_row, w1 = w1_row, w2 = w2_row;
            float u = u_row, v = v_row;
            Color8Bit_t *dst = dst_row;
            char inside = 0;
            for (int x = xmin; x <= xmax; ++x) {
                if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
                    *dst = tex[(int) v * tex_width + (int) u];
                    inside = 1;
                } else if (inside) {
                    // triangle is convex: no more pixels in this row
                    break;
                }
                w0 += e0dx;
                w1 += e1dx;
                w2 += e2dx;
                u += dudx;
                v += dvdx;
                ++dst;
            }

            w0_row += e0dy;
            w1_row += e1dy;
            w2_row += e2dy;
            u_row += dudy;
            v_row += dvdy;
            dst_row += sw->fb->width;
        }
    }
}
