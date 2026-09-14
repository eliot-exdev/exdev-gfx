/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#ifndef EXDEVGFX2_SW_RENDERER_H
#define EXDEVGFX2_SW_RENDERER_H

#include <exdevgfx/framebuffer_8bit.h>
#include <exdevgfx/vertex3d.h>

#ifdef __cplusplus
extern "C" {
#endif

struct SWRenderer8bit {
    Framebuffer8Bit_t *fb;
    float pc;
    float cx;
    float cy;
};

typedef struct SWRenderer8bit SWRenderer8bit_t;

void sw_renderer_8bit_init(SWRenderer8bit_t *sw, Framebuffer8Bit_t *fb);

void sw_renderer_8bit_deinit(SWRenderer8bit_t *sw);

void sw_renderer_8bit_draw_triangles_color(SWRenderer8bit_t *sw,
                                         const Vertex3d_t *triangles,
                                         int triangles_count,
                                         const Color8Bit_t *colors,
                                         const Vertex3d_t trans,
                                         const Vertex3d_t rot);

void sw_renderer_8bit_fill_triangles_color(SWRenderer8bit_t *sw,
                                         const Vertex3d_t *triangles,
                                         int triangles_count,
                                         const Color8Bit_t *colors,
                                         const Vertex3d_t trans,
                                         const Vertex3d_t rot);

#ifdef __cplusplus
}
#endif

#endif //EXDEVGFX2_SW_RENDERER_H
