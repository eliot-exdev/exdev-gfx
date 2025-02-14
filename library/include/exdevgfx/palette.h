/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#ifndef EXDEVGFX2_PALETTE_H
#define EXDEVGFX2_PALETTE_H

#include <exdevgfx/color.h>
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PEN_MAX 0xffffffff
#define PEN_HALF 0x7FFFFFFF
#define PEN_MIN 0x00000000

struct Pen {
    uint32_t r;
    uint32_t g;
    uint32_t b;
};

typedef struct Pen Pen_t;

extern const Pen_t PEN_BLACK;
extern const Pen_t PEN_WHITE;
extern const Pen_t PEN_GRAY;
extern const Pen_t PEN_RED;
extern const Pen_t PEN_GREEN;
extern const Pen_t PEN_BLUE;
extern const Pen_t PEN_YELLOW;
extern const Pen_t PEN_CYAN;

void pen_init(Pen_t *p, unsigned int r, unsigned int g, unsigned int b);

void pen_set(Pen_t *p, const ColorRGB_t *c);

void pen_to_color_rgb(const Pen_t *p, ColorRGB_t *c);

int pen_equals(const Pen_t *p1, const Pen_t *p2);

struct Palette8Bit {
    int numPens;
    Pen_t pens[255];
};

typedef struct Palette8Bit Palette8Bit_t;

void palette_8bit_init(Palette8Bit_t *p, int numPens);

void palette_8bit_set_pen(Palette8Bit_t *palette, const Pen_t *pen, unsigned char pos);

unsigned char palette_8bit_add_pen(Palette8Bit_t *palette, const Pen_t *pen);

Pen_t *palette_8bit_get_pen(Palette8Bit_t *palette, unsigned char pos);

const Pen_t *palette_8bit_get_pen_const(const Palette8Bit_t *palette, unsigned char pos);

int palette_8bit_find_pen(const Palette8Bit_t *palette, const Pen_t *pen);

int palette_8bit_save_as_dat(const Palette8Bit_t *p, const char *path);

int palette_8bit_save_as_jasc(const Palette8Bit_t *p, const char *path);

int palette_8bit_read_from_dat(Palette8Bit_t *p, const char *path);

#ifdef __cplusplus
}
#endif

#endif //EXDEVGFX2_PALETTE_H
