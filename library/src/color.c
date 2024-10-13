#include "exdevgfx/color.h"

void color_rgba_init(ColorRGBA_t *c) {
    c->r = 0;
    c->g = 0;
    c->b = 0;
    c->a = 0xff;
}

void color_rgba_set(ColorRGBA_t *c, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    c->r = r;
    c->g = g;
    c->b = b;
    c->a = a;
}

void color_rgba_copy(const ColorRGBA_t *src, ColorRGBA_t *dst) {
    *dst = *src;
}

void color_rgb_init(ColorRGB_t *c) {
    c->r = 0;
    c->g = 0;
    c->b = 0;
}

void color_rgb_set(ColorRGB_t *c, unsigned char r, unsigned char g, unsigned char b) {
    c->r = r;
    c->g = g;
    c->b = b;
}

void color_rgb_copy(const ColorRGB_t *src, ColorRGB_t *dst) {
    *dst = *src;
}

void color_rgba_rgb_copy(const ColorRGBA_t *src, ColorRGB_t *dst) {
    dst->r = src->r;
    dst->g = src->g;
    dst->b = src->b;
}

void color_rgb_rgba_copy(const ColorRGB_t *src, ColorRGBA_t *dst) {
    dst->r = src->r;
    dst->g = src->g;
    dst->b = src->b;
    dst->a = 0xff;
}

void color_rgba_rgb_combine(const ColorRGBA_t *src, ColorRGB_t *dst) {
    const float alpha = (float) src->a / 255.0f;
    dst->r = (unsigned char) ((float) src->r * alpha + (1.0f - alpha) * (float) dst->r);
    dst->g = (unsigned char) ((float) src->g * alpha + (1.0f - alpha) * (float) dst->g);
    dst->b = (unsigned char) ((float) src->b * alpha + (1.0f - alpha) * (float) dst->b);
}