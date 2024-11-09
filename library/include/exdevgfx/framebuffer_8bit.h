/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#ifndef EXDEVGFX2_FRAMEBUFFER_8BIT_H
#define EXDEVGFX2_FRAMEBUFFER_8BIT_H

#include <exdevgfx/framebuffer.h>
#include <exdevgfx/palette.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char Color8Bit_t;

struct Framebuffer8Bit {
    int width;
    int height;
    Color8Bit_t *buffer;
};
typedef struct Framebuffer8Bit Framebuffer8Bit_t;

void framebuffer_8bit_init(Framebuffer8Bit_t *fb, int width, int height);

int framebuffer_8bit_init_from_framebuffer(Framebuffer8Bit_t *f, Palette8Bit_t *p, const Framebuffer_t *fb, int init_palette);

Color8Bit_t *framebuffer_8bit_pixel_at(const Framebuffer8Bit_t *fb, int x, int y);

void framebuffer_8bit_draw_vertical_line(Framebuffer8Bit_t *fb, int x, int y, int to_y, Color8Bit_t c);

void framebuffer_8bit_draw_horizontal_line(Framebuffer8Bit_t *fb, int x, int y, int to_x, Color8Bit_t c);

void framebuffer_8bit_draw_pixel(Framebuffer8Bit_t *fb, int x, int y, Color8Bit_t c);

void framebuffer_8bit_fill_rect(Framebuffer8Bit_t *fb, int x, int y, int width, int height, Color8Bit_t c);

void framebuffer_8bit_draw_rect(Framebuffer8Bit_t *fb, int x, int y, int width, int height, Color8Bit_t c);

int framebuffer_8bit_num_bytes(const Framebuffer8Bit_t *fb);

int framebuffer_8bit_read(Framebuffer8Bit_t *fb, const char *path);

int framebuffer_8bit_save_as_dat(const Framebuffer8Bit_t *fb, const char *path);

int framebuffer_8bit_read_from_dat(Framebuffer8Bit_t *fb, const char *path);

void framebuffer_8bit_fill(Framebuffer8Bit_t *fb, Color8Bit_t c);

void framebuffer_8bit_deinit(Framebuffer8Bit_t *fb);

void framebuffer_8bit_draw_text(Framebuffer8Bit_t *fb, const Font_t *f, const char *text, int text_length, Color8Bit_t c, int x, int y);

void framebuffer_8bit_fill_triangle(Framebuffer8Bit_t *fb, const Vertex2d_t *triangle, Color8Bit_t c);

void framebuffer_8bit_fill_triangle_fast(Framebuffer8Bit_t *fb, const Vertex2d_t *triangle, Color8Bit_t c);

//void framebuffer_8bit_fill_triangle_texture(Framebuffer8Bit_t *fb, const Vertex2d_t *triangle, const Framebuffer8Bit_t *texture);

void framebuffer_8bit_draw_framebuffer(Framebuffer8Bit_t *fb, int x, int y, const Framebuffer8Bit_t *src);

void framebuffer_8bit_draw_framebuffer_flip_vertical(Framebuffer8Bit_t *fb, int x, int y, const Framebuffer8Bit_t *src);

void framebuffer_8bit_draw_framebuffer_shifted(Framebuffer8Bit_t *fb, int x_shifted, int to_y,const Framebuffer8Bit_t *src);

void framebuffer_8bit_draw_framebuffer_scaled(Framebuffer8Bit_t *fb, int center_x, int center_y, const Framebuffer8Bit_t *src, float scale, int alpha);

void framebuffer_8bit_draw_framebuffer_rotated(Framebuffer8Bit_t *fb, int center_x, int center_y, const Framebuffer8Bit_t *src, float angle, int alpha);

void framebuffer_fill_8bit(Framebuffer_t *fb, const Framebuffer8Bit_t *src, const Palette8Bit_t *p);

#ifdef __cplusplus
}
#endif

#endif //EXDEVGFX2_FRAMEBUFFER_8BIT_H
