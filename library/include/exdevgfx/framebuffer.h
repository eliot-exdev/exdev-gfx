/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#ifndef EXDEVGFX2_FRAMEBUFFER_H
#define EXDEVGFX2_FRAMEBUFFER_H

#include <exdevgfx/vertex2d.h>
#include <exdevgfx/font.h>

#define edgeFunction(a, b, c) ((c[0] - a[0]) * (b[1] - a[1]) - (c[1] - a[1]) * (b[0] - a[0]))

typedef unsigned char Color8Bit_t;

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

unsigned char color_rgb_to_8bit(const ColorRGB_t *c);

void color_rgba_rgb_combine(const ColorRGBA_t *src, ColorRGB_t *dst);

struct Framebuffer {
    int width;
    int height;
    ColorRGB_t *buffer;
};
typedef struct Framebuffer Framebuffer_t;

void framebuffer_init(Framebuffer_t *fb, int width, int height);

void framebuffer_deinit(Framebuffer_t *fb);

ColorRGB_t *framebuffer_pixel_at(const Framebuffer_t *fb, int x, int y);

void framebuffer_draw_line(Framebuffer_t *fb, const Vertex2d_t a, const Vertex2d_t b, const ColorRGBA_t *c);

void framebuffer_draw_vertical_line_rgb(Framebuffer_t *fb, int x, int y, int dy, const ColorRGB_t *c);

void framebuffer_draw_vertical_line(Framebuffer_t *fb, int x, int y, int dy, const ColorRGBA_t *c);

void framebuffer_draw_horizontal_line(Framebuffer_t *fb, int x, int y, int dx, const ColorRGB_t *c);

void framebuffer_draw_framebuffer(Framebuffer_t *fb, int x, int y, const Framebuffer_t *src);

void framebuffer_draw_triangle(Framebuffer_t *fb, Vertex2d_t *triangle, const ColorRGBA_t *c);

void framebuffer_fill(Framebuffer_t *fb, const ColorRGBA_t *c);

void framebuffer_fill_rgb(Framebuffer_t *fb, const ColorRGB_t *c);

void framebuffer_fill_rect(Framebuffer_t *fb, int x, int y, int width, int height, const ColorRGBA_t *c);

void framebuffer_copy(Framebuffer_t *dst, const Framebuffer_t *src);

void framebuffer_fill_rect_rgb(Framebuffer_t *fb, int x, int y, int width, int height, const ColorRGB_t *c);

void framebuffer_fill_triangle(Framebuffer_t *fb, Vertex2d_t *triangle, const ColorRGBA_t *colors);

void framebuffer_fill_triangle_rgb(Framebuffer_t *fb, const Vertex2d_t *triangle, const ColorRGB_t *color);

void framebuffer_fill_triangle_fast(Framebuffer_t *fb, const Vertex2d_t *triangle, const ColorRGB_t *color);

void framebuffer_draw_pixel(Framebuffer_t *fb, int x, int y, const ColorRGBA_t *c);

void framebuffer_draw_pixel_rgb(Framebuffer_t *fb, int x, int y, const ColorRGB_t *c);

int framebuffer_num_pixels(const Framebuffer_t *fb);

int framebuffer_num_bytes(const Framebuffer_t *fb);

int framebuffer_num_bytes_per_line(const Framebuffer_t *fb);

int framebuffer_save(const Framebuffer_t *fb, const char *path);

int framebuffer_read(Framebuffer_t *fb, const char *path);

int framebuffer_save_as_dat(const Framebuffer_t *fb, const char *path);

int framebuffer_read_from_dat(Framebuffer_t *fb, const char *path);

void framebuffer_draw_text(Framebuffer_t *fb, const Font_t *f, const char *text, int text_length, const ColorRGBA_t *c, int x, int y);

void framebuffer_draw_text_rgb(Framebuffer_t *fb, const Font_t *f, const char *text, unsigned int text_length, const ColorRGB_t *c, int x,
                               int y);

#endif
