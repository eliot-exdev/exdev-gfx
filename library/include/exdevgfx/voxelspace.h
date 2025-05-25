/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#ifndef EXDEVGFX2_VOXELSPACE_H
#define EXDEVGFX2_VOXELSPACE_H

#include <exdevgfx/heightmap.h>
#include <exdevgfx/vertex3d.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float max_distance;
    int x_step_size;
} zone_t;

typedef struct {
    zone_t *zones;
    int size;
} zones_t;

struct Voxelspace {
    Heightmap_t heightmap;
    Framebuffer8Bit_t *fb;
    int *ybuffer;
    float scale_height;
    Color8Bit_t sky_color;
    Framebuffer8Bit_t *sky_texture;
    zones_t *zones;
};

typedef struct Voxelspace Voxelspace_t;

void zones_init(zones_t *zones, int size);

void zones_deinit(zones_t *zones);

void voxelspace_init(Voxelspace_t *v,
                     Framebuffer8Bit_t *height_map,
                     Framebuffer8Bit_t *color_map,
                     Framebuffer8Bit_t *fb,
                     float scale_height,
                     Color8Bit_t sky_color,
                     Framebuffer8Bit_t *sky_texture,
                     zones_t *zones);

void voxelspace_deinit(Voxelspace_t *v);

void voxelspace_render(const Vertex3d_t p, float rot, float horizon, float distance, const Voxelspace_t *v);

#ifdef __cplusplus
}
#endif

#endif //EXDEVGFX2_VOXELSPACE_H
