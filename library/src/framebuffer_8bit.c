/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#include "exdevgfx/framebuffer_8bit.h"
#include "exdevgfx/helper.h"

#define EXDEVGFX2_LOG_LEVEL 3

#include "exdevgfx/logger.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>

void framebuffer_8bit_init(Framebuffer8Bit_t *fb, const int width, const int height) {
    assert(fb);

    fb->width = width;
    fb->height = height;
    fb->buffer = (Color8Bit_t *) malloc(width * height);
}

int framebuffer_8bit_init_from_framebuffer(Framebuffer8Bit_t *f, Palette8Bit_t *p, const Framebuffer_t *fb) {
    assert(f);
    assert(p);
    assert(fb);

    palette_8bit_init(p, 0);
    framebuffer_8bit_init(f, fb->width, fb->height);

    for (int x = 0; x < f->width; ++x) {
        for (int y = 0; y < f->height; ++y) {
            const ColorRGB_t *color = framebuffer_pixel_at(fb, x, y);
            Pen_t pen;
            pen_set(&pen, color);
            int pos = palette_8bit_find_pen(p, &pen);
            if (pos >= 0) {
                *framebuffer_8bit_pixel_at(f, x, y) = pos;
            } else if (p->numPens < 256) {
                *framebuffer_8bit_pixel_at(f, x, y) = palette_8bit_add_pen(p, &pen);
            } else {
                log_warning("we are out of pens");
                return 1;
            }
        }
    }

    return 0;
}

void framebuffer_8bit_deinit(Framebuffer8Bit_t *fb) {
    assert(fb);

    free(fb->buffer);
    fb->buffer = NULL;
}

void framebuffer_8bit_fill(Framebuffer8Bit_t *fb, Color8Bit_t c) {
    assert(fb);

    memset(fb->buffer, c, framebuffer_8bit_num_bytes(fb));
}

Color8Bit_t *framebuffer_8bit_pixel_at(const Framebuffer8Bit_t *fb, const int x, const int y) {
    assert(x < fb->width);
    assert(y < fb->height);
    assert(x >= 0);
    assert(y >= 0);

    return fb->buffer + ((y * fb->width) + x);
}

void framebuffer_8bit_draw_vertical_line(Framebuffer8Bit_t *fb, const int x, int y, const int dy, const Color8Bit_t c) {
    assert(fb);

    for (; y < dy; ++y) {
        fb->buffer[y * fb->width + x] = c;
    }
}

void framebuffer_8bit_draw_horizontal_line(Framebuffer8Bit_t *fb, int x, const int y, int dx, const Color8Bit_t c) {
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
    memset(framebuffer_8bit_pixel_at(fb, x, y), c, dx);
}

void framebuffer_8bit_draw_pixel(Framebuffer8Bit_t *fb, int x, int y, Color8Bit_t c) {
    assert(fb);

    if (x >= fb->width || x < 0) {
        return;
    }
    if (y >= fb->height || y < 0) {
        return;
    }
    *framebuffer_8bit_pixel_at(fb, x, y) = c;
}

void framebuffer_8bit_fill_rect(Framebuffer8Bit_t *fb, const int x, const int y, const int width, const int height, const Color8Bit_t c) {
    assert(fb);

    for (int i = 0; i < height; ++i) {
        framebuffer_8bit_draw_horizontal_line(fb, x, i + y, width, c);
    }
}

void framebuffer_8bit_draw_rect(Framebuffer8Bit_t *fb, const int x, const int y, const int width, const int height, const Color8Bit_t c) {
    assert(fb);

    framebuffer_8bit_draw_vertical_line(fb, x, y, width, c);
    framebuffer_8bit_draw_vertical_line(fb, x + height, y, width, c);
    framebuffer_8bit_draw_horizontal_line(fb, x, y, height, c);
    framebuffer_8bit_draw_horizontal_line(fb, x + width, y, height, c);
}

