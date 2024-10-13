#include <exdevgfx/palette.h>
#include <stdio.h>
#include <assert.h>

int main() {
    Palette8Bit_t palette;
    palette_8bit_init(&palette, 0xff);
    printf("pen count=%d\n", palette.numPens);

    {
        // test color to pen max
        ColorRGB_t color;
        color_rgb_set(&color, 0xff, 0xff, 0xff);
        Pen_t *pen = palette_8bit_get_pen(&palette, 0);
        pen_set(pen, &color);

        printf("color=%d %d %d\n", (int) color.r, (int) color.g, (int) color.b);
        printf("pen=%u %u %u\n", pen->r, pen->g, pen->b);

        assert(pen->r == PEN_MAX);
        assert(pen->g == PEN_MAX);
        assert(pen->b == PEN_MAX);
    }

    {
        // test pen to color min
        Pen_t *pen = palette_8bit_get_pen(&palette, 0);
        pen_init(pen, PEN_MAX, PEN_MAX, PEN_MAX);
        ColorRGB_t color;
        pen_to_color_rgb(pen, &color);

        printf("color=%d %d %d\n", (int) color.r, (int) color.g, (int) color.b);
        printf("pen=%u %u %u\n", pen->r, pen->g, pen->b);

        assert(color.r == 0xff);
        assert(color.g == 0xff);
        assert(color.b == 0xff);
    }

    {
        // test color to pen max
        ColorRGB_t color;
        color_rgb_set(&color, 0, 0, 0);
        Pen_t *pen = palette_8bit_get_pen(&palette, 0);
        pen_set(pen, &color);

        printf("color=%d %d %d\n", (int) color.r, (int) color.g, (int) color.b);
        printf("pen=%u %u %u\n", pen->r, pen->g, pen->b);

        assert(pen->r == 0);
        assert(pen->g == 0);
        assert(pen->b == 0);
    }

    {
        // test pen to color max
        Pen_t *pen = palette_8bit_get_pen(&palette, 0);
        pen_init(pen, PEN_MIN, PEN_MIN, PEN_MIN);
        ColorRGB_t color;
        pen_to_color_rgb(pen, &color);

        printf("color=%d %d %d\n", (int) color.r, (int) color.g, (int) color.b);
        printf("pen=%u %u %u\n", pen->r, pen->g, pen->b);

        assert(color.r == 0);
        assert(color.g == 0);
        assert(color.b == 0);
    }
    return 0;
}

