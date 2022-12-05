/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#ifndef EXDEVGFX2_VOXELSPACE_H
#define EXDEVGFX2_VOXELSPACE_H

#include <exdevgfx/heightmap.h>
#include <exdevgfx/vertex3d.h>

struct Sprite {
    Vertex3d_t position;
    Framebuffer8Bit_t image;
};
typedef struct Sprite Sprite_t;

void sprite_init(Sprite_t *s);

void sprite_deinit(Sprite_t *s);

struct SpriteList{
    int num;
    Sprite_t *sprites;
};
typedef struct SpriteList SpriteList_t;

void sprite_list_init(SpriteList_t *l, int num);

void sprite_list_deinit(SpriteList_t *l);

struct Voxelspace {
    Heightmap_t heightmap;
    Framebuffer8Bit_t *fb;
    int *ybuffer;
    float scale_height;
    Color8Bit_t sky_color;
    SpriteList_t sprites;
};

typedef struct Voxelspace Voxelspace_t;

void voxelspace_init(Voxelspace_t *v,
                     Framebuffer8Bit_t *height_map,
                     Framebuffer8Bit_t *color_map,
                     Framebuffer8Bit_t *fb,
                     float scale_height,
                     Color8Bit_t sky_color);

void voxelspace_deinit(Voxelspace_t *v);

void voxelspace_render(const Vertex3d_t p,
                       float phi,
                       float horizon,
                       float distance,
                       float dz,
                       int skip_x,
                       Voxelspace_t *v);

#endif //EXDEVGFX2_VOXELSPACE_H
