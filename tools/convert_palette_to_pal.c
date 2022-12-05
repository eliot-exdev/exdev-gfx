#include <exdevgfx/exdev_base.h>
#include <exdevgfx/palette.h>

#define EXDEVGFX2_LOG_LEVEL 1

#include <exdevgfx/logger.h>

int main(int argc, char **argv) {
    // parse args
    if (argc != 3) {
        printf("usage: convert_palette_to_pal: <in.dat> <out.pal>\n");
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
    log_info_fmt("try reading palette from: %s", infile);
    res = palette_8bit_read_from_dat(&p, infile);
    if (res) {
        log_warning_fmt("could not read palette, res: %d", res);
        return 2;
    }
    log_info_fmt("number of pens: %d", p.numPens);

    res = palette_save_as_pal(&p,outfile);
    if (res) {
        log_warning("could not save pal");
    } else {
        log_info_fmt("saved pal to: %s", outfile);
    }

    // clean up
    exdev_base_deinit();

    return 0;
}