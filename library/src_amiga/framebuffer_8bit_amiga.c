/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#include "exdevgfx/amiga/framebuffer_8bit_amiga.h"
#include <cybergraphx/cybergraphics.h>
#include <intuition/intuition.h>
#include <intuition/screens.h>

#include <stdlib.h>
#include <assert.h>
struct NativeWindow {
    struct Window *window;
    struct Screen *screen;
};


void framebuffer_8bit_amiga_init(Framebuffer8BitAmiga_t *fb, const int width, const int height,  void * likeWindow) {
    assert(fb);
    
    struct NativeWindow *nw = (struct NativeWindow*)likeWindow;
    
    fb->width = width;
    fb->height = height;
    fb->bm = AllocBitMap(width, height, 8, 0 , nw->window->RPort->BitMap);
    InitBitMap(fb->bm,8,width,height);
    InitRastPort(&fb->rp);
    fb->rp.BitMap = fb->bm;
}

void framebuffer_8bit_amiga_deinit(Framebuffer8BitAmiga_t *fb) {
    assert(fb);
    FreeBitMap(fb->bm);
}

void framebuffer_8bit_amiga_fill(Framebuffer8BitAmiga_t *fb, Color8Bit_t c) {
    assert(fb);
    SetRast(&fb->rp, c);
}

void framebuffer_8bit_amiga_draw_vertical_line(Framebuffer8BitAmiga_t *fb, const int x, int y, int dy, const Color8Bit_t c) {
    if(x<0)
        return;
    if(x>fb->width)
        return;
    if(y<0)
        return;
    if(y>fb->height)
        return;
    if(dy<0)
        return;
    if(dy>fb->height)
        return;
    SetAPen(&fb->rp, c);
    Move(&fb->rp,x,y);
    Draw(&fb->rp,x, dy);
}

void
framebuffer_8bit_amiga_draw_text(Framebuffer8BitAmiga_t *fb, const Font_t *f, const char *text, const int text_length, const Color8Bit_t c,
                           const int x, const int y) {
    assert(fb);
    assert(f);
    assert(text);

    for (int i = 0; i < text_length; ++i) {
        const char *c_arr = font_get_character(f, text[i]);
        const int offset = i * f->width + i * f->spacing;
        for (int xx = 0; xx < f->width; ++xx) {
            for (int yy = 0; yy < f->height; ++yy) {
                if (c_arr[yy * f->width + xx]) {
                    framebuffer_8bit_amiga_draw_vertical_line(fb, x + xx + offset, y + yy, y + yy, c);
                }
            }
        }
    }
}