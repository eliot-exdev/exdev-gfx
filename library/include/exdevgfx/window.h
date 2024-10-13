/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#ifndef EXDEVGFX2_WINDOW_LINUX_H
#define EXDEVGFX2_WINDOW_LINUX_H

#include <exdevgfx/framebuffer.h>
#include <exdevgfx/framebuffer_8bit.h>
#include <exdevgfx/palette.h>

typedef void *Window_t;

enum FULLSCREEN {
    FS_NO,
    FS_8_BIT,
    FS_24_BIT
};

Window_t *window_create(int width, int height, const char *title, enum FULLSCREEN fs);

void window_destroy(Window_t *w);

int window_get_width(const Window_t *w);

int window_get_height(const Window_t *w);

int window_get_inner_width(const Window_t *w);

int window_get_inner_height(const Window_t *w);

void window_fill(Window_t *w, const Framebuffer_t *gb);

void window_fill_8bit(Window_t *w, const Framebuffer8Bit_t *gb);

void window_update_palette(Window_t *w, const Palette8Bit_t *p);

enum key_event {
    KEY_EVENT_INVALID,
    KEY_EVENT_PRESSED,
    KEY_EVENT_RELEASED
};

enum key_type {
    KEY_TYPE_INVALID,
    KEY_TYPE_CODE,
    KEY_TYPE_UP,
    KEY_TYPE_DOWN,
    KEY_TYPE_LEFT,
    KEY_TYPE_RIGHT,
    KEY_TYPE_ESC,
    KEY_TYPE_F1,
    KEY_TYPE_F2,
    KEY_TYPE_F3,
    KEY_TYPE_F4,
    KEY_TYPE_F5,
    KEY_TYPE_F6
};

struct KeyEvent {
    enum key_event event;
    enum key_type type;
    unsigned char code;
};

typedef struct KeyEvent KeyEvent_t;

void key_event_init(KeyEvent_t *events, int num);

int window_poll_events(Window_t *w, char *closeEvent, KeyEvent_t *events, int num);

#endif //EXDEVGFX2_WINDOW_LINUX_H
