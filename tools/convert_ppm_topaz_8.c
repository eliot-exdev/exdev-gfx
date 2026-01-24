/**
 * Copyright 2026 Andre Geisler (andre@exdev.de)
 */

#include <exdevgfx/framebuffer.h>
#include <exdevgfx/exdev_base.h>
#include <exdevgfx/logger.h>

#include <stdio.h>

static void print_char(const int y_offset, const int num_char, const Framebuffer_t *fb) {
    printf("{");
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            const ColorRGB_t *color = framebuffer_pixel_at(fb, num_char * 8 + x, y_offset + y);
            printf("%d", color->r != 0 || color->g != 0 || color->b != 0);
            if (!(x == 7 && y == 7)) {
                printf(", ");
            }
        }
    }
    printf("},\n");
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("usage: convert_ppm_topaz_8 <ppm>\n");
        return 0;
    }

    const char *path_in = argv[1];

    log_info_fmt("infile=%s", path_in);

    exdev_base_init();

    Framebuffer_t fb;
    int res = framebuffer_read(&fb, path_in);
    if (res) {
        log_warning_fmt("could not read infile, res=%d", res);
        return res;
    }

    printf("{\n");

    // row 0
    for (int c = 0; c < 26; ++c) {
        print_char(0, c, &fb);
    }
    // row 1
    for (int c = 0; c < 26; ++c) {
        print_char(10, c, &fb);
    }
    // row 2
    for (int c = 0; c < 26; ++c) {
        print_char(20, c, &fb);
    }

    printf("}\n");

    framebuffer_deinit(&fb);
    exdev_base_init();

    return 0;
}