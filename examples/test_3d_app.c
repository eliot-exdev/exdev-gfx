/**
* Copyright 2026 Andre Geisler (andre@exdev.de)
 */

#include <exdevgfx/ui/ui.h>
#include <exdevgfx/exdev_base.h>
#include <exdevgfx/helper.h>
#include <exdevgfx/logger.h>

#ifdef LOW_RESOLUTION
#if USE_C2P
#define WIDTH 320
#define HEIGHT 256
#else
#define WIDTH 320
#define HEIGHT 240
#endif
#else
#define WIDTH 640
#define HEIGHT 480
#endif

#define UPDATE_INTERVAL 40 // ms

#define NUM_TRIANGLES 12
#define NUM_COLORS NUM_TRIANGLES
#define NUM_VERTICES 36

// cube
static const Vertex3d_t CUBE_TRIANGLES[NUM_VERTICES] = {
        {-1, -1, -1},// front
        {1, -1, -1},
        {1, 1, -1},
        {-1, -1, -1},
        {1, 1, -1},
        {-1, 1, -1},
        {-1, -1, 1},// left
        {-1, 1, -1},
        {-1, 1, 1},
        {-1, -1, 1},
        {-1, -1, -1},
        {-1, 1, -1},
        {1, -1, 1},// right
        {1, 1, -1},
        {1, -1, -1},
        {1, -1, 1},
        {1, 1, 1},
        {1, 1, -1},
        {-1, -1, 1},// back
        {-1, 1, 1},
        {1, 1, 1},
        {-1, -1, 1},
        {1, 1, 1},
        {1, -1, 1},
        {-1, 1, -1},// top
        {1, 1, 1},
        {-1, 1, 1},
        {-1, 1, -1},
        {1, 1, -1},
        {1, 1, 1},
        {-1, -1, -1},// bottom
        {-1, -1, 1},
        {1, -1, 1},
        {-1, -1, -1},
        {1, -1, 1},
        {1, -1, -1}
};

static const TextureCoordinates_t CUBE_UV[NUM_VERTICES] = {
        {0, 1}, {1, 1}, {1, 0},// front
        {0, 1}, {1, 0}, {0, 0},
        {0, 1}, {1, 0}, {0, 0},// left
        {0, 1}, {1, 1}, {1, 0},
        {1, 1}, {0, 0}, {0, 1},// right
        {1, 1}, {1, 0}, {0, 0},
        {1, 1}, {1, 0}, {0, 0},// back
        {1, 1}, {0, 0}, {0, 1},
        {0, 1}, {1, 0}, {0, 0},// top
        {0, 1}, {1, 1}, {1, 0},
        {0, 0}, {0, 1}, {1, 1},// bottom
        {0, 0}, {1, 1}, {1, 0}
};

static const Color8Bit_t CUBE_COLORS[NUM_COLORS] = {
        PEN_INDEX_RED, PEN_INDEX_RED,//front
        PEN_INDEX_GREEN, PEN_INDEX_GREEN,// left
        PEN_INDEX_BLUE, PEN_INDEX_BLUE,// right
        PEN_INDEX_YELLOW, PEN_INDEX_YELLOW,//back,
        PEN_INDEX_GRAY, PEN_INDEX_GRAY,// top
        PEN_INDEX_CYAN, PEN_INDEX_CYAN// bottom
};

static Framebuffer8Bit_t texture;

static float rot_y = 0.0f;

static void my_update_scene(UI3DRenderer_t *self, const long time_elapsed, const Event_t *events, const int num_events, struct UIApplication *app,
                            void *usr_ptr) { rot_y -= deg_to_rad(2.0f); }

static void my_render_scene(UI3DRenderer_t *self, void *usr_ptr) {
    // render left cube wireframe
    MATRIX_DEFAULT(m);
    matrix_rotateY(m, rot_y, m);
    matrix_translateXYZ(m, -3.0f, 0.0f, 5.0f, m);
    sw_renderer_8bit_draw_triangles(&self->properties.renderer, CUBE_TRIANGLES, NUM_TRIANGLES, CUBE_COLORS, m);

    // render right cube flat
    matrix_init(m);
    matrix_rotateY(m, rot_y, m);
    matrix_translateXYZ(m, 3.0f, 0.0f, 5.0f, m);
    sw_renderer_8bit_fill_triangles(&self->properties.renderer, CUBE_TRIANGLES, NUM_TRIANGLES, CUBE_COLORS, m);

    // render middle cube textured
    matrix_init(m);
    matrix_rotateY(m, rot_y, m);
    matrix_translateXYZ(m, 0.0f, -2.0f, 4.0f, m);
    sw_renderer_8bit_fill_triangles_texture(&self->properties.renderer, CUBE_TRIANGLES, NUM_TRIANGLES, &texture, CUBE_UV, m);
}


int main() {
    exdev_base_init();

    // load texture
    int res = framebuffer_8bit_read_from_dat(&texture, "assets/amiga_logo_8bit.dat");
    if (res) {
        log_warning_fmt("could not read texture from: %s", "assets/amiga_logo_8bit.dat");
        return res;
    }

    // application
    UIApplication_t app;
    ui_application_init(&app, WIDTH, HEIGHT, NULL);

    // set palette
    res = palette_8bit_read_from_dat(&app.palette, "assets/amiga_logo_8bit.pal");
    if (res) {
        log_warning_fmt("could not read palette from: %s", "assets/amiga_logo_8bit.pal");
        return res;
    }

    // 3d
    UI3DRenderer_t *ui_3d = ui_3d_renderer_create(2, 2, WIDTH - 4, HEIGHT - 4);
    ui_3d->functions.update_scene = &my_update_scene;
    ui_3d->functions.render_scene = &my_render_scene;
    ui_component_connect(&app.root, ui_3d);

    // run
    ui_application_prepare(&app);
    ui_application_run(&app, "test_3d_app", UPDATE_INTERVAL);

    // cleanup
    ui_application_destroy(&app);
    framebuffer_8bit_deinit(&texture);
    exdev_base_deinit();

    return 0;
}