int framebuffer_8bit_num_bytes(const Framebuffer8Bit_t *fb) {
    assert(fb);

    return fb->width * fb->height;
}

#define LINE_LENGTH 256

int framebuffer_8bit_read(Framebuffer8Bit_t *fb, const char *path) {
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
    if (line_length != 3 || strcmp(line, "P2\n") != 0) {
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

    framebuffer_8bit_init(fb, width, height);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {

            int tmp = 0;
            // parse gray value
            line_length = read_line(fp, line, LINE_LENGTH);
            if (line_length <= 0) {
                log_warning("could not read gray value");
                return 3;
            }
            sscanf(line, "%i", &tmp);

            // draw pixel
            fb->buffer[y * width + x] = (unsigned char) tmp;
        }
    }

    fclose(fp);
    return 0;

}

int framebuffer_8bit_save_as_dat(const Framebuffer8Bit_t *fb, const char *path) {
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

    const size_t res = fwrite((const char *) fb->buffer, framebuffer_8bit_num_bytes(fb), 1, fp);
    if (res != 1) {
        log_warning_fmt("could not write all bytes, res=%lu", res);
        fclose(fp);
        return 1;
    }

    fclose(fp);
    return 0;
}

int framebuffer_8bit_read_from_dat(Framebuffer8Bit_t *fb, const char *path) {
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
    if (res == 0) {
        log_warning_fmt("could not read all bytes, res=%lu", res);
        fclose(fp);
        return 1;
    }
    res = fread(&height, sizeof(int), 1, fp);
    if (res == 0) {
        log_warning_fmt("could not read all bytes, res=%lu", res);
        fclose(fp);
        return 1;
    }
    swap_bytes_int(&width);
    swap_bytes_int(&height);
    log_info_fmt("width=%d, height=%d", width, height);

    framebuffer_8bit_init(fb, width, height);

    res = fread(fb->buffer, framebuffer_8bit_num_bytes(fb), 1, fp);
    if (res != 1) {
        log_warning_fmt("could not read all bytes, res=%lu", res);
        fclose(fp);
        return 1;
    }

    fclose(fp);
    return 0;
}

void
framebuffer_8bit_draw_text(Framebuffer8Bit_t *fb, const Font_t *f, const char *text, const int text_length, const Color8Bit_t c, const int x, const int y) {
    assert(fb);
    assert(f);
    assert(text);

    for (int i = 0; i < text_length; ++i) {
        const char *c_arr = font_get_character(f, text[i]);
        const int offset = i * f->width + i * f->spacing;
        for (int xx = 0; xx < f->width; ++xx) {
            for (int yy = 0; yy < f->height; ++yy) {
                if (c_arr[yy * f->width + xx]) {
                    framebuffer_8bit_draw_pixel(fb, x + xx + offset, y + yy, c);
                }
            }
        }
    }
}

void framebuffer_8bit_fill_triangle(Framebuffer8Bit_t *fb, const Vertex2d_t *triangle, const Color8Bit_t c) {
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
    if (xmax >= fb->width) {
        xmax = fb->width - 1;
    }
    if (ymin < 0) {
        ymin = 0;
    }
    if (ymax >= fb->height) {
        ymax = fb->height - 1;
    }

    float w0, w1, w2;
    Vertex2d_t p;
    for (int j = ymin; j <= ymax; ++j) {
        for (int i = xmin; i <= xmax; ++i) {
            p[0] = (float) i;
            p[1] = (float) j;
            w0 = edgeFunction(triangle[1], triangle[2], p);
            w1 = edgeFunction(triangle[2], triangle[0], p);
            w2 = edgeFunction(triangle[0], triangle[1], p);
            if (w0 > 0 && w1 > 0 && w2 > 0) {
                *framebuffer_8bit_pixel_at(fb, i, j) = c;
            }
        }
    }
}

#define Ax v1[0]
#define Ay v1[1]
#define Bx v2[0]
#define By v2[1]
#define Cx v3[0]
#define Cy v3[1]

