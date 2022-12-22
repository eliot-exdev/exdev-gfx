#include <exdevgfx/exdev_base.h>
#include <exdevgfx/framebuffer_8bit.h>
#include <exdevgfx/window.h>
#include <exdevgfx/logger.h>

#define WIDTH 640
#define HEIGHT 480
#define STEP_SCALE 0.05f
#define STEP_ROTATE 3.0f

static void paint(Window_t *window, Framebuffer8Bit_t *offscreen, Framebuffer8Bit_t *sprite, const float scale, const float rotate) {
    (void) rotate;
    // calc center

    // clear buffer + draw
    framebuffer_8bit_fill(offscreen, 0);
    // render scale
    framebuffer_8bit_draw_framebuffer_scaled(offscreen, WIDTH * 0.33f, HEIGHT * 0.5f, sprite, scale);
    // render rotate
    framebuffer_8bit_draw_framebuffer_rotated(offscreen, WIDTH * 0.66f, HEIGHT * 0.5f, sprite, rotate);
    window_fill_8bit(window, offscreen);
}

static float scale_step = STEP_SCALE;
static float rotate_step = STEP_ROTATE;

static float updateScale(const float scale) {
    if (scale > 3.0f) {
        scale_step = -STEP_SCALE;
    } else if (scale < 0.1f) {
        scale_step = +STEP_SCALE;
    }
    return scale + scale_step;
}

static float updateRotate(const float rotate) {
    if (rotate > 360.0f) {
        rotate_step = -STEP_ROTATE;
    } else if (rotate < 0.0f) {
        rotate_step = +STEP_SCALE;
    }
    return rotate + rotate_step;
}

int main() {
    // variables
    float scale = 1.0f;
    float rotate = 0.0f;
    Framebuffer8Bit_t offscreen;
    Framebuffer8Bit_t sprite;
    Palette8Bit_t palette;
    Window_t *window = NULL;
    char close_event = 0;
    KeyEvent_t keyEvent;
    int eventCount = 0;


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
    if (!(window = window_create(WIDTH, HEIGHT, "Test Window", 0))) {
        log_warning("could not create window");
        return 3;
    }
    window_update_palette(window, &palette);

    // loop events and render
    while (!close_event) {
        // render
        paint(window, &offscreen, &sprite, scale, rotate);

        // handle events
        eventCount = window_poll_events(window, &close_event, &keyEvent, 1);
        if (eventCount && keyEvent.event == KEY_EVENT_PRESSED && keyEvent.type == KEY_TYPE_ESC) {
            close_event = 1;
        }
//        else if (eventCount && keyEvent.event == KEY_EVENT_PRESSED && keyEvent.type == KEY_TYPE_LEFT) {
//            step_rotate -= STEP_ROTATE;
//        } else if (eventCount && keyEvent.event == KEY_EVENT_PRESSED && keyEvent.type == KEY_TYPE_RIGHT) {
//            step_rotate += STEP_ROTATE;
//        } else if (eventCount && keyEvent.event == KEY_EVENT_RELEASED && keyEvent.type == KEY_TYPE_LEFT) {
//            step_rotate = 0.0f;
//        } else if (eventCount && keyEvent.event == KEY_EVENT_RELEASED && keyEvent.type == KEY_TYPE_RIGHT) {
//            step_rotate = 0.0f;
//        } else if (eventCount && keyEvent.event == KEY_EVENT_PRESSED && keyEvent.type == KEY_TYPE_CODE &&
//                   keyEvent.code == '-') {
//            step_scale = -STEP_SCALE;
//        } else if (eventCount && keyEvent.event == KEY_EVENT_RELEASED && keyEvent.type == KEY_TYPE_CODE &&
//                   keyEvent.code == '-') {
//            step_scale = 0.0f;
//        } else if (eventCount && keyEvent.event == KEY_EVENT_PRESSED && keyEvent.type == KEY_TYPE_CODE &&
//                   keyEvent.code == '+') {
//            step_scale = STEP_SCALE;
//        } else if (eventCount && keyEvent.event == KEY_EVENT_RELEASED && keyEvent.type == KEY_TYPE_CODE &&
//                   keyEvent.code == '+') {
//            step_scale = 0.0f;
//        }
        scale = updateScale(scale);
        rotate= updateRotate(rotate);
    }

    // deinit
    framebuffer_8bit_deinit(&offscreen);
    framebuffer_8bit_deinit(&sprite);
    window_destroy(window);
    exdev_base_deinit();

    return 0;
}
