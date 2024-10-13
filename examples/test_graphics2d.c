#include <exdevgfx/framebuffer.h>

#include <stdio.h>

/*
 * compile for amiga:
 *  vc -c99 -Iinclude src/vertex3d.c src/graphics2d.c examples/test_graphics2d.c -o test_graphics2d -lmsoft -lauto -lamiga -O2 -speed -final
 *  vc -c99 -Iinclude src/vertex3d.c src/graphics2d.c examples/test_graphics2d.c -o test_graphics2d -fpu=68060-lm060 -lauto -lamiga -O2 -speed -final
 *
 * compile for morphos:
 *  vc -Iinclude src/vertex3d.c src/graphics2d.c examples/test_graphics2d.c -o test_graphics2d -lm -lauto -O3 -speed -final
 *
 * cross compile for amiga:
 *  vc +aos68k -c99 -Iinclude src/vertex3d.c src/graphics2d.c examples/test_graphics2d.c -o test_graphics2d -fpu=68060-lm060 -lauto -lamiga -O3 -speed -final
 *
 */


int main() {
    ColorRGBA_t c;
    Vertex2d_t triangle[3];
    ColorRGBA_t colors[3];

    Vertex2d_t p1, p2;

    Framebuffer_t gb;

    framebuffer_init(&gb, 320, 240);
    color_rgba_init(&c);

    framebuffer_fill(&gb, &c); /* fill black */

    vertex2d_set(p1, 0, 0);
    vertex2d_set(p2, 319, 239);
    color_rgba_set(&c, 0xff, 0xff, 0, 0xff);
    framebuffer_draw_line(&gb, p1, p2, &c); /* draw line from upper left to lower right */

    vertex2d_set(p1, 319, 0);
    vertex2d_set(p2, 0, 239);
    color_rgba_set(&c, 0x00, 0xff, 0xff, 0xff);
    framebuffer_draw_line(&gb, p1, p2, &c); /* draw line from upper right to lower left */

    color_rgba_set(&c, 0xff, 0, 0, 0xff);
    framebuffer_fill_rect(&gb, 0, 0, 20, 20, &c); /* draw red rect */

    color_rgba_set(&c, 0, 0xff, 0, 0xff);
    framebuffer_fill_rect(&gb, 20, 20, 20, 20, &c); /* draw yellow rect */

    color_rgba_set(&c, 0, 0, 0xff, 0xff);
    framebuffer_fill_rect(&gb, 40, 40, 20, 20, &c); /* draw blue rect */

    color_rgba_set(&c, 0xff, 0xff, 0xff, 0xff);
    framebuffer_draw_pixel(&gb, 319, 239, &c); /* draw pixel at bottom right */

    color_rgba_set(&c, 0x80, 0x80, 0x80, 0x80);
    framebuffer_fill_rect(&gb, 50, 50, 20, 20, &c); /* draw gray half transparent rect */

    vertex2d_set(triangle[0], 100, 100);
    vertex2d_set(triangle[1], 50, 160);
    vertex2d_set(triangle[2], 150, 160);
    color_rgba_set(&colors[0], 0xff, 0, 0, 0xff);
    color_rgba_set(&colors[1], 0, 0xff, 0, 0xff);
    color_rgba_set(&colors[2], 0, 0, 0xff, 0xff);
    framebuffer_fill_triangle(&gb, triangle, colors); /* fill triangle */

    color_rgba_set(&c, 0xff, 0xff, 0xff, 0xff);
    framebuffer_draw_triangle(&gb, triangle, &c); /* draw triangle outline */

    vertex2d_set(triangle[0], 130, 130);
    vertex2d_set(triangle[1], 90, 190);
    vertex2d_set(triangle[2], 190, 190);
    color_rgba_set(&colors[0], 0xff, 0, 0, 0x80);
    color_rgba_set(&colors[1], 0, 0xff, 0, 0x80);
    color_rgba_set(&colors[2], 0, 0, 0xff, 0x80);
    framebuffer_fill_triangle(&gb, triangle, colors); /* fill second triangle */

    if (framebuffer_save(&gb, "gb.ppm")) {
        printf("[ERROR] could not write graphic buffer\n");
    }

    framebuffer_deinit(&gb);

    return 0;
}
