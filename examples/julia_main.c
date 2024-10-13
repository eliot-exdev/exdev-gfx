#include <exdevgfx/exdev_base.h>
#include <exdevgfx/framebuffer.h>
#include <exdevgfx/window.h>
#include <exdevgfx/julia.h>
#include <exdevgfx/args.h>
#include <stdio.h>
#include <stdlib.h>

#define EXDEVGFX2_LOG_LEVEL 2

#include <exdevgfx/logger.h>

static int width = 640;
static int height = 480;
static int max_it = 50;

#define IMAGE_PATH "julia.ppm"

static void print_help() {
    printf("julia  [OPTIONS]...\n"
           "options:\n"
           " -h  --help              print help message and exit\n"
           "     --width <par>       the width of the window (default 640)\n"
           "     --height <par>      the height of the window (default 480)\n"
           "     --iterations <par>  maximal iteration (default 50)\n"
           "\n"
           "keyboard commands:\n"
           "     ESC or q                   quit\n"
           "     +                          zoom in\n"
           "     -                          zoom out\n"
           "     s                          save current view\n"
           "     r                          reset view\n"
           "     cursor up/down/left/right  navigate\n");
}

static void parse_args(const int argc, char **argv) {
    if (args_find_option(argc, argv, 'h', "help")) {
        print_help();
        exit(0);
    }
    const char *ptr = NULL;
    if ((ptr = args_get_long_option_parameter(argc, argv, "width")) != NULL) {
        width = atoi(ptr);
    }
    if ((ptr = args_get_long_option_parameter(argc, argv, "height")) != NULL) {
        height = atoi(ptr);
    }
    if ((ptr = args_get_long_option_parameter(argc, argv, "iterations")) != NULL) {
        max_it = atoi(ptr);
    }

    log_info_fmt("width=%d, height=%d, iterations=%d", width, height, max_it);
}

int main(int argc, char **argv) {
    int res = exdev_base_init();
    if (res) {
        log_warning_fmt("could no init exdev base=%d", res);
        return 1;
    }

    parse_args(argc, argv);

    Framebuffer_t fb;
    Julia_t julia;
    Window_t *window;
    KeyEvent_t keyEvents;
    MouseEvent_t mouseEvents;

    char close_event = 0;

    framebuffer_init(&fb, width, height);
    julia_init(&julia);
    julia.max_it = max_it;
    julia_paint(&julia, &fb);

    window = window_create(width, height, "Julia", FS_NO);
    window_fill(window, &fb);

    while (!close_event) {
        window_poll_events(window, &close_event, &keyEvents, &mouseEvents, 1);
        if (keyEvents.event == KEY_EVENT_PRESSED) {
            if (keyEvents.type == KEY_TYPE_ESC) {
                close_event = 1;
            } else if (keyEvents.type == KEY_TYPE_UP) {
                julia_move_up(&julia);
                julia_paint(&julia, &fb);
                window_fill(window, &fb);
            } else if (keyEvents.type == KEY_TYPE_DOWN) {
                julia_move_down(&julia);
                julia_paint(&julia, &fb);
                window_fill(window, &fb);
            } else if (keyEvents.type == KEY_TYPE_LEFT) {
                julia_move_left(&julia);
                julia_paint(&julia, &fb);
                window_fill(window, &fb);
            } else if (keyEvents.type == KEY_TYPE_RIGHT) {
                julia_move_right(&julia);
                julia_paint(&julia, &fb);
                window_fill(window, &fb);
            } else if (keyEvents.type == KEY_TYPE_CODE) {
                switch (keyEvents.code) {
                    case 'q':
                        close_event = 1;
                        break;
                    case '+':
                        julia_move_zoom_in(&julia);
                        julia_paint(&julia, &fb);
                        window_fill(window, &fb);
                        break;
                    case '-':
                        julia_move_zoom_out(&julia);
                        julia_paint(&julia, &fb);
                        window_fill(window, &fb);
                        break;
                    case 'r':
                        julia_init(&julia);
                        julia_paint(&julia, &fb);
                        window_fill(window, &fb);
                        break;
                    case 's':
                        framebuffer_save(&fb, IMAGE_PATH);
                        log_info_fmt("saved image to:%s", IMAGE_PATH);
                }
            }
        }
    }

    // clean up
    window_destroy(window);
    framebuffer_deinit(&fb);

    return exdev_base_deinit();
}
