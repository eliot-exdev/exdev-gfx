#include <exdevgfx/exdev_base.h>
#include <exdevgfx/framebuffer_8bit.h>
#include <exdevgfx/window.h>
#include <exdevgfx/logger.h>
#include <exdevgfx/helper.h>

//#include <easing/Sine.h>
#include <easing/Back.h>
//#include <easing/Bounce.h>
//#include <easing/Elastic.h>
//#include <easing/Cubic.h>
//#include <easing/Linear.h>
//#include <easing/Quad.h>
//#include <easing/Quint.h>

#define WIDTH 640
#define HEIGHT 480
#define DURATION 4000

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

//static float scale_step = STEP_SCALE;
//static float rotate_step = STEP_ROTATE;

//static float update(const float t, const float from, const float change, const int in) {
//    if (in) {
//        return easing_linear_easeIn(t, from, change, DURATION);
//    }
//    return easing_linear_easeOut(t, from, change, DURATION);
//}

//static float updateRotate(const TIMESTAMP s, const int in) {
//    const float t = (float) (now() - s);
//    if(in)
//    return easing_back_easeInOut(t, 0.0f, 360.0f, DURATION);
//}

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
    if (!(window = window_create(WIDTH, HEIGHT, "Test Window", FS_8_BIT))) {
        log_warning("could not create window");
        return 3;
    }
    window_update_palette(window, &palette);

    TIMESTAMP start = now();
    TIMESTAMP t = 0;
    while (!close_event) {
        // render
        paint(window, &offscreen, &sprite, scale, rotate);

        // handle events
        eventCount = window_poll_events(window, &close_event, &keyEvent, 1);
        if (eventCount && keyEvent.event == KEY_EVENT_PRESSED && keyEvent.type == KEY_TYPE_ESC) {
            close_event = 1;
        }

        // update animation
        t = now() - start;
        if (t > DURATION) {
            t = 0;
            start = now();
        }
        scale = easing_back_easeInOut(t, 1.0f, 2.0f, DURATION);
        rotate =easing_back_easeInOut(t, 0.0f, 360.f, DURATION);
    }

    // deinit
    framebuffer_8bit_deinit(&offscreen);
    framebuffer_8bit_deinit(&sprite);
    window_destroy(window);
    exdev_base_deinit();

    return 0;
}
