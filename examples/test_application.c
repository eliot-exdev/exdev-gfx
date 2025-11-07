#include <exdevgfx/ui/ui.h>
#include <exdevgfx/exdev_base.h>

#define WIDTH 640
#define HEIGHT 480
#define UPDATE_INTERVAL 100 // 10 times a second

static int i = 0;

void my_paint(UIComponent_t *self, Framebuffer8Bit_t *fb) {
    ui_component_paint(self, fb); // call paint of base class
    framebuffer_8bit_fill_rect(fb, self->properties.x + 2, self->properties.y + i, self->properties.width - 3, 10, 2);
    self->flags.dirty_flag = 1; // always paint again
}

void my_update(UIComponent_t *self, const exdev_timestamp_t ms) {
    ui_component_update(self, ms); // call update of base class

    i += 5;
    if (i >= HEIGHT - 12) {
        i = 0;
    }
}

int main() {
    exdev_base_init();

    Application_t app;
    application_init(&app, WIDTH, HEIGHT);
    palette_8bit_add_pen(app.palette, &PEN_BLUE);

    UIComponent_t *left = malloc(sizeof(UIComponent_t));
    ui_component_init(left, 2, 2, 538, 476, app.root);
    ui_component_list_add(&app.root->childs, left);

    UIComponent_t *right = malloc(sizeof(UIComponent_t));
    ui_component_init(right, 542, 2, 96, 476, app.root);
    right->functions.paint_func = (void (*)(void *, Framebuffer8Bit_t *)) &my_paint;
    right->functions.update_func = (void (*)(void *, exdev_timestamp_t)) &my_update;
    ui_component_list_add(&app.root->childs, right);

    application_run(&app, UPDATE_INTERVAL);
    application_destroy(&app);

    exdev_base_deinit();

    return 0;
}