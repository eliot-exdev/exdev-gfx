/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#ifndef EXDEVGFX2_JULIA_H
#define EXDEVGFX2_JULIA_H

#include <exdevgfx/framebuffer.h>

struct Julia {
    float im_min;
    float im_max;
    float re_min;
    float re_max;
    float abs_max;
    int max_it;
    float zoom;
    float horizontal;
    float vertical;
    float step;
};

typedef struct Julia Julia_t;

void julia_init(Julia_t *j);

void julia_move_left(Julia_t *j);

void julia_move_right(Julia_t *j);

void julia_move_up(Julia_t *j);

void julia_move_down(Julia_t *j);

void julia_move_zoom_in(Julia_t *j);

void julia_move_zoom_out(Julia_t *j);

void julia_paint(const Julia_t *j, Framebuffer_t *fb);

#endif //EXDEVGFX2_JULIA_H
