/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#include "exdevgfx/framebuffer.h"
#include "exdevgfx/helper.h"

#define EXDEVGFX2_LOG_LEVEL 1

#include "exdevgfx/logger.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

void framebuffer_init(Framebuffer_t *fb, const int width, const int height) {
    fb->width = width;
    fb->height = height;
    fb->buffer = (ColorRGB_t *) malloc(framebuffer_num_bytes(fb));
}

void framebuffer_deinit(Framebuffer_t *fb) {
    free(fb->buffer);
}

ColorRGB_t *framebuffer_pixel_at(const Framebuffer_t *fb, const int x, const int y) {
    assert(x < fb->width);
    assert(y < fb->height);
    assert(x >= 0);
    assert(y >= 0);
    return &fb->buffer[(y * fb->width) + x];
}

int sgn(const int x) {
    return (x > 0) ? 1 : (x < 0) ? -1 : 0;
}

void framebuffer_draw_line(Framebuffer_t *fb, const Vertex2d_t a, const Vertex2d_t b, const ColorRGBA_t *c) {
    int x, y, t, dx, dy, incx, incy, pdx, pdy, ddx, ddy, deltaslowdirection, deltafastdirection, err;

    dx = (int) (b[0] - a[0]);
    dy = (int) (b[1] - a[1]);

    incx = sgn(dx);
    incy = sgn(dy);
    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;

    if (dx > dy) {
        pdx = incx;
        pdy = 0;
        ddx = incx;
        ddy = incy;
        deltaslowdirection = dy;
        deltafastdirection = dx;
    } else {
        pdx = 0;
        pdy = incy;
        ddx = incx;
        ddy = incy;
        deltaslowdirection = dx;
        deltafastdirection = dy;
    }

    x = (int) a[0];
    y = (int) a[1];
    err = deltafastdirection / 2;
    framebuffer_draw_pixel(fb, x, y, c);

    for (t = 0; t < deltafastdirection; ++t) {
        err -= deltaslowdirection;
        if (err < 0) {
            err += deltafastdirection;
            x += ddx;
            y += ddy;
        } else {
            x += pdx;
            y += pdy;
        }
        framebuffer_draw_pixel(fb, x, y, c);
    }
}

void framebuffer_draw_vertical_line(Framebuffer_t *fb, int x, const int y, int dy, const ColorRGBA_t *c) {
    assert(fb);

    if (x >= fb->width) {
        return;
    }
    if (x < 0) {
        return;
    }

    if (y >= fb->height) {
        return;
    }

    if (dy <= 0) {
        return;
    }

    if (y + dy >= fb->height) {
        dy = fb->height - y;
    }

    for (int i = 0; i < dy; ++i) {
        color_rgba_rgb_combine(c, framebuffer_pixel_at(fb, x + i, y));
    }
}

void framebuffer_draw_horizontal_line(Framebuffer_t *fb, int x, int y, int dx, const ColorRGB_t *c) {
    assert(fb);

    if (x >= fb->width) {
        return;
    }

    if (y < 0) {
        return;
    }
    if (y >= fb->height) {
        return;
    }

    if (dx <= 0) {
        return;
    }
    if (x < 0) {
        dx = dx + x;
        x = 0;
    }
    if (x + dx > fb->width) {
        dx = fb->width - x;
    }

    for (int i = 0; i < dx; ++i) {
        *framebuffer_pixel_at(fb, x + i, y) = *c;
    }
}

void framebuffer_draw_framebuffer(Framebuffer_t *fb, const int x, const int y, const Framebuffer_t *src) {
    assert(fb);
    assert(src);
    if (x > fb->width) {
        return;
    }
    if (y > fb->height) {
        return;
    }

    for (int i = 0; i + x < fb->width && i < src->width; ++i) {
        for (int j = 0; j + y < fb->height && j < src->height; ++j) {
            fb->buffer[((j + y) * fb->width) + i + x] = src->buffer[j * src->width + i];
        }
    }
}


void framebuffer_draw_vertical_line_rgb(Framebuffer_t *fb, int x, const int y, int dx, const ColorRGB_t *c) {
    assert(fb);

    if (x >= fb->width) {
        return;
    }
    if (y < 0) {
        return;
    }
    if (y >= fb->height) {
        return;
    }
    if (dx <= 0) {
        return;
    }
    if (x < 0) {
        x = 0;
    }
    if (x + dx > fb->width) {
        dx = fb->width - x;
    }
    for (int i = 0; i < dx; ++i) {
        *framebuffer_pixel_at(fb, x + i, y) = *c;
    }
}

void framebuffer_draw_triangle(Framebuffer_t *fb, Vertex2d_t *triangle, const ColorRGBA_t *c) {
    framebuffer_draw_line(fb, triangle[0], triangle[1], c);
    framebuffer_draw_line(fb, triangle[1], triangle[2], c);
    framebuffer_draw_line(fb, triangle[2], triangle[0], c);
}

void framebuffer_fill(Framebuffer_t *fb, const ColorRGBA_t *c) {
    const int num_pixels = framebuffer_num_pixels(fb);
    ColorRGB_t *c_ptr = fb->buffer;
    int i = 0;
    struct ColorRGB c_rgb;
    color_rgba_rgb_copy(c, &c_rgb);
    for (; i < num_pixels; ++i) {
        *c_ptr = c_rgb;
        ++c_ptr;
    }
}

void framebuffer_fill_rgb(Framebuffer_t *fb, const ColorRGB_t *c) {
    const int num_pixels = framebuffer_num_pixels(fb);
    ColorRGB_t *c_ptr = fb->buffer;
    int i = 0;
    for (; i < num_pixels; ++i) {
        *c_ptr = *c;
        ++c_ptr;
    }
}

void framebuffer_fill_rect(Framebuffer_t *fb,
                           const int x,
                           const int y,
                           const int width,
                           const int height,
                           const ColorRGBA_t *c) {
    int x_;
    int y_;

    if (x + width > fb->width)
        return;
    if (y + height > fb->height)
        return;

    for (x_ = x; x_ < width + x; ++x_) {
        for (y_ = y; y_ < height + y; ++y_) {
            color_rgba_rgb_combine(c, framebuffer_pixel_at(fb, x_, y_));
        }
    }
}

void framebuffer_copy(Framebuffer_t *dst, const Framebuffer_t *src) {
    assert(dst->height == src->height);
    assert(dst->width == src->width);
    memcpy(dst->buffer, src->buffer, framebuffer_num_bytes(dst));
}


void framebuffer_fill_rect_rgb(Framebuffer_t *fb, int x, int y, int width, int height, const ColorRGB_t *c) {
    int x_;
    int y_;

    if (x + width > fb->width)
        return;
    if (y + height > fb->height)
        return;

    for (x_ = x; x_ < width + x; ++x_) {
        for (y_ = y; y_ < height + y; ++y_) {
            color_rgb_copy(c, framebuffer_pixel_at(fb, x_, y_));
        }
    }
}

struct FloatColorRGBA {
    float r;
    float g;
    float b;
    float a;
};
typedef struct FloatColorRGBA FloatColorRGBA_t;

static inline void color_rgba_to_float_color_rgba(const ColorRGBA_t *src, FloatColorRGBA_t *dst) {
    dst->r = (float) src->r / 255.f;
    dst->g = (float) src->g / 255.f;
    dst->b = (float) src->b / 255.f;
    dst->a = (float) src->a / 255.f;
}

static inline void float_color_rgba_to_color_rgba(const FloatColorRGBA_t *src, ColorRGBA_t *dst) {
    dst->r = (unsigned char) (src->r * 255);
    dst->g = (unsigned char) (src->g * 255);
    dst->b = (unsigned char) (src->b * 255);
    dst->a = (unsigned char) (src->a * 255);
}

