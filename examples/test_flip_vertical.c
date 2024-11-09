#define EXDEVGFX2_LOG_LEVEL 2

#include <exdevgfx/exdev_base.h>
#include <exdevgfx/framebuffer_8bit.h>
#include <exdevgfx/logger.h>

#include <stdio.h>

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("usgae: %s <input.dat> <output.dat>\n", argv[0]);
        return 1;
    }

    char *path_in = argv[1];
    char *path_out = argv[2];
    log_info_fmt("in=%s", path_in);
    log_info_fmt("out=%s", path_out);

    int res = exdev_base_init();
    if (res) {
        log_warning("could not init exdevgfx");
        return 2;
    }

    Framebuffer8Bit_t fb_in;
    Framebuffer8Bit_t fb_out;

    res = framebuffer_8bit_read_from_dat(&fb_in, path_in);
    if (res) {
        log_warning_fmt("could not read input=%s", path_in);
        return 3;
    }

    framebuffer_8bit_init(&fb_out, fb_in.width * 2, fb_in.height);
    framebuffer_8bit_draw_framebuffer(&fb_out, 0, 0, &fb_in);
    framebuffer_8bit_draw_framebuffer_flip_vertical(&fb_out, fb_in.width, 0, &fb_in);
    framebuffer_8bit_save_as_dat(&fb_out, path_out);

    framebuffer_8bit_deinit(&fb_in);
    framebuffer_8bit_deinit(&fb_out);

    exdev_base_deinit();
    return 0;
}