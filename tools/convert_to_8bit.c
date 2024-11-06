#include <exdevgfx/framebuffer_8bit.h>

#define EXDEVGFX2_LOG_LEVEL 1

#include <exdevgfx/logger.h>

int main(int argc, char **argv) {
    if (argc != 4) {
        printf("usage: convert_to_8bit <dat 24bit> <dat 8bit> <palette>\n");
        return 0;
    }

    const char *path_in = argv[1];
    const char *path_out = argv[2];
    const char *path_palette_out = argv[3];

    log_info_fmt("infile=%s", path_in);
    log_info_fmt("outfile=%s", path_out);
    log_info_fmt("outfile palette=%s", path_palette_out);

    // load 24 bit image
    Framebuffer_t fb;
    int res = framebuffer_read_from_dat(&fb, path_in);
    if (res) {
        log_warning_fmt("could not read infile, res=%d", res);
        return res;
    }

    // convert to 8 bit image
    Framebuffer8Bit_t f;
    Palette8Bit_t p;

    res = palette_8bit_read_from_dat(&p, path_palette_out);
    if (res) {
        log_info("no palette existed, will create new one");
    } else {
        log_debug_fmt("num pens existing: %d", p.numPens);
    }

    res = framebuffer_8bit_init_from_framebuffer(&f, &p, &fb, 0);
    if (res) {
        log_warning("could not convert to 8 bit");
        return res;
    }
    log_info_fmt("num of pens: %d", p.numPens);

    // save 8 bit image
    res = framebuffer_8bit_save_as_dat(&f, path_out);
    if (res) {
        log_warning_fmt("could not write outfile, res=%d", res);
        return res;
    }

    // save palette
    res = palette_8bit_save_as_dat(&p, path_palette_out);
    if (res) {
        log_warning_fmt("could not write palette, res=%d", res);
        return res;
    }

    return 0;
}
