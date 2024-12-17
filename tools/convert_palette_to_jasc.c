#include <exdevgfx/exdev_base.h>
#include <exdevgfx/framebuffer_8bit.h>

#define EXDEVGFX2_LOG_LEVEL 1

#include <exdevgfx/logger.h>
#include <string.h>

int main(int argc, char **argv) {
    // parse args
    if (argc != 3) {
        printf("usage: convert_palette_to_jasc: <palette dat> <palette jasc>\n");
        return 0;
    }
    const char *infile = argv[1];
    const char *outfile = argv[2];

    int res = exdev_base_init();
    if (res) {
        log_warning_fmt("could no init exdev base: %d", res);
        return 1;
    }

    // load palette
    Palette8Bit_t p;
    log_info_fmt("trying to read palette from: %s", infile);
    res = palette_8bit_read_from_dat(&p, infile);
    if (res) {
        log_warning_fmt("could not read palette, res: %d", res);
        return 2;
    }
    log_info_fmt("number of pens: %d", p.numPens);

    // save palette
    log_info_fmt("trying to save palette to: %s", outfile);
    res = palette_8bit_save_as_jasc(&p,outfile);
    if (res) {
        log_warning_fmt("could not save palette, res: %d", res);
        return 3;
    }

    // clean up
    exdev_base_deinit();

    return 0;
}
