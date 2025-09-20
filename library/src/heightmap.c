/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#include "exdevgfx/heightmap.h"
#include <assert.h>
#include <stdlib.h>

void heightmap_init(Heightmap_t *hm, const int width, const int height) {
    assert(hm);
    assert(width > 0);
    assert(height > 0);

    hm->width = width;
    hm->height = height;
    hm->values = malloc(sizeof(HeightmapValue_t) * width * height);
}

void heightmap_deinit(Heightmap_t *hm) {
    assert(hm);

    free(hm->values);
    hm->values = NULL;
}

void heightmap_init_with_framebuffer(Heightmap_t *hm, const Framebuffer8Bit_t *heights, const Framebuffer8Bit_t *colors) {
    assert(hm);
    assert(colors);
    assert(colors->buffer);
    assert(heights);
    assert(heights->buffer);
    assert(colors->width == heights->width);
    assert(colors->height == heights->height);

    heightmap_init(hm, heights->width, heights->height);

    for (int x = 0; x < hm->width; ++x) {
        for (int y = 0; y < hm->height; ++y) {
            HeightmapValue_t *value = heightmap_value_at(hm, x, y);
            value->height = *framebuffer_8bit_pixel_at(heights, x, y);
            value->color = *framebuffer_8bit_pixel_at(colors, x, y);
        }
    }
}

HeightmapValue_t *heightmap_value_at(Heightmap_t *hm, const int x, const int y) {
    assert(hm);
    assert(x < hm->width);
    assert(y < hm->height);
    assert(x >= 0);
    assert(y >= 0);
    assert(hm->values);

    return hm->values + ((y * hm->width) + x);
}

const HeightmapValue_t *heightmap_value_at_const(const Heightmap_t *hm, const int x, const int y) {
    assert(x < hm->width);
    assert(y < hm->height);
    assert(x >= 0);
    assert(y >= 0);

    return hm->values + ((y * hm->width) + x);
}
