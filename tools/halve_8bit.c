#include <exdevgfx/framebuffer_8bit.h>

#define EXDEVGFX2_LOG_LEVEL 1

#include <exdevgfx/logger.h>

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("usage: convert_to_8bit <dat 24bit> <dat 8bit>\n");
        return 0;
    }

    const char *path_in = argv[1];
    const char *path_out = argv[2];

    log_info_fmt("infile: %s", path_in);
    log_info_fmt("outfile: %s", path_out);

    // load 8 bit image
    Framebuffer8Bit_t fb_src;
    int res = framebuffer_8bit_read_from_dat(&fb_src, path_in);
    if (res) {
        log_warning_fmt("could not read infile, res: %d", res);
        return res;
    }

    log_info_fmt("src dimensions: %dx%d", fb_src.width, fb_src.height);
    log_info_fmt("outfile=%s", path_out);

    const int width = (int) ((float) fb_src.width * 0.5f);
    const int height = (int) ((float) fb_src.height * 0.5f);

    Framebuffer8Bit_t fb_out;
    framebuffer_8bit_init(&fb_out, width, height);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            framebuffer_8bit_draw_pixel(&fb_out, x, y, *framebuffer_8bit_pixel_at(&fb_src, x * 2, y * 2));
        }
    }

    res = framebuffer_8bit_save_as_dat(&fb_out, path_out);
    if (res) {
        log_warning_fmt("could not read infile, res: %d", res);
        return res;
    }

    framebuffer_8bit_deinit(&fb_src);
    framebuffer_8bit_deinit(&fb_out);

    return 0;
}
