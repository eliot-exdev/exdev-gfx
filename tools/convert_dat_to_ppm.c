#include <stdio.h>

#define EXDEVGFX2_LOG_LEVEL 1

#include <exdevgfx/logger.h>
#include <exdevgfx/framebuffer.h>

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("usage: convert_dat_to_ppm <dat> <ppm>\n");
        return 0;
    }

    const char *path_in = argv[1];
    const char *path_out = argv[2];

    log_info_fmt("infile=%s", path_in);
    log_info_fmt("outfile=%s", path_out);

    Framebuffer_t fb;
    int res = framebuffer_read_from_dat(&fb, path_in);
    if (res) {
        log_warning_fmt("could not read infile, res=%d", res);
        return res;
    }

    res = framebuffer_save(&fb, path_out);
    if (res) {
        log_warning_fmt("could not write outfile, res=%d", res);
        return res;
    }

    log_info("done");
    return 0;
}
