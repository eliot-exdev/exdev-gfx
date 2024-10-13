#include <exdevgfx/voxelspace.h>
#include <exdevgfx/vertex2d.h>
#include <exdevgfx/helper.h>

#define EXDEVGFX2_LOG_LEVEL 2

#include <exdevgfx/logger.h>

#include <stdlib.h>
#include <assert.h>
#include <math.h>
// see: https://github.com/s-macke/VoxelSpace


#ifdef LOW_RESOLUTION
#define AUTO_HEIGHT_OVER_GROUND 5.f
#else
#define AUTO_HEIGHT_OVER_GROUND 10.f
#endif

void voxelspace_init(Voxelspace_t *v,
                     Framebuffer8Bit_t *height_map,
                     Framebuffer8Bit_t *color_map,
                     Framebuffer8Bit_t *fb,
                     float scale_height,
                     Color8Bit_t sky_color) {
    assert(v);
    assert(height_map);
    assert(color_map);
    assert(fb);
    assert(scale_height > 0);

    assert(height_map->height == color_map->height);
    assert(height_map->width == color_map->width);

    heightmap_init_with_framebuffer(&v->heightmap, height_map, color_map);
    v->fb = fb;
    v->scale_height = scale_height;
    v->sky_color = sky_color;
    v->ybuffer = malloc(sizeof(int) * fb->width);
}

void voxelspace_deinit(Voxelspace_t *v) {
    assert(v);

    heightmap_deinit(&v->heightmap);
    framebuffer_8bit_deinit(v->fb);
    v->fb = NULL;
    free(v->ybuffer);
    v->ybuffer = NULL;
}

void voxelspace_render(const Vertex3d_t p,
                       const float phi,
                       const float horizon,
                       const float distance,
                       const float dz,
                       const int skip_x,
                       Voxelspace_t *v) {
    // precalculate viewing angle parameters
    const float sinphi = sin(phi);
    const float cosphi = cos(phi);

    // Draw from back to the front (high z coordinate to low z coordinate)
    float z = 1.0f;
//    float dz = 1.0f;

    Vertex2d_t pleft;
    int pleft_n[2];
    Vertex2d_t pright;

    float dx, dy;
    int i = 0, si = 0;

    // init ybuffer
    for (i = 0; i < v->fb->width; ++i) {
        v->ybuffer[i] = v->fb->height;
    }

    // render sky
    framebuffer_8bit_fill(v->fb, v->sky_color);

    // auto height
    float height = p[2];
    if ((int) height < 0) {
        height = (float) heightmap_value_at(&v->heightmap, (int) p[0], (int) p[1])->height + AUTO_HEIGHT_OVER_GROUND;
    }
    // log_info_fmt("height=%f", height);

    log_debug("--> starting render round");
    while (z < distance) {
        log_debug_fmt("z=%f", z);
        // Find line on map. This calculation corresponds to a field of view of 90°
        pleft[0] = (-cosphi * z - sinphi * z) + p[0];
        pleft[1] = (sinphi * z - cosphi * z) + p[1];

        pright[0] = (cosphi * z - sinphi * z) + p[0];
        pright[1] = (-sinphi * z - cosphi * z) + p[1];

        // segment the line
        dx = (pright[0] - pleft[0]) / (float) v->fb->width;
        dy = (pright[1] - pleft[1]) / (float) v->fb->width;

        // Raster line and draw a vertical line for each segment
        if (skip_x) {
            si = !si;
            i = si;
        } else {
            i = 0;
        }
        while (i < v->fb->width) {
            // calc height on screen
            pleft_n[0] = normalize_int((int) pleft[0], v->heightmap.width);
            pleft_n[1] = normalize_int((int) pleft[1], v->heightmap.height);
            const HeightmapValue_t *value = heightmap_value_at_const(&v->heightmap, pleft_n[0], pleft_n[1]);
            int height_on_screen = (int) ((height - (float) value->height) / z * v->scale_height + horizon);
            if (height_on_screen < 0) {
                height_on_screen = 0;
            }

            // render
            if (height_on_screen < v->ybuffer[i]) {
                framebuffer_8bit_draw_vertical_line(v->fb,
                                                    i,
                                                    height_on_screen,
                                                    v->ybuffer[i],
                                                    value->color);
                v->ybuffer[i] = height_on_screen;
            }

            // next step
            if (skip_x) {
                pleft[0] += dx * 2;
                pleft[1] += dy * 2;
                i += 2;
            } else {
                pleft[0] += dx;
                pleft[1] += dy;
                ++i;
            }
        }

        z += dz;
    }
    log_debug("--> render round done!");
}
