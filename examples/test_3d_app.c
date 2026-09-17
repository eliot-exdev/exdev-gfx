#include <exdevgfx/ui/ui.h>
#include <exdevgfx/exdev_base.h>

#include <stdlib.h>

#include "exdevgfx/helper.h"

#define WIDTH 640
#define HEIGHT 480
#define UPDATE_INTERVAL 40// ms

#define NUM_TRIANGLES 12
#define NUM_COLORS NUM_TRIANGLES
#define NUM_VERTICES 36

// cube
static const Vertex3d_t cube_triangles[NUM_VERTICES] = {
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
        {1, -1, -1}};

static const Color8Bit_t cube_colors[NUM_COLORS] = {
        PEN_INDEX_RED, PEN_INDEX_RED,      //front
        PEN_INDEX_GREEN, PEN_INDEX_GREEN,  // left
        PEN_INDEX_BLUE, PEN_INDEX_BLUE,    // right
        PEN_INDEX_YELLOW, PEN_INDEX_YELLOW,//back,
        PEN_INDEX_GRAY, PEN_INDEX_GRAY,    // top
        PEN_INDEX_CYAN, PEN_INDEX_CYAN     // bottom
};

static Vertex3d_t rot = {0, 0, 0};

static void my_update_scene(UI3DRenderer_t *self, const long time_elapsed, const Event_t *events, const int num_events, struct UIApplication *app, void *usr_ptr) {
    rot[1] -= deg_to_rad(4.0f);
}

static void my_render_scene(UI3DRenderer_t *self, void *usr_ptr) {

    // render left cube wireframe
    Vertex3d_t pos;
    vertex3d_set(pos, -3.0f, 0, 5);
    MATRIX_DEFAULT(m);
    matrix_init(m);
    matrix_rotate(m, rot, m);
    matrix_translate(m, pos, m);
    sw_renderer_8bit_draw_triangles(&self->properties.renderer, cube_triangles, NUM_TRIANGLES, cube_colors, m);

    // render right cube filled
    Vertex3d_t rot_tmp;
    vertex3d_set(rot_tmp, 0, -rot[1], 0);
    vertex3d_set(pos, 3.0f, 0, 5);
    matrix_init(m);
    matrix_rotate(m, rot_tmp, m);
    matrix_translate(m, pos, m);
    sw_renderer_8bit_fill_triangles(&self->properties.renderer, cube_triangles, NUM_TRIANGLES, cube_colors, m);
}


int main() {
    exdev_base_init();

    // application
    UIApplication_t app;
    ui_application_init(&app, WIDTH, HEIGHT, NULL);

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
    exdev_base_deinit();

    return 0;
}
