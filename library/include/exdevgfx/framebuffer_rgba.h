/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#ifndef EXDEVGFX2_FRAMEBUFFER_RGBA_H
#define EXDEVGFX2_FRAMEBUFFER_RGBA_H

#include <exdevgfx/framebuffer_8bit.h>

struct FramebufferRGBA {
    int width;
    int height;
    ColorRGBA_t *buffer;
};

typedef struct FramebufferRGBA FramebufferRGBA_t;

void framebuffer_rgba_init(FramebufferRGBA_t *fb, int width, int height);

void framebuffer_rgba_deinit(FramebufferRGBA_t *fb);

void framebuffer_rgba_fill(FramebufferRGBA_t *fb, const Framebuffer_t *src, unsigned char a);

void framebuffer_rgba_fill_8bit(FramebufferRGBA_t *fb, const Framebuffer8Bit_t *src, const Palette8Bit_t *p);

void framebuffer_rgba_copy(const FramebufferRGBA_t *src, FramebufferRGBA_t *dst);

int framebuffer_rgba_num_pixels(const FramebufferRGBA_t *fb);

int framebuffer_rgba_num_bytes(const FramebufferRGBA_t *fb);

int framebuffer_rgba_num_bytes_per_line(const FramebufferRGBA_t *fb);

void framebuffer_rgba_set_pixel(FramebufferRGBA_t *fb, int x, int y, const ColorRGB_t *c, unsigned char a);

void framebuffer_rgba_swap(FramebufferRGBA_t *fb);

void framebuffer_rgba_set_pixel_rgba(FramebufferRGBA_t *fb, int x, int y, const ColorRGBA_t *c);

ColorRGBA_t *framebuffer_rgba_pixel_at(const FramebufferRGBA_t *fb, int x, int y);

#endif //EXDEVGFX2_FRAMEBUFFER_RGBA_H
