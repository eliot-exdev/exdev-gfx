#include <exdevgfx/exdev_base.h>
#include <exdevgfx/framebuffer_8bit.h>
#include <exdevgfx/window.h>
#include <exdevgfx/logger.h>
#define WIDTH 640
#define HEIGHT 480
#define STEP 0.1f

static void paint(Window_t *window, Framebuffer8Bit_t *offscreen, Framebuffer8Bit_t *sprite, const float scale) {
    // calc center
    const int x = (int) (((float) offscreen->width - (float) sprite->width * scale) * 0.5f);
    const int y = (int) (((float) offscreen->height - (float) sprite->height * scale) * 0.5f);

    // clear buffer + draw
    framebuffer_8bit_fill(offscreen, 0);
    framebuffer_8bit_draw_framebuffer_scaled(offscreen, x, y, sprite, scale);
    window_fill_8bit(window, offscreen);
}

int main() {
    // variables
    float scale = 1.0f;
    Framebuffer8Bit_t offscreen;
    Framebuffer8Bit_t sprite;
    Palette8Bit_t palette;
    Window_t *window = NULL;
    char close_event = 0;
    KeyEvent_t keyEvent;
    int eventCount = 0;
    float step = 0.0f;

    // init
    exdev_base_init();
    framebuffer_8bit_init(&offscreen, WIDTH, HEIGHT);
    if (framebuffer_8bit_read_from_dat(&sprite, "assets/texture_8bit.dat")) {
        log_warning("could not read sprite");
        return 1;
    }
    if (palette_8bit_read_from_dat(&palette, "assets/texture_8bit_palette.dat")) {
        log_warning("could not read palette");
        return 2;
    }
    if(!(window = window_create(WIDTH, HEIGHT, "Test Window", 0))){
        log_warning("could not create window");
        return 3;
    }
    window_update_palette(window, &palette);

    // loop events and render
    while (!close_event) {
        // render
        paint(window, &offscreen, &sprite, scale);

        // handle events
        eventCount = window_poll_events(window, &close_event, &keyEvent, 1);
        if (eventCount && keyEvent.event == KEY_EVENT_PRESSED && keyEvent.type == KEY_TYPE_ESC) {
            close_event = 1;
        } else if (eventCount && keyEvent.event == KEY_EVENT_PRESSED && keyEvent.type == KEY_TYPE_CODE &&
                   keyEvent.code == '-') {
            step = -STEP;
        } else if (eventCount && keyEvent.event == KEY_EVENT_RELEASED && keyEvent.type == KEY_TYPE_CODE &&
                   keyEvent.code == '-') {
            step = 0.0f;
        } else if (eventCount && keyEvent.event == KEY_EVENT_PRESSED && keyEvent.type == KEY_TYPE_CODE &&
                   keyEvent.code == '+') {
            step = STEP;
        } else if (eventCount && keyEvent.event == KEY_EVENT_RELEASED && keyEvent.type == KEY_TYPE_CODE &&
                   keyEvent.code == '+') {
            step = 0.0f;
        }
        scale += step;
    }

    // deinit
    framebuffer_8bit_deinit(&offscreen);
    framebuffer_8bit_deinit(&sprite);
    window_destroy(window);
    exdev_base_deinit();

    return 0;
}
