#include "exdevgfx/palette.h"
#include "exdevgfx/helper.h"

#define EXDEVGFX2_LOG_LEVEL 3

#include "exdevgfx/logger.h"

#include <assert.h>
#include <stdio.h>

Pen_t PEN_BLACK = {0, 0, 0};
Pen_t PEN_WHITE = {PEN_MAX, PEN_MAX, PEN_MAX};
Pen_t PEN_GRAY = {PEN_HALF, PEN_HALF, PEN_HALF};
Pen_t PEN_RED = {PEN_MAX, 0, 0};
Pen_t PEN_GREEN = {0, PEN_MAX, 0};
Pen_t PEN_BLUE = {0, 0, PEN_MAX};
Pen_t PEN_YELLOW = {PEN_MAX, PEN_MAX, 0};
Pen_t PEN_CYAN = {0, PEN_MAX, PEN_MAX};

void pen_init(Pen_t *p, const unsigned int r, const unsigned int g, const unsigned int b) {
    assert(p);

    p->r = r;
    p->g = g;
    p->b = b;
}

void pen_set(Pen_t *p, const ColorRGB_t *c) {
    assert(p);
    assert(c);

    p->r = c->r / 255.0 * PEN_MAX;
    p->g = c->g / 255.0 * PEN_MAX;
    p->b = c->b / 255.0 * PEN_MAX;
}

void pen_to_color_rgb(const Pen_t *p, ColorRGB_t *c) {
    assert(p);
    assert(c);

    c->r = p->r / (double) PEN_MAX * 255;
    c->g = p->g / (double) PEN_MAX * 255;
    c->b = p->b / (double) PEN_MAX * 255;
}

char pen_equals(const Pen_t *p1, const Pen_t *p2) {
    assert(p1);
    assert(p2);

    return p1->r == p2->r && p1->g == p2->g && p1->b == p2->b;
}


void palette_8bit_init(Palette8Bit_t *p, const int numPens) {
    assert(p);
    assert(numPens <= 256);

    p->numPens = numPens;
    for (unsigned char i = 0; i < numPens; ++i) {
        pen_init(&p->pens[i], 0, 0, 0);
    }
}

void palette_8bit_set_pen(Palette8Bit_t *palette, const Pen_t *pen, unsigned char pos) {
    assert(palette);
    assert(pen);
    assert(pos < palette->numPens);

    palette->pens[pos] = *pen;
}

unsigned char palette_8bit_add_pen(Palette8Bit_t *palette, const Pen_t *pen) {
    assert(palette);
    assert(pen);
    assert(palette->numPens < 256);

    palette->pens[palette->numPens] = *pen;
    ++palette->numPens;
    return palette->numPens - 1;
}

Pen_t *palette_8bit_get_pen(Palette8Bit_t *palette, unsigned char pos) {
    assert(palette);
    assert(pos < palette->numPens);
    return &palette->pens[pos];
}

const Pen_t *palette_8bit_get_pen_const(const Palette8Bit_t *palette, const unsigned char pos) {
    assert(palette);
    assert(pos < palette->numPens);
    return &palette->pens[pos];
}

int palette_8bit_find_pen(const Palette8Bit_t *palette, const Pen_t *pen) {
    assert(palette);
    assert(pen);
    for (unsigned char i = 0; i < palette->numPens; ++i) {
        if (pen_equals(&palette->pens[i], pen)) {
            return i;
        }
    }
    return -1;
}

int palette_8bit_save_as_dat(const Palette8Bit_t *p, const char *path) {
    assert(p);
    assert(path);

    FILE *fp;
    fp = fopen(path, "w");
    if (!fp) {
        log_warning_fmt("could not open file=%s", path);
        return 1;
    }

    int tmp = p->numPens;
    swap_bytes_int(&tmp);
    int res = fwrite(&tmp, sizeof(int), 1, fp);

    if (res != 1) {
        log_warning_fmt("could not write num pens, res=%d", res);
        fclose(fp);
        return 1;
    }

    for (int i = 0; i < p->numPens; ++i) {
        tmp = (int) p->pens[i].r;
        swap_bytes_int(&tmp);
        res = fwrite(&tmp, sizeof(int), 1, fp);
        if (res != 1) {
            log_warning_fmt("could not write r, res=%d", res);
            fclose(fp);
            return 2;
        }

        tmp = (int) p->pens[i].g;
        swap_bytes_int(&tmp);
        res = fwrite(&tmp, sizeof(int), 1, fp);
        if (res != 1) {
            log_warning_fmt("could not write g, res=%d", res);
            fclose(fp);
            return 3;
        }

        tmp = (int) p->pens[i].b;
        swap_bytes_int(&tmp);
        res = fwrite(&tmp, sizeof(int), 1, fp);
        if (res != 1) {
            log_warning_fmt("could not write b, res=%d", res);
            fclose(fp);
            return 4;
        }
    }

    fclose(fp);
    return 0;
}

int palette_8bit_read_from_dat(Palette8Bit_t *p, const char *path) {
    assert(p);
    assert(path);

    FILE *fp;

    fp = fopen(path, "r");
    if (!fp) {
        log_warning_fmt("could not open file=%s", path);
        return 1;
    }

    // num pens
    int tmp = 0;
    int res = fread(&tmp, sizeof(int), 1, fp);
    if (res != 1) {
        log_warning_fmt("could not read num pens, res=%d", res);
        fclose(fp);
        return 1;
    }

    swap_bytes_int(&tmp);
    palette_8bit_init(p, tmp);

    for (int i = 0; i < p->numPens; ++i) {
        res = fread(&tmp, sizeof(int), 1, fp);
        if (res != 1) {
            log_warning_fmt("could not read r, res=%d", res);
            fclose(fp);
            return 2;
        }
        swap_bytes_int(&tmp);
        p->pens[i].r = (unsigned int) tmp;

        res = fread(&tmp, sizeof(int), 1, fp);
        if (res != 1) {
            log_warning_fmt("could not read g, res=%d", res);
            fclose(fp);
            return 3;
        }
        swap_bytes_int(&tmp);
        p->pens[i].g = (unsigned int) tmp;

        res = fread(&tmp, sizeof(int), 1, fp);
        if (res != 1) {
            log_warning_fmt("could not read b, res=%d", res);
            fclose(fp);
            return 4;
        }
        swap_bytes_int(&tmp);
        p->pens[i].b = (unsigned int) tmp;
    }

    fclose(fp);
    return 0;
}
