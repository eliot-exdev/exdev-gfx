#include <exdevgfx/exdev_base.h>
#include <exdevgfx/framebuffer_8bit.h>
#include <exdevgfx/window.h>

#define EXDEVGFX2_LOG_LEVEL 1

#include <exdevgfx/logger.h>

#define WIDTH 640
#define HEIGHT 480

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("usage: render_palette <palette>\n");
        return 0;
    }
    const char *infile = argv[1];

    int res = exdev_base_init();
    if (res) {
        log_warning_fmt("could no init exdev base=%d", res);
        return 1;
    }

    // load palette
    Palette8Bit_t p;
    res = palette_8bit_read_from_dat(&p, infile);
    if (res) {
        log_warning_fmt("could not read palette, res=%d", res);
        return 2;
    }
    log_info_fmt("number of pens=%d", p.numPens);
    int black_pen_pos = palette_8bit_find_pen(&p, &PEN_BLACK);
    if (black_pen_pos < 0) {
        log_info("could not find color black, use first color for background");
        black_pen_pos = 0;
    }

    Framebuffer8Bit_t fb;
    framebuffer_8bit_init(&fb, WIDTH, HEIGHT);
    framebuffer_8bit_fill(&fb, black_pen_pos);
    int x = 0, y = 0;
    for (int i = 0; i < p.numPens; ++i) {
        framebuffer_8bit_fill_rect(&fb, x, y, 20, 20, i);
        x += 20;
        if (x == WIDTH) {
            x = 0;
            y += 20;
        }
    }

    Window_t window = window_create(WIDTH, HEIGHT, "8bit", FS_8_BIT);
    window_update_palette(window, &p);
    window_fill_8bit(window, &fb);

    char closeEvent = 0;
    KeyEvent_t keyEvent;
    MouseEvent_t mouseEvent;
    while (!closeEvent) {
        window_poll_events(window, &closeEvent, &keyEvent, &mouseEvent, 1);
        if (keyEvent.event == KEY_EVENT_PRESSED && keyEvent.type == KEY_TYPE_ESC) {
            closeEvent = 1;
        }
    }

    window_destroy(window);
    framebuffer_8bit_deinit(&fb);

    return exdev_base_deinit();
}