static inline void vertex2d_swap(const float **a, const float **b) {
    const float *tmp = *a;
    *a = *b;
    *b = tmp;
}

void framebuffer_8bit_fill_triangle_fast(Framebuffer8Bit_t *fb, const Vertex2d_t *triangle, const Color8Bit_t c) {
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
            framebuffer_8bit_draw_horizontal_line(fb, (int) Sx, (int) Sy, (int) (Ex - Sx), c);
            Sx += dx2;
            Ex += dx1;
            ++Sy;
        }
    } else {
        while (Sy < Ey) {
            framebuffer_8bit_draw_horizontal_line(fb, (int) Sx, (int) Sy, (int) (Ex - Sx), c);
            Sx += dx1;
            Ex += dx2;
            ++Sy;
        }
    }

    Sy = By;
    Ey = Cy;
    if (Bx <= Cx) {
        Sx = Bx;
        while (Sy < Ey) {
            framebuffer_8bit_draw_horizontal_line(fb, (int) Sx, (int) Sy, (int) (Ex - Sx), c);
            Sx += dx3;
            Ex += dx2;
            ++Sy;
        }
    } else {
        Ex = Bx;
        while (Sy < Ey) {
            framebuffer_8bit_draw_horizontal_line(fb, (int) Sx, (int) Sy, (int) (Ex - Sx), c);
            Sx += dx2;
            Ex += dx3;
            ++Sy;
        }
    }
}

//void framebuffer_8bit_fill_triangle_texture(Framebuffer8Bit_t *fb, const Vertex2d_t *triangle, const Framebuffer8Bit_t *texture) {
//    int xmin = min(min(triangle[0][0], triangle[1][0]), triangle[2][0]);
//    int xmax = max(max(triangle[0][0], triangle[1][0]), triangle[2][0]);
//    int ymin = min(min(triangle[0][1], triangle[1][1]), triangle[2][1]);
//    int ymax = max(max(triangle[0][1], triangle[1][1]), triangle[2][1]);
//
//    if (xmax <= 0 || xmin >= fb->width) {
//        return;
//    }
//    if (ymax <= 0 || ymin >= fb->height) {
//        return;
//    }
//
//    if (xmin < 0) {
//        xmin = 0;
//    }
//    if (xmax >= fb->width) {
//        xmax = fb->width - 1;
//    }
//    if (ymin < 0) {
//        ymin = 0;
//    }
//    if (ymax >= fb->height) {
//        ymax = fb->height - 1;
//    }
//
//    float w0, w1, w2;
//    Vertex2d_t p;
//    for (int j = ymin; j <= ymax; ++j) {
//        for (int i = xmin; i <= xmax; ++i) {
//            p[0] = (float) i;
//            p[1] = (float) j;
//            w0 = edgeFunction(triangle[1], triangle[2], p);
//            w1 = edgeFunction(triangle[2], triangle[0], p);
//            w2 = edgeFunction(triangle[0], triangle[1], p);
//            if (w0 > 0 && w1 > 0 && w2 > 0) {
//                *framebuffer_8bit_pixel_at(fb, i, j) = *framebuffer_8bit_pixel_at(
//                        texture,
//                        ((float) (i - xmin) / (float) (xmax - xmin)) * (float) (texture->width - 1),
//                        ((float) (j - ymin) / (float) (ymax - ymin)) * (float) (texture->height - 1));
//            }
//        }
//    }
//}

