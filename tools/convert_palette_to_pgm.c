#include <exdevgfx/exdev_base.h>
#include <exdevgfx/framebuffer_8bit.h>
#include <exdevgfx/framebuffer.h>

#define EXDEVGFX2_LOG_LEVEL 1

#include <exdevgfx/logger.h>
#include <string.h>

#define WIDTH 640
#define HEIGHT 320

#define MAX_PENS_PER_ROW 32
#define MAX_ROWS 8
#define PEN_WIDTH 20
#define PEN_HEIGHT 20

int main(int argc, char **argv) {
    // parse args
    if (argc != 3) {
        printf("usage: convert_palette_to_pgm: <palette> <pgm>\n");
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

    // create font
    Font_t mia1;
    font_init_mia_1(&mia1);

    // create image
    Framebuffer8Bit_t fb;
    framebuffer_8bit_init(&fb, WIDTH, HEIGHT);
    framebuffer_8bit_fill(&fb, 0);

    // render image
    char text[8];
    for (int row = 0; row < MAX_ROWS; row++) {
        for (int col = 0; col < MAX_PENS_PER_ROW; col++) {
            const int pos = row * MAX_PENS_PER_ROW + col;
            if (pos >= p.numPens) {
                break;
            }
            framebuffer_8bit_fill_rect(&fb, col * PEN_WIDTH, row * (PEN_HEIGHT + PEN_HEIGHT), PEN_WIDTH, PEN_HEIGHT, pos);
            memset(text, 0, 8);
            sprintf(text, "%d", pos);
            framebuffer_8bit_draw_text(&fb,
                                       &mia1,
                                       text,
                                       strlen(text),
                                       p.numPens - 1,
                                       col * PEN_WIDTH,
                                       row * (PEN_HEIGHT + PEN_HEIGHT) + PEN_HEIGHT + 1);
        }
    }

    // save image as pgm
    Framebuffer_t fb_pgm;
    framebuffer_init(&fb_pgm, WIDTH, HEIGHT);
    framebuffer_fill_8bit(&fb_pgm, &fb, &p);
    res = framebuffer_save(&fb_pgm, outfile);
    if (res) {
        log_warning("could not save pgm");
    } else {
        log_info_fmt("saved pgm to: %s", outfile);
    }

    // clean up
    font_deinit(&mia1);
    framebuffer_8bit_deinit(&fb);
    framebuffer_deinit(&fb_pgm);
    exdev_base_deinit();

    return 0;
}