void framebuffer_fill_triangle(Framebuffer_t *fb, Vertex2d_t *triangle, const ColorRGBA_t *colors) {
    int j, i;
    Vertex2d_t p;
    float w0, w1, w2;
    ColorRGBA_t color;
    int xmin, xmax, ymin, ymax;
    FloatColorRGBA_t floatColor;
    FloatColorRGBA_t floatColors[3];
    const float area = edgeFunction(triangle[0], triangle[1], triangle[2]);

    color_rgba_to_float_color_rgba(&colors[0], &floatColors[0]);
    color_rgba_to_float_color_rgba(&colors[1], &floatColors[1]);
    color_rgba_to_float_color_rgba(&colors[2], &floatColors[2]);

    xmin = min(min(triangle[0][0], triangle[1][0]), triangle[2][0]);
    xmax = max(max(triangle[0][0], triangle[1][0]), triangle[2][0]);
    ymin = min(min(triangle[0][1], triangle[1][1]), triangle[2][1]);
    ymax = max(max(triangle[0][1], triangle[1][1]), triangle[2][1]);

    for (j = ymin; j < ymax; ++j) {
        for (i = xmin; i < xmax; ++i) {
            vertex2d_set(p, i + 0.5f, j + 0.5f);
            w0 = edgeFunction(triangle[1], triangle[2], p);
            w1 = edgeFunction(triangle[2], triangle[0], p);
            w2 = edgeFunction(triangle[0], triangle[1], p);
            if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
                w0 /= area;
                w1 /= area;
                w2 /= area;
                floatColor.r = w0 * floatColors[0].r + w1 * floatColors[1].r + w2 * floatColors[2].r;
                floatColor.g = w0 * floatColors[0].g + w1 * floatColors[1].g + w2 * floatColors[2].g;
                floatColor.b = w0 * floatColors[0].b + w1 * floatColors[1].b + w2 * floatColors[2].b;
                floatColor.a = w0 * floatColors[0].a + w1 * floatColors[1].a + w2 * floatColors[2].a;
                float_color_rgba_to_color_rgba(&floatColor, &color);
                framebuffer_draw_pixel(fb, i, j, &color);
            }
        }
    }
}

void framebuffer_fill_triangle_rgb(Framebuffer_t *fb, const Vertex2d_t *triangle, const ColorRGB_t *color) {
    int xmin = min(min(triangle[0][0], triangle[1][0]), triangle[2][0]);
    int xmax = max(max(triangle[0][0], triangle[1][0]), triangle[2][0]);
    int ymin = min(min(triangle[0][1], triangle[1][1]), triangle[2][1]);
    int ymax = max(max(triangle[0][1], triangle[1][1]), triangle[2][1]);

    if (xmax <= 0 || xmin >= fb->width) {
        return;
    }
    if (ymax <= 0 || ymin >= fb->height) {
        return;
    }

    if (xmin < 0) {
        xmin = 0;
    }
    if (xmax > fb->width) {
        xmax = fb->width;
    }
    if (ymin < 0) {
        ymin = 0;
    }
    if (ymax > fb->height) {
        ymax = fb->height;
    }

    float w0, w1, w2;
    for (int j = ymin; j < ymax; ++j) {
        for (int i = xmin; i < xmax; ++i) {
            const Vertex2d_t p = {(float) i, (float) j};
            w0 = edgeFunction(triangle[1], triangle[2], p);
            w1 = edgeFunction(triangle[2], triangle[0], p);
            w2 = edgeFunction(triangle[0], triangle[1], p);
            if (w0 > 0 && w1 > 0 && w2 > 0) {
                *framebuffer_pixel_at(fb, i, j) = *color;
            }
        }
    }
}

// https://www-users.mat.uni.torun.pl//~wrona/3d_tutor/tri_fillers.html
static inline void vertex2d_swap(const float **a, const float **b) {
    const float *tmp = *a;
    *a = *b;
    *b = tmp;
}

#define Ax v1[0]
#define Ay v1[1]
#define Bx v2[0]
#define By v2[1]
#define Cx v3[0]
#define Cy v3[1]

void framebuffer_fill_triangle_fast(Framebuffer_t *fb, const Vertex2d_t *triangle, const ColorRGB_t *color) {
    const float *v1 = triangle[0];
    const float *v2 = triangle[1];
    const float *v3 = triangle[2];

    if (v1[1] > v2[1])
        vertex2d_swap(&v1, &v2);
    if (v2[1] > v3[1])
        vertex2d_swap(&v2, &v3);
    if (v1[1] > v2[1])
        vertex2d_swap(&v1, &v2);

    float dx1 = 0, dx2 = 0, dx3 = 0;
    if (By - Ay > 0)
        dx1 = (Bx - Ax) / (By - Ay);
    if (Cy - Ay > 0)
        dx2 = (Cx - Ax) / (Cy - Ay);
    if (Cy - By > 0)
        dx3 = (Cx - Bx) / (Cy - By);

    float Sx = Ax, Sy = Ay, Ex = Ax, Ey = By;
    if (Ax <= Bx) {
        while (Sy < Ey) {
            Sx += dx2;
            Ex += dx1;
            framebuffer_draw_horizontal_line(fb, (int) Sx, (int) Sy, (int) (Ex - Sx), color);
            ++Sy;
        }
    } else {
        while (Sy < Ey) {
            Sx += dx1;
            Ex += dx2;
            framebuffer_draw_horizontal_line(fb, (int) Sx, (int) Sy, (int) (Ex - Sx), color);
            ++Sy;
        }
    }

    Sy = By;
    Ey = Cy;
    if (Bx <= Cx) {
        Sx = Bx;
        while (Sy < Ey) {
            Sx += dx3;
            Ex += dx2;
            framebuffer_draw_horizontal_line(fb, (int) Sx, (int) Sy, (int) (Ex - Sx), color);
            ++Sy;
        }
    } else {
        Ex = Bx;
        while (Sy < Ey) {
            Sx += dx2;
            Ex += dx3;
            framebuffer_draw_horizontal_line(fb, (int) Sx, (int) Sy, (int) (Ex - Sx), color);
            ++Sy;
        }
    }
}

void framebuffer_draw_pixel(Framebuffer_t *fb, const int x, const int y, const ColorRGBA_t *c) {
    if (x >= fb->width) {
        return;
    }
    if (y >= fb->height) {
        return;
    }
    if (x < 0) {
        return;
    }
    if (y < 0) {
        return;
    }
    color_rgba_rgb_combine(c, framebuffer_pixel_at(fb, x, y));
}

void framebuffer_draw_pixel_rgb(Framebuffer_t *fb, const int x, const int y, const ColorRGB_t *c) {
    if (x >= fb->width) {
        return;
    }
    if (y >= fb->height) {
        return;
    }
    if (x < 0) {
        return;
    }
    if (y < 0) {
        return;
    }
    *framebuffer_pixel_at(fb, x, y) = *c;
}


int framebuffer_num_pixels(const Framebuffer_t *fb) {
    return fb->width * fb->height;
}

int framebuffer_num_bytes(const Framebuffer_t *fb) {
    return fb->width * fb->height * sizeof(ColorRGB_t);
}

int framebuffer_num_bytes_per_line(const Framebuffer_t *fb) {
    return fb->width * sizeof(ColorRGB_t);
}

int framebuffer_save(const Framebuffer_t *fb, const char *path) {
    assert(fb);
    assert(path);

    FILE *fp;
    char s[128];

    fp = fopen(path, "w");
    if (!fp) {
        log_warning_fmt("could not open file=%s", path);
        return 1;
    }

    memset(s, 0, 128);
    sprintf(s, "P6\n%i %i\n255\n", fb->width, fb->height);
    fwrite(s, strlen(s), 1, fp);
    fwrite((const char *) fb->buffer, framebuffer_num_bytes(fb), 1, fp);
    fclose(fp);
    return 0;
}

#define LINE_LENGTH 256