void framebuffer_8bit_draw_framebuffer(Framebuffer8Bit_t *fb, const int x, const int y, const Framebuffer8Bit_t *src) {
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

void framebuffer_8bit_draw_framebuffer_scaled(Framebuffer8Bit_t *fb, const int center_x, const int center_y, const Framebuffer8Bit_t *src, const float scale,
                                              const int alpha) {
    assert(fb);
    assert(src);

    if (scale <= 0.0f) {
        return;
    }
    if (center_x >= fb->width) {
        return;
    }
    if (center_y >= fb->height) {
        return;
    }

    const float max_width = (scale * (float) src->width);
    const float max_height = (scale * (float) src->height);
    const float step_x = (float) src->width / max_width;
    const float step_y = (float) src->height / max_height;
    const int x = (int) ((float) center_x - (float) max_width * 0.5f);
    const int y = (int) ((float) center_y - (float) max_width * 0.5f);


    const int alphaEnabled = alpha >= 0;
    int pos_x = 0;
    int pos_y = 0;
    Color8Bit_t pixel = 0;

    for (int i = 0; i + x < fb->width && i < max_width; ++i) {
        for (int j = 0; j + y < fb->height && j < max_height; ++j) {
            if (i + x < 0) {
                continue;
            }
            if (j + y < 0) {
                continue;
            }
            pos_x = (int) ((float) i * step_x);
            pos_y = (int) ((float) j * step_y);
            pixel = *framebuffer_8bit_pixel_at(src, pos_x, pos_y);
            if (!alphaEnabled || alpha != pixel) {
                fb->buffer[((j + y) * fb->width) + i + x] = pixel;
            }
        }
    }
}

void framebuffer_8bit_draw_framebuffer_rotated(Framebuffer8Bit_t *fb, const int center_x, const int center_y, const Framebuffer8Bit_t *src, float angle,
                                               const int alpha) {
    assert(fb);
    assert(src);

    if (center_x >= fb->width) {
        return;
    }
    if (center_y >= fb->height) {
        return;
    }

    // normalize angle
//    angle = (float) ((((int) angle % 360) + 360) % 360);
    const float radians = deg_to_rad(-angle);

    const float cos_a = (float) cos(radians);
    const float sin_a = (float) sin(radians);
    const int max_length = (int) ((float) (max(src->width, src->height)) * 1.3f);
    const int dst_center_x = (int) ((float) max_length * 0.5f);
    const int dst_center_y = (int) ((float) max_length * 0.5f);
    const int x_offset = (int) ((float) (max_length - src->width) * 0.5f);
    const int y_offset = (int) ((float) (max_length - src->height) * 0.5f);
    const int alpha_enabled = alpha >= 0;

    Color8Bit_t pixel = 0;
    int pos_x = 0;
    int pos_y = 0;

    for (int x = 0; x < max_length; ++x) {
        for (int y = 0; y < max_length; ++y) {
            pos_x = (int) ((float) dst_center_x + (float) (x - dst_center_x) * cos_a - (float) (y - dst_center_y) * sin_a);
            pos_y = (int) ((float) dst_center_y + (float) (x - dst_center_x) * sin_a + (float) (y - dst_center_y) * cos_a);
            pos_x -= x_offset;
            pos_y -= y_offset;
            if (pos_x >= 0 && pos_x < src->width && pos_y >= 0 && pos_y < src->height) {
                pixel = *framebuffer_8bit_pixel_at(src, pos_x, pos_y);
                if (alpha_enabled && alpha == pixel) {
                    continue;
                }
                framebuffer_8bit_draw_pixel(fb, x + center_x - dst_center_x, y + center_y - dst_center_y, pixel);
            }
        }
    }
}


void framebuffer_fill_8bit(Framebuffer_t *fb, const Framebuffer8Bit_t *src, const Palette8Bit_t *p) {
    assert(src);
    assert(fb);
    assert(p);
    assert(src->width == fb->width);
    assert(src->height == fb->height);

    ColorRGB_t color;
    for (int x = 0; x < fb->width; ++x) {
        for (int y = 0; y < fb->height; ++y) {
            const Color8Bit_t pen_id = *framebuffer_8bit_pixel_at(src, x, y);
            const Pen_t *pen = palette_8bit_get_pen_const(p, pen_id);
            pen_to_color_rgb(pen, &color);
            framebuffer_draw_pixel_rgb(fb, x, y, &color);
        }
    }
}
