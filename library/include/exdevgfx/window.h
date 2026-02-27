/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#ifndef EXDEVGFX2_WINDOW_LINUX_H
#define EXDEVGFX2_WINDOW_LINUX_H

#include <exdevgfx/framebuffer.h>
#include <exdevgfx/framebuffer_8bit.h>
#include <exdevgfx/palette.h>
#include <exdevgfx/events.h>

#ifdef __cplusplus
extern "C" {



#endif

typedef void *Window_t;

enum FULLSCREEN {
    FS_8_BIT = 8,
    FS_24_BIT = 24
};

struct HW_Framebuffer {
    int width;
    int height;
    void *hw;
};

typedef struct HW_Framebuffer HW_Framebuffer_t;

Window_t *window_create(int width, int height, const char *title, enum FULLSCREEN fs);

void window_destroy(Window_t *w);

int window_get_width(const Window_t *w);

int window_get_height(const Window_t *w);

int window_get_inner_width(const Window_t *w);

int window_get_inner_height(const Window_t *w);

void window_blit_chunky_buffer(Window_t *win);

Framebuffer8Bit_t *window_get_chunky_buffer(Window_t *win);

void window_fill(Window_t *w, const Framebuffer_t *gb);

void window_fill_8bit(Window_t *w, const Framebuffer8Bit_t *gb);

void window_update_palette(Window_t *w, const Palette8Bit_t *p);

int window_poll_events(Window_t *w, char *closeEvent, Event_t *events, int maxEvents);

int window_get_mouse_position(Window_t *w, Event_t *event);

void window_init_hw_framebuffer(Window_t *w, HW_Framebuffer_t *fb);

void window_destroy_hw_framebuffer(HW_Framebuffer_t *fb);

void window_hw_framebuffer_fill_rect(HW_Framebuffer_t *fb, int x, int y, int width, int height, Color8Bit_t c);

void window_hw_framebuffer_blit(Window_t *w, HW_Framebuffer_t *fb);

#ifdef __cplusplus
}
#endif

#endif //EXDEVGFX2_WINDOW_LINUX_H
