#define EXDEVGFX2_LOG_LEVEL 2

#include <exdevgfx/exdev_base.h>
#include <exdevgfx/framebuffer_8bit.h>
#include <exdevgfx/window.h>
#include <exdevgfx/logger.h>
#include <exdevgfx/helper.h>

#include <easing/Back.h>
#include <easing/Bounce.h>
#include <easing/Circ.h>
#include <easing/Cubic.h>
#include <easing/Elastic.h>
#include <easing/Expo.h>
#include <easing/Linear.h>
#include <easing/Quad.h>
#include <easing/Quart.h>
#include <easing/Quint.h>
#include <easing/Sine.h>

#define WIDTH 640
#define HEIGHT 480
#define DURATION 4000// in ms
#define NUM_FUNCTIONS 11

static Vertex2d_t posSprite1 = {WIDTH * 0.33f, HEIGHT * 0.5f};
static Vertex2d_t posSprite2 = {WIDTH * 0.66f, HEIGHT * 0.5f};

typedef float (*penner_functions_t)(float t, float b, float c, float d);

static penner_functions_t functions[NUM_FUNCTIONS];

static int functions_index = 0;

static void paint(Window_t *window, Framebuffer8Bit_t *offscreen, Framebuffer8Bit_t *sprite, const float scale,
                  const float rotate) {
    // clear buffer + draw
    framebuffer_8bit_fill(offscreen, 31);
    // render scale
    framebuffer_8bit_draw_framebuffer_scaled(offscreen, posSprite1[0], posSprite1[1], sprite, scale, 0);
    // render rotate
    framebuffer_8bit_draw_framebuffer_rotated(offscreen, posSprite2[0], posSprite2[1], sprite, rotate, 0);
    window_fill_8bit(window, offscreen);
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
    Event_t event;

    // functions
    functions[0] = easing_back_easeInOut;
    functions[1] = easing_bounce_easeInOut;
    functions[2] = easing_circ_easeInOut;
    functions[3] = easing_cubic_easeInOut;
    functions[4] = easing_elastic_easeIn;
    functions[5] = easing_expo_easeIn;
    functions[6] = easing_linear_easeIn;
    functions[7] = easing_quad_easeIn;
    functions[8] = easing_quart_easeIn;
    functions[9] = easing_quint_easeIn;
    functions[10] = easing_sine_easeIn;

    // init
    exdev_base_init();
    framebuffer_8bit_init(&offscreen, WIDTH, HEIGHT);
    if (framebuffer_8bit_read_from_dat(&sprite, "assets/guybrush_8bit.dat")) {
        log_warning("could not read sprite");
        return 1;
    }
    if (palette_8bit_read_from_dat(&palette, "assets/guybrush_8bit_pallette.dat")) {
        log_warning("could not read palette");
        return 2;
    }
    if (!(window = window_create(WIDTH, HEIGHT, "Test Window", FS_8_BIT))) {
        log_warning("could not create window");
        return 3;
    }
    window_update_palette(window, &palette);

    exdev_timestamp_t start = now();
    exdev_timestamp_t t = 0;
    int in = 1;
    while (!close_event) {
        // render
        paint(window, &offscreen, &sprite, scale, rotate);

        // handle events
        window_poll_events(window, &close_event, &event, 1);

        if (event.type == EVENT_KEY && event.key_event.event == KEY_EVENT_PRESSED) {
            if (event.key_event.key == KEY_TYPE_ESC) {
                close_event = 1;
            } else if (event.key_event.code == 's') {
                ++functions_index;
                if (functions_index == NUM_FUNCTIONS) {
                    functions_index = 0;
                }
                log_info_fmt("switched to easing function=%d", functions_index);
            }
        }

        if (event.type == EVENT_MOUSE && event.mouse_event.event == MOUSE_EVENT_BUTTON_PRESSED) {
            if (event.mouse_event.button == MOUSE_BUTTON_0) {
                posSprite1[0] = event.mouse_event.position_x;
                posSprite1[1] = event.mouse_event.position_y;
            } else if (event.mouse_event.button == MOUSE_BUTTON_1) {
                posSprite2[0] = event.mouse_event.position_x;
                posSprite2[1] = event.mouse_event.position_y;
            } else if (event.mouse_event.button == MOUSE_BUTTON_2) {
                log_info("middle mouse button pressed");
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
            scale = functions[functions_index]((float) t, 1.0f, 2.0f, DURATION);
            rotate = functions[functions_index]((float) t, 0.0f, 360.f, DURATION);
        } else {
            scale = functions[functions_index]((float) t, 3.0f, -2.0f, DURATION);
            rotate = functions[functions_index]((float) t, 360.0f, -360.0f, DURATION);
        }
    }

    // deinit
    framebuffer_8bit_deinit(&offscreen);
    framebuffer_8bit_deinit(&sprite);
    window_destroy(window);
    exdev_base_deinit();

    return 0;
}
