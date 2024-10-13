/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#ifndef EXDEVGFX2_FB_AMIGA_H
#define EXDEVGFX2_FB_AMIGA_H

#include <exdevgfx/framebuffer.h>
#include <exdevgfx/palette.h>
#include <exdevgfx/font.h>

#include <clib/graphics_protos.h>

struct Framebuffer8BitAmiga {
    int width;
    int height;
    struct BitMap *bm;
    struct RastPort rp;
};

typedef struct Framebuffer8BitAmiga Framebuffer8BitAmiga_t;

void framebuffer_8bit_amiga_init(Framebuffer8BitAmiga_t *fb,  int width,  int height, void * likeWindow);
void framebuffer_8bit_amiga_deinit(Framebuffer8BitAmiga_t *fb);
void framebuffer_8bit_amiga_fill(Framebuffer8BitAmiga_t *fb, Color8Bit_t c);
void framebuffer_8bit_amiga_draw_vertical_line(Framebuffer8BitAmiga_t *fb, int x, int y, int dy, Color8Bit_t c);
void framebuffer_8bit_amiga_draw_text(Framebuffer8BitAmiga_t *fb, const Font_t *f, const char *text, int text_length, Color8Bit_t c,int x,  int y);

#endif