#include <exdevgfx/exdev_base.h>
#include <exdevgfx/framebuffer_8bit.h>
#include <exdevgfx/window.h>
#include <exdevgfx/logger.h>
#include <exdevgfx/helper.h>
#include <exdevgfx/vertex2d.h>

#include <easing/Back.h>

#define WIDTH 640
#define HEIGHT 480
#define DURATION 4000 // in ms

static Vertex2d_t posSprite1 = {WIDTH * 0.33f, HEIGHT * 0.5f};
static Vertex2d_t posSprite2 = {WIDTH * 0.66f, HEIGHT * 0.5f};

static void paint(Window_t *window, Framebuffer8Bit_t *offscreen, Framebuffer8Bit_t *sprite, const float scale, const float rotate) {
    // clear buffer + draw
    framebuffer_8bit_fill(offscreen, 0);
    // render scale
    framebuffer_8bit_draw_framebuffer_scaled(offscreen, posSprite1[0], posSprite1[1], sprite, scale);
    // render rotate
    framebuffer_8bit_draw_framebuffer_rotated(offscreen, posSprite2[0], posSprite2[1], sprite, rotate);
    window_fill_8bit(window, offscreen);
}

#define MAX_KEY_EVENTS 1

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
    MouseEvent_t mouseEvent;

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
    int in = 1;
    while (!close_event) {
        // render
        paint(window, &offscreen, &sprite, scale, rotate);

        // handle events
        window_poll_events(window, &close_event, &keyEvent, &mouseEvent, MAX_KEY_EVENTS);

        if (keyEvent.event == KEY_EVENT_PRESSED && keyEvent.type == KEY_TYPE_ESC) {
            close_event = 1;
        }

        if (mouseEvent.event == MOUSE_EVENT_BUTTON_PRESSED) {
            if (mouseEvent.button == MOUSE_BUTTON_0) {
                posSprite1[0] = mouseEvent.position_x;
                posSprite1[1] = mouseEvent.position_y;
            } else if (mouseEvent.button == MOUSE_BUTTON_1) {
                posSprite2[0] = mouseEvent.position_x;
                posSprite2[1] = mouseEvent.position_y;
            }
        }

        // update animation
        t = now() - start;
        if (t > DURATION) {
            t = 0;
            start = now();
            if (in) {
                in = 0;
            } else {
                in = 1;
            }
        }
        if (in) {
            scale = easing_back_easeInOut((float) t, 1.0f, 2.0f, DURATION);
            rotate = easing_back_easeInOut((float) t, 0.0f, 360.f, DURATION);
        } else {
            scale = easing_back_easeInOut((float) t, 3.0f, -2.0f, DURATION);
            rotate = easing_back_easeInOut((float) t, 360.0f, -360.0f, DURATION);
        }
    }

    // deinit
    framebuffer_8bit_deinit(&offscreen);
    framebuffer_8bit_deinit(&sprite);
    window_destroy(window);
    exdev_base_deinit();

    return 0;
}
