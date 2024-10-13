/**
 * Copyright 2023 Andre Geisler (andre@exdev.de)
 */

#ifndef EXDEVGFX_BASE_COLOR_H
#define EXDEVGFX_BASE_COLOR_H

struct ColorRGBA {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};
typedef struct ColorRGBA ColorRGBA_t;

void color_rgba_init(ColorRGBA_t *c);

void color_rgba_set(ColorRGBA_t *c, unsigned char r, unsigned char g, unsigned char b, unsigned char a);

void color_rgba_copy(const ColorRGBA_t *src, ColorRGBA_t *dst);

struct ColorRGB {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

typedef struct ColorRGB ColorRGB_t;

void color_rgb_init(ColorRGB_t *c);

void color_rgb_set(ColorRGB_t *c, unsigned char r, unsigned char g, unsigned char b);

void color_rgb_copy(const ColorRGB_t *src, ColorRGB_t *dst);

void color_rgba_rgb_copy(const ColorRGBA_t *src, ColorRGB_t *dst);

void color_rgb_rgba_copy(const ColorRGB_t *src, ColorRGBA_t *dst);

void color_rgba_rgb_combine(const ColorRGBA_t *src, ColorRGB_t *dst);

#endif //EXDEVGFX_BASE_COLOR_H
