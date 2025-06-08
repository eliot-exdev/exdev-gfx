/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#include <exdevgfx/voxelspace.h>
#include <exdevgfx/helper.h>

#define EXDEVGFX2_LOG_LEVEL 2

#include <exdevgfx/logger.h>

#include <stdlib.h>
#include <assert.h>
#include <math.h>

#ifndef __VBCC__

#include <wchar.h>

#endif
// see: https://github.com/s-macke/VoxelSpace

#ifdef LOW_RESOLUTION
#define AUTO_HEIGHT_OVER_GROUND 10.f
#define SKY_TEXTURE_HEIGHT 208
#else
#define AUTO_HEIGHT_OVER_GROUND 20.f
#define SKY_TEXTURE_HEIGHT 300
#endif

void zones_init(zones_t *zones, const int size) {
    assert(zones);
    assert(size > 0);

    zones->zones = malloc(sizeof(zone_t) * size);
    zones->size = size;
}

void zones_deinit(zones_t *zones) {
    assert(zones);

    free(zones->zones);
    zones->zones = NULL;
    zones->size = 0;
}

void voxelspace_init(Voxelspace_t *v,
                     Framebuffer8Bit_t *height_map,
                     Framebuffer8Bit_t *color_map,
                     Framebuffer8Bit_t *fb,
                     float scale_height,
                     Color8Bit_t sky_color,
                     Framebuffer8Bit_t *sky_texture,
                     zones_t *zones) {
    assert(v);
    assert(height_map);
    assert(color_map);
    assert(fb);
    assert(zones);
    assert(scale_height > 0);

    assert(height_map->height == color_map->height);
    assert(height_map->width == color_map->width);

    heightmap_init_with_framebuffer(&v->heightmap, height_map, color_map);
    v->fb = fb;
    v->scale_height = scale_height;
    v->sky_color = sky_color;
    v->ybuffer = ALLOC_FAST_MEM(sizeof(int) * fb->width);
    v->sky_texture = sky_texture;
    v->zones = zones;
}

void voxelspace_deinit(Voxelspace_t *v) {
    assert(v);

    heightmap_deinit(&v->heightmap);
    FREE_MEM(v->ybuffer, sizeof(sizeof(int) * v->fb->width));
    v->ybuffer = NULL;
    v->fb = NULL;
    v->sky_texture = NULL;
    v->zones = NULL;
}

void voxelspace_render(const Vertex3d_t p,
                       const float rot,
                       const float horizon,
                       const float distance,
                       const Voxelspace_t *v) {
    // precalculate viewing angle parameters
    const float phi = deg_to_rad(rot);
    const float sinphi = sin(phi);
    const float cosphi = cos(phi);

    float z = 4.0f;
    Vertex2d_t pleft;
    Vertex2d_t pright;
    int pleft_n[2];

    float dx, dy;
    int i = 0, si = 0;

    // init ybuffer
#ifdef __VBCC__
    for (; i < v->fb->width; ++i) { v->ybuffer[i] = v->fb->height; }
#else
    wmemset((wchar_t *) v->ybuffer, v->fb->height, v->fb->width);
#endif
    // render sky
    int x_shifted = 0;
    if (rot < 0) {
        x_shifted = (int) ((float) -rot / 360.0f * (float) v->sky_texture->width);
    } else {
        x_shifted = v->sky_texture->width - (int) ((float) rot / 360.0f * (float) v->sky_texture->width);
    }

    framebuffer_8bit_draw_framebuffer_shifted(v->fb, x_shifted, SKY_TEXTURE_HEIGHT, v->sky_texture);

    // auto height
    float height = p[2];
    if ((int) height < 0) {
        height = (float) heightmap_value_at_const(&v->heightmap, (int) p[0], (int) p[1])->height + AUTO_HEIGHT_OVER_GROUND;
    }
    // log_info_fmt("height=%f", height);

    log_debug("--> starting render round");
    const HeightmapValue_t *value = NULL;
    int height_on_screen = 0;
    int *ybuffer = v->ybuffer;
    const Heightmap_t *heightmap = &v->heightmap;

    zone_t *current_zone = v->zones->zones;

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

        // find current zone
        i = 1;
        while (i < v->zones->size) {
            if (z < v->zones->zones[i].max_distance) {
                current_zone = v->zones->zones + i;
                break;
            }
            ++i;
        }

        //        log_info_fmt("using zone: %f %d, %f", current_zone->dz, current_zone->x_step_size, current_zone->max_distance);
        i = 0;
        while (i < v->fb->width) {
            // calc height on screen
            // log_info_fmt("%d %d", pleft_n[0], pleft_n[1]);
            pleft_n[0] = normalize_int((int) pleft[0], heightmap->width);
            pleft_n[1] = normalize_int((int) pleft[1], heightmap->height);
            value = heightmap_value_at_const_inline(heightmap, pleft_n[0], pleft_n[1]);
            height_on_screen = (int) ((height - (float) value->height) / z * v->scale_height + horizon);
            if (height_on_screen < 0) {
                height_on_screen = 0;
            }

            // render
            if (height_on_screen < ybuffer[i]) {
                framebuffer_8bit_fill_rect(v->fb, i, height_on_screen, current_zone->x_step_size, ybuffer[i] - height_on_screen, value->color);
                for (si = 0; si < current_zone->x_step_size; ++si) {
                    ybuffer[i + si] = height_on_screen;
                }
            }

            // next step
            pleft[0] += dx * (float) current_zone->x_step_size;
            pleft[1] += dy * (float) current_zone->x_step_size;
            i = i + current_zone->x_step_size;
        }
        z += current_zone->dz;
    }
    log_debug("--> render round done!");
}
