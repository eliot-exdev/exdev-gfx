/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#ifndef EXDEVGFX2_SW_RENDERER_H
#define EXDEVGFX2_SW_RENDERER_H

#include <exdevgfx/framebuffer.h>
#include <exdevgfx/vertex3d.h>

struct SWRenderer {
    Framebuffer_t *fb;
    float pc;
    float cx;
    float cy;
    Vertex3d_t light;
};

typedef struct SWRenderer SWRenderer_t;

void sw_renderer_init(SWRenderer_t *sw, Framebuffer_t *fb);

void sw_renderer_deinit(SWRenderer_t *sw);

void sw_renderer_fill_polygon_rgb(SWRenderer_t *sw,
                                  const Vertex3d_t *triangles,
                                  const Vertex3d_t *normals,
                                  const ColorRGB_t *colors,
                                  int triangles_count,
                                  const Vertex3d_t trans,
                                  const Vertex3d_t rot
                                  );


#endif //EXDEVGFX2_SW_RENDERER_H
