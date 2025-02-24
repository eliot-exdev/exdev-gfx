/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#include <exdevgfx/voxelspace.h>
#include <exdevgfx/helper.h>

#define EXDEVGFX2_LOG_LEVEL 2

#include <exdevgfx/logger.h>

#include <stdlib.h>
#include <assert.h>

#ifndef EXDEV_FP_MATH
#include <math.h>
#endif

#ifndef __VBCC__

#include <wchar.h>

#endif
// see: https://github.com/s-macke/VoxelSpace

#ifdef LOW_RESOLUTION
#ifdef EXDEV_FP_MATH
#define AUTO_HEIGHT_OVER_GROUND exdev_int_to_fp(5)
#else
#define AUTO_HEIGHT_OVER_GROUND 5.f
#endif
#define SKY_TEXTURE_HEIGHT 100
#else
#ifdef EXDEV_FP_MATH
#define AUTO_HEIGHT_OVER_GROUND exdev_int_to_fp(10)
#else
#define AUTO_HEIGHT_OVER_GROUND 10.f
#endif
#define SKY_TEXTURE_HEIGHT 200
#endif

void voxelspace_init(Voxelspace_t *v,
                     Framebuffer8Bit_t *height_map,
                     Framebuffer8Bit_t *color_map,
                     Framebuffer8Bit_t *fb,
                     EXDEV_FLOAT scale_height,
                     Color8Bit_t sky_color,
                     Framebuffer8Bit_t *sky_texture) {
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
    v->sky_texture = sky_texture;
}

void voxelspace_deinit(Voxelspace_t *v) {
    assert(v);

    heightmap_deinit(&v->heightmap);
    v->fb = NULL;
    free(v->ybuffer);
    v->ybuffer = NULL;
    v->sky_texture = NULL;
}