int framebuffer_read(Framebuffer_t *fb, const char *path) {
    assert(fb);
    assert(path);

    FILE *fp;

    fp = fopen(path, "r");
    if (!fp) {
        log_warning_fmt("could not open file=%s", path);
        return 1;
    }

    char line[LINE_LENGTH];

    // check format
    size_t line_length = read_line(fp, line, LINE_LENGTH);
    if (line_length != 3 || strcmp(line, "P3\n") != 0) {
        log_warning_fmt("wrong file format=%s", line);
        return 1;
    }

    // skip comments
    do {
        line_length = read_line(fp, line, LINE_LENGTH);
    } while (line_length > 0 && line[0] == '#');

    // width height
    int width = 0, height = 0;
    sscanf(line, "%d %d", &width, &height);
    log_info_fmt("width=%d, height=%d", width, height);

    // max
    line_length = read_line(fp, line, LINE_LENGTH);
    if (line_length <= 0) {
        log_warning("could not read max value");
        return 2;
    } else {
        log_info_fmt("read max value=%s", line);
    }

    framebuffer_init(fb, width, height);

    ColorRGB_t pixel;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {

            int tmp = 0;
            // parse red
            line_length = read_line(fp, line, LINE_LENGTH);
            if (line_length <= 0) {
                log_warning("could not read red value");
                return 3;
            }
            sscanf(line, "%i", &tmp);
            pixel.r = (unsigned char) tmp;

            // parse green
            line_length = read_line(fp, line, LINE_LENGTH);
            if (line_length <= 0) {
                log_warning("could not read green value");
                return 3;
            }
            sscanf(line, "%i", &tmp);
            pixel.g = (unsigned char) tmp;

            // parse green
            line_length = read_line(fp, line, LINE_LENGTH);
            if (line_length <= 0) {
                log_warning("could not read blue value");
                return 3;
            }
            sscanf(line, "%i", &tmp);
            pixel.b = (unsigned char) tmp;

            // draw pixel
            framebuffer_draw_pixel_rgb(fb, x, y, &pixel);
        }
    }

    fclose(fp);
    return 0;
}

int framebuffer_save_as_dat(const Framebuffer_t *fb, const char *path) {
    assert(fb);
    assert(path);

    FILE *fp;
    fp = fopen(path, "w");
    if (!fp) {
        log_warning_fmt("could not open file=%s", path);
        return 1;
    }

    int tmp = fb->width;
    swap_bytes_int(&tmp);
    fwrite(&tmp, sizeof(int), 1, fp);

    tmp = fb->height;
    swap_bytes_int(&tmp);
    fwrite(&tmp, sizeof(int), 1, fp);

    const size_t res = fwrite((const char *) fb->buffer, framebuffer_num_bytes(fb), 1, fp);
    if (res != 1) {
        log_warning_fmt("could not write all bytes, res=%lu", res);
        fclose(fp);
        return 1;
    }

    fclose(fp);
    return 0;
}

int framebuffer_read_from_dat(Framebuffer_t *fb, const char *path) {
    assert(fb);
    assert(path);

    FILE *fp;

    fp = fopen(path, "r");
    if (!fp) {
        log_warning_fmt("could not open file=%s", path);
        return 1;
    }

    // width height
    int width = 0, height = 0;
    size_t res = fread(&width, sizeof(int), 1, fp);
    if (res != 1) {
        log_warning_fmt("could not read all bytes, res=%lu", res);
        fclose(fp);
        return 1;
    }
    res = fread(&height, sizeof(int), 1, fp);
    if (res != 1) {
        log_warning_fmt("could not read all bytes, res=%lu", res);
        fclose(fp);
        return 1;
    }
    swap_bytes_int(&width);
    swap_bytes_int(&height);
    log_info_fmt("width=%d, height=%d", width, height);

    framebuffer_init(fb, width, height);
    res = fread(fb->buffer, framebuffer_num_bytes(fb), 1, fp);
    if (res != 1) {
        log_warning_fmt("could not read all bytes, res=%lu", res);
        fclose(fp);
        return 1;
    }

    fclose(fp);
    return 0;
}

void
framebuffer_draw_text(Framebuffer_t *fb, const Font_t *f, const char *text, const int text_length, const ColorRGBA_t *c,
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
                    framebuffer_draw_pixel(fb, x + xx + offset, y + yy, c);
                }
            }
        }
    }
}

void framebuffer_draw_text_rgb(Framebuffer_t *fb, const Font_t *f, const char *text, unsigned int text_length, const ColorRGB_t *c, int x,
                               int y) {
    assert(fb);
    assert(f);
    assert(text);

    for (unsigned int i = 0; i < text_length; ++i) {
        const char *c_arr = font_get_character(f, text[i]);
        const int offset = i * f->width + i * f->spacing;
        for (int xx = 0; xx < f->width; ++xx) {
            for (int yy = 0; yy < f->height; ++yy) {
                if (c_arr[yy * f->width + xx]) {
                    framebuffer_draw_pixel_rgb(fb, x + xx + offset, y + yy, c);
                }
            }
        }
    }
}
