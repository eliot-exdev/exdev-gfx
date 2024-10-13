/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#ifndef EXDEVGFX2_HEIGHTMAP_H
#define EXDEVGFX2_HEIGHTMAP_H

#include <exdevgfx/framebuffer_8bit.h>

#ifdef __cplusplus
extern "C" {
#endif

struct HeightmapValue {
    Color8Bit_t height;
    Color8Bit_t color;
};

typedef struct HeightmapValue HeightmapValue_t;

struct Heightmap {
    int width;
    int height;
    HeightmapValue_t *values;
};

typedef struct Heightmap Heightmap_t;

void heightmap_init(Heightmap_t *hm, int width, int height);

void heightmap_deinit(Heightmap_t *hm);

void heightmap_init_with_framebuffer(Heightmap_t *hm, const Framebuffer8Bit_t *heights, const Framebuffer8Bit_t *colors);

HeightmapValue_t *heightmap_value_at(const Heightmap_t *hm, int x, int y);

const HeightmapValue_t *heightmap_value_at_const(const Heightmap_t *hm, int x, int y);

#ifdef __cplusplus
}
#endif

#endif //EXDEVGFX2_HEIGHTMAP_H
