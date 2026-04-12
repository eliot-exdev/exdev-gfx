#include <exdevgfx/framebuffer_8bit.h>

#define EXDEVGFX2_LOG_LEVEL 1

#include <exdevgfx/logger.h>

#include <stdlib.h>

int main(int argc, char **argv) {
    if (argc != 4) {
        printf("usage: add_pens_to_8bit <dat 8bit> <pal 8bit> <num pens>\n");
        return 0;
    }

    const char *path_dat = argv[1];
    const char *path_pal = argv[2];
    int num_pens = atoi(argv[3]);

    log_info_fmt("dat file: %s", path_dat);
    log_info_fmt("pal file: %s", path_pal);
    log_info_fmt("num pens: %d", num_pens);

    // load 8bit image
    Framebuffer8Bit_t fb_src;
    int res = framebuffer_8bit_read_from_dat(&fb_src, path_dat);
    if (res) {
        log_warning_fmt("could not read dat file, res: %d", res);
        return res;
    }

    // load palette
    Palette8Bit_t pal_src;
    res = palette_8bit_read_from_dat(&pal_src, path_pal);
    if (res) {
        log_warning_fmt("could not read pal file, res: %d", res);
        return res;
    }
    log_info_fmt("num existing pens: %d", pal_src.numPens);

    // add pens
    for (int y = 0; y < fb_src.height; ++y) {
        for (int x = 0; x < fb_src.width; ++x) {
            const Color8Bit_t pixel = *framebuffer_8bit_pixel_at(&fb_src, x, y);
            framebuffer_8bit_draw_pixel(&fb_src, x, y, pixel + num_pens);
        }
    }

    Palette8Bit_t pal_new;
    palette_8bit_init(&pal_new, num_pens);
    for (int i = 0; i < pal_src.numPens; ++i) {
        palette_8bit_add_pen(&pal_new, palette_8bit_get_pen_const(&pal_src, i));
    }

    // write result
    res = framebuffer_8bit_save_as_dat(&fb_src, path_dat);
    if (res) {
        log_warning_fmt("could not write dat file, res: %d", res);
        return res;
    }

    res = palette_8bit_save_as_dat(&pal_new, path_pal);
    if (res) {
        log_warning_fmt("could not write pal file, res: %d", res);
        return res;
    }
    log_info_fmt("num total pens: %d", pal_new.numPens);

    framebuffer_8bit_deinit(&fb_src);

    log_info_fmt("add %d pens", num_pens);
    return 0;
}
