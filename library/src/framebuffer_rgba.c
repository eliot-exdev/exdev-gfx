/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#include "exdevgfx/framebuffer_rgba.h"

#include <stdlib.h>
#include <assert.h>

void framebuffer_rgba_init(FramebufferRGBA_t *fb, const int width, const int height) {
    fb->width = width;
    fb->height = height;
    fb->buffer = (ColorRGBA_t *) malloc(framebuffer_rgba_num_bytes(fb));
}

void framebuffer_rgba_deinit(FramebufferRGBA_t *fb) {
    free(fb->buffer);
    fb->buffer = NULL;
}

void framebuffer_rgba_fill(FramebufferRGBA_t *fb, const Framebuffer_t *src, unsigned char a) {
    assert(src);
    assert(fb);
    assert(src->width == fb->width);
    assert(src->height == fb->height);
    for (int x = 0; x < fb->width; ++x) {
        for (int y = 0; y < fb->height; ++y) {
            framebuffer_rgba_set_pixel(fb, x, y, framebuffer_pixel_at(src, x, y), a);
        }
    }
}

void framebuffer_rgba_fill_8bit(FramebufferRGBA_t *fb, const Framebuffer8Bit_t *src, const Palette8Bit_t *p, unsigned char a) {
    assert(src);
    assert(fb);
    assert(p);
    assert(src->width == fb->width);
    assert(src->height == fb->height);

    ColorRGB_t color;
    for (int x = 0; x < fb->width; ++x) {
        for (int y = 0; y < fb->height; ++y) {
            const Color8Bit_t pen_id = *framebuffer_8bit_pixel_at(src, x, y);
            const Pen_t *pen = palette_8bit_get_pen_const(p, pen_id);
            pen_to_color_rgb(pen, &color);
            framebuffer_rgba_set_pixel(fb, x, y, &color, 0xff);
        }
    }
}


int framebuffer_rgba_num_pixels(const FramebufferRGBA_t *fb) {
    return fb->width * fb->height;
}

int framebuffer_rgba_num_bytes(const FramebufferRGBA_t *fb) {
    return fb->width * fb->height * sizeof(ColorRGBA_t);
}

int framebuffer_rgba_num_bytes_per_line(const FramebufferRGBA_t *fb) {
    return (int) (fb->width * sizeof(ColorRGBA_t));
}

void framebuffer_rgba_set_pixel(FramebufferRGBA_t *fb, const int x, const int y, const ColorRGB_t *c, const unsigned char a) {
    ColorRGBA_t *pixel = framebuffer_rgba_pixel_at(fb, x, y);
    pixel->r = c->r;
    pixel->g = c->g;
    pixel->b = c->b;
    pixel->a = a;
}

void framebuffer_rgba_swap(FramebufferRGBA_t *fb) {
    for (int x = 0; x < fb->width; ++x) {
        for (int y = 0; y < fb->height; ++y) {
            ColorRGBA_t *pixel = framebuffer_rgba_pixel_at(fb, x, y);
            const unsigned char tmp = pixel->r;
            pixel->r = pixel->b;
            pixel->b = tmp;
        }
    }
}

void framebuffer_rgba_set_pixel_rgba(FramebufferRGBA_t *fb, const int x, const int y, const ColorRGBA_t *c) {
    ColorRGBA_t *pixel = framebuffer_rgba_pixel_at(fb, x, y);
    pixel->r = c->r;
    pixel->g = c->g;
    pixel->b = c->b;
    pixel->a = c->a;
}

ColorRGBA_t *framebuffer_rgba_pixel_at(const FramebufferRGBA_t *fb, const int x, const int y) {
    assert(x < fb->width);
    assert(y < fb->height);
    assert(x >= 0);
    assert(y >= 0);
    return &fb->buffer[(y * fb->width) + x];
}

void framebuffer_rgba_copy(const FramebufferRGBA_t *src, FramebufferRGBA_t *dst) {
    assert(src);
    assert(dst);
    assert(src->width == dst->width);
    assert(src->height == dst->height);

    for (int x = 0; x < src->width; ++x) {
        for (int y = 0; src->height; ++y) {
            framebuffer_rgba_set_pixel_rgba(dst, x, y, framebuffer_rgba_pixel_at(dst, x, y));
        }
    }
}