void
voxelspace_render(const Vertex3d_t p, const EXDEV_FLOAT rot, const EXDEV_FLOAT horizon, const EXDEV_FLOAT distance, const EXDEV_FLOAT dz, const int skip_x, const Voxelspace_t *v) {
    // precalculate viewing angle parameters
    const EXDEV_FLOAT phi = deg_to_rad(rot);
#ifdef EXDEV_FP_MATH
    const EXDEV_FLOAT sinphi = exdev_fp_sin(phi);
    const EXDEV_FLOAT cosphi = exdev_fp_cos(phi);
    EXDEV_FLOAT z = EXDEV_FP_ONE;
#else
    const EXDEV_FLOAT sinphi = sin(phi);
    const EXDEV_FLOAT cosphi = cos(phi);
    EXDEV_FLOAT z = 1.0f;
#endif
    Vertex2d_t pleft;
    int pleft_n[2];
    Vertex2d_t pright;

    EXDEV_FLOAT dx, dy;
    int i = 0, si = 0;

    // init ybuffer
#ifdef __VBCC__
    for (i = 0; i < v->fb->width; ++i) { v->ybuffer[i] = v->fb->height; }
#else
    wmemset((wchar_t *) v->ybuffer, v->fb->height, v->fb->width);
#endif
    // render sky
    if (!v->sky_texture) {
        framebuffer_8bit_fill(v->fb, v->sky_color);
    } else {
        int x_shifted = 0;
        if (rot < 0) {
#ifdef EXDEV_FP_MATH
            x_shifted = exdev_fp_to_int(exdev_fp_mul(exdev_fp_div(-rot, exdev_int_to_fp(360)), exdev_int_to_fp(v->sky_texture->width)));
#else
            x_shifted = (int) ((float) -rot / 360.0f * (float) v->sky_texture->width);
#endif
        } else {
#ifdef EXDEV_FP_MATH
            x_shifted = exdev_fp_to_int(exdev_fp_mul(exdev_fp_div(rot, exdev_int_to_fp(360)), exdev_int_to_fp(v->sky_texture->width)));
#else
            x_shifted = (int) ((float) rot / 360.0f * (float) v->sky_texture->width);
#endif
        }
        if (rot > 0) {
            x_shifted = v->sky_texture->width - x_shifted;
        }
        framebuffer_8bit_draw_framebuffer_shifted(v->fb, x_shifted, SKY_TEXTURE_HEIGHT, v->sky_texture);
    }

    // auto height
    EXDEV_FLOAT height = p[2];
    if ((int) height < 0) {
#ifdef EXDEV_FP_MATH
        height = exdev_int_to_fp(heightmap_value_at(&v->heightmap, exdev_fp_to_int(p[0]), exdev_int_to_fp(p[1]))->height) + AUTO_HEIGHT_OVER_GROUND;
#else
        height = (float) heightmap_value_at(&v->heightmap, (int) p[0], (int) p[1])->height + AUTO_HEIGHT_OVER_GROUND;
#endif
    }
    // log_info_fmt("height=%f", height);

    log_debug("--> starting render round");
    int height_on_screen = 0;
    while (z < distance) {
        log_debug_fmt("z=%f", z);
        // Find line on map. This calculation corresponds to a field of view of 90°
#ifdef EXDEV_FP_MATH
        pleft[0] = exdev_fp_mul(-cosphi, z) - exdev_fp_mul(sinphi, z) + p[0];
        pleft[1] = exdev_fp_mul(sinphi, z) - exdev_fp_mul(cosphi, z) + p[1];

        pright[0] = exdev_fp_mul(cosphi, z) - exdev_fp_mul(sinphi, z) + p[0];
        pright[1] = exdev_fp_mul(-sinphi, z) - exdev_fp_mul(cosphi, z) + p[1];

        // segment the line
        dx = exdev_fp_div((pright[0] - pleft[0]), exdev_int_to_fp(v->fb->width));
        dy = exdev_fp_div((pright[1] - pleft[1]), exdev_int_to_fp(v->fb->width));
#else
        pleft[0] = (-cosphi * z - sinphi * z) + p[0];
        pleft[1] = (sinphi * z - cosphi * z) + p[1];

        pright[0] = (cosphi * z - sinphi * z) + p[0];
        pright[1] = (-sinphi * z - cosphi * z) + p[1];

        // segment the line
        dx = (pright[0] - pleft[0]) / (float) v->fb->width;
        dy = (pright[1] - pleft[1]) / (float) v->fb->width;
#endif
        // Raster line and draw a vertical line for each segment
        if (skip_x) {
            si = !si;
            i = si;
        } else {
            i = 0;
        }
        while (i < v->fb->width) {
            // calc height on screen
//            log_info_fmt("%d %d", pleft_n[0], pleft_n[1]);
#ifdef EXDEV_FP_MATH
            pleft_n[0] = normalize_int(exdev_fp_to_int(pleft[0]), v->heightmap.width);
            pleft_n[1] = normalize_int(exdev_fp_to_int(pleft[1]), v->heightmap.height);
#else
            pleft_n[0] = normalize_int((int) pleft[0], v->heightmap.width);
            pleft_n[1] = normalize_int((int) pleft[1], v->heightmap.height);
#endif
            const HeightmapValue_t *value = heightmap_value_at_const(&v->heightmap, pleft_n[0], pleft_n[1]);
#ifdef EXDEV_FP_MATH
            height_on_screen = exdev_fp_to_int(exdev_fp_mul(exdev_fp_div(height - exdev_int_to_fp((int) value->height), z), v->scale_height) + horizon);
#else
            height_on_screen = (int) ((height - (float) value->height) / z * v->scale_height + horizon);
#endif
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
#ifdef EXDEV_FP_MATH
                pleft[0] += exdev_fp_mul(dx, exdev_int_to_fp(2));
                pleft[1] += exdev_fp_mul(dy, exdev_int_to_fp(2));
#else
                pleft[0] += dx * 2.0f;
                pleft[1] += dy * 2.0f;
#endif
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
