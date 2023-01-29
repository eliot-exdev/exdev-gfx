#include <exdevgfx/framebuffer.h>
#include <exdevgfx/window.h>
#include <stddef.h>

#define WIDTH 640
#define HEIGHT 480

static void paint(Framebuffer_t *gb) {
    ColorRGBA_t c;
    Vertex2d_t triangle[3];
    Vertex2d_t p1, p2;
    ColorRGBA_t colors[3];

    color_rgba_init(&c);
    framebuffer_fill(gb, &c); /* fill black */

    vertex2d_set(p1, 0, 0);
    vertex2d_set(p2, WIDTH - 1, HEIGHT - 1);
    color_rgba_set(&c, 0xff, 0xff, 0, 0xff);
    framebuffer_draw_line(gb, p1, p2, &c); /* draw line from upper left to lower right */

    vertex2d_set(p1, WIDTH - 1, 0);
    vertex2d_set(p2, 0, HEIGHT - 1);
    color_rgba_set(&c, 0x00, 0xff, 0xff, 0xff);
    framebuffer_draw_line(gb, p1, p2, &c); /* draw line from upper right to lower left */

    color_rgba_set(&c, 0xff, 0, 0, 0xff);
    framebuffer_fill_rect(gb, 0, 0, 20, 20, &c); /* draw red rect */

    color_rgba_set(&c, 0, 0xff, 0, 0xff);
    framebuffer_fill_rect(gb, 20, 20, 20, 20, &c); /* draw yellow rect */

    color_rgba_set(&c, 0, 0, 0xff, 0xff);
    framebuffer_fill_rect(gb, 40, 40, 20, 20, &c); /* draw blue rect */

    vertex2d_set(triangle[0], 320, 100);
    vertex2d_set(triangle[1], 100, 380);
    vertex2d_set(triangle[2], 540, 380);
    color_rgba_set(&colors[0], 0xff, 0, 0, 0xff);
    color_rgba_set(&colors[1], 0, 0xff, 0, 0xff);
    color_rgba_set(&colors[2], 0, 0, 0xff, 0xff);
    framebuffer_fill_triangle(gb, triangle, colors); /* fill colored triangle */

    color_rgba_set(&c, 0xff, 0xff, 0xff, 0xff);
    framebuffer_draw_triangle(gb, triangle, &c); /* draw triangle outline */

    color_rgba_set(&c, 0xff, 0xff, 0x00, 0x80);
    framebuffer_fill_rect(gb, 50, 50, 320, 240, &c); /* draw gray half transparent rect */

}

int main() {
    Framebuffer_t gb;
    Window_t *window;
    char close_event = 0;

    framebuffer_init(&gb, WIDTH, HEIGHT);
    paint(&gb);

    window = window_create(WIDTH, HEIGHT, "Test Window", 0);
    window_fill(window, &gb);

    while (!close_event) {
        window_poll_events(window, &close_event, NULL, NULL, 0);
    }

    window_destroy(window);

    framebuffer_deinit(&gb);

    return 0;
}
