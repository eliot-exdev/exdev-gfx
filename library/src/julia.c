#include "exdevgfx/julia.h"
#define EXDEVGFX2_LOG_LEVEL 1
#include "exdevgfx/logger.h"

#define DEFAULT_STEP 0.05f
#define DEFAULT_MIN_MAX 2.0f
#define DEFAULT_ABS_MAX 5.0f
#define DEFAULT_MAX_IT 150

void julia_init(Julia_t *j) {
    j->im_min = -DEFAULT_MIN_MAX;
    j->im_max = DEFAULT_MIN_MAX;
    j->re_min = -DEFAULT_MIN_MAX;
    j->re_max = DEFAULT_MIN_MAX;
    j->abs_max = DEFAULT_ABS_MAX;
    j->max_it = DEFAULT_MAX_IT;
    j->zoom = 0.0f;
    j->horizontal = 0.0f;
    j->vertical = 0.0f;
    j->step = DEFAULT_STEP;
}

void julia_move_left(Julia_t *j) {
    j->horizontal -= j->step;
}

void julia_move_right(Julia_t *j) {
    j->horizontal += j->step;
}

void julia_move_up(Julia_t *j) {
    j->vertical -= j->step;
}

void julia_move_down(Julia_t *j) {
    j->vertical += j->step;
}

void julia_move_zoom_in(Julia_t *j) {
    if (j->zoom + j->step + (j->step / DEFAULT_MIN_MAX) > DEFAULT_MIN_MAX) {
        j->step = j->step / DEFAULT_MIN_MAX;
    }
    j->zoom += j->step;
}

void julia_move_zoom_out(Julia_t *j) {
    if (j->step < DEFAULT_STEP) {
        j->step += j->step;
    } else if (j->step > DEFAULT_STEP) {
        j->step = DEFAULT_STEP;
    }
    j->zoom -= j->step;
}


static int julia(float x, float y, const float xadd, const float yadd, const float max_absolute, int iter) {
    const int max_iter = iter;
    float xx = x * x;
    float yy = y * y;
    float xy = x * y;
    float absolute = xx + yy;
    while (absolute <= max_absolute && iter > 0) {
        --iter;
        x = xx - yy + xadd;
        y = xy + xy + yadd;
        xx = x * x;
        yy = y * y;
        xy = x * y;
        absolute = xx + yy;
    }
    return max_iter - iter;
}

void julia_paint(const Julia_t *j, Framebuffer_t *fb) {
    log_debug("--> julia_paint()");
    ColorRGBA_t color = {0, 0, 0, 0xff};

    const float width = (float) fb->width;
    const float height = (float) fb->height;

    const float c_im_expr_1 = (j->im_min + j->zoom + j->vertical);
    const float c_im_expr_2 = ((j->im_max - j->zoom + j->vertical) - (j->im_min + j->zoom + j->vertical));

    const float c_re_expr_1 = (j->re_min + j->zoom + j->horizontal);
    const float c_re_expr_2 = ((j->re_max - j->zoom + j->horizontal) - (j->re_min + j->zoom + j->horizontal));

    for (int y = 0; y < fb->height; ++y) {
        const float c_im = c_im_expr_1 + c_im_expr_2 * (float) y / height;
        for (int x = 0; x < fb->width; ++x) {
            const float c_re = c_re_expr_1 + c_re_expr_2 * (float) x / width;
            const int iter = julia(c_re, c_im, c_re, c_im, j->abs_max, j->max_it);
            const char c = (char) ((float) iter / (float) j->max_it * 255.f);

            color_rgba_set(&color, 0, c, c, 0xff);
            framebuffer_draw_pixel(fb, x, y, &color);
        }
    }

    log_debug("<-- julia_paint()");
}
