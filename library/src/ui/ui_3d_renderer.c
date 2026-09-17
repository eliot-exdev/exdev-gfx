#include "exdevgfx/ui/ui.h"

#include <assert.h>
#include <stdlib.h>

void ui_3d_renderer_init(UI3DRenderer_t *self, const int x, const int y, const int width, const int height) {
    assert(self);

    ui_component_init(&self->base, x, y, width, height);
    self->base.type = UI_COMPONENT_3D_RENDERER;
    self->base.functions.destroy_func = (void (*)(void *)) &ui_3d_renderer_destroy;
    self->base.functions.paint_func = (int (*)(void *, Framebuffer8Bit_t *, int, int, int, int, void *)) &ui_3d_renderer_paint;
    self->base.functions.update_func = (void (*)(void *, long, const Event_t *, int, UIApplication_t *, void *)) &ui_3d_renderer_update;

    self->flags.first_paint = 1;

    self->functions.update_scene = NULL;
    self->functions.render_scene = NULL;
}

UI3DRenderer_t *ui_3d_renderer_create(const int x, const int y, const int width, const int height) {
    UI3DRenderer_t *self = malloc(sizeof(UI3DRenderer_t));
    ui_3d_renderer_init(self, x, y, width, height);

    return self;
}

void ui_3d_renderer_destroy(UI3DRenderer_t *self) {
    assert(self);

    ui_component_destroy(&self->base);
}

int ui_3d_renderer_paint(UI3DRenderer_t *self, Framebuffer8Bit_t *fb, const int x_offset, const int y_offset, const int width, const int height, void *usr_ptr) {
    assert(self);
    assert(fb);
    ui_component_set_dirty(&self->base);

    ui_component_paint(&self->base, fb, x_offset, y_offset, width, height, usr_ptr);

    if (self->flags.first_paint) {
        self->flags.first_paint = 0;
        sw_renderer_8bit_init(&self->properties.renderer, fb);
    }

    if (self->functions.render_scene) {
        self->functions.render_scene(self, usr_ptr);
    }
    return 1;
}

void ui_3d_renderer_update(UI3DRenderer_t *self, const long time_elapsed, const Event_t *events, int num_events, struct UIApplication *app, void *usr_ptr) {
    assert(self);

    if (!self->base.flags.enabled_flag) {
        return;
    }

    ui_component_update(&self->base, time_elapsed, events, num_events, app, usr_ptr);

    // call usr ptr
    if (self->functions.update_scene) {
        self->functions.update_scene(self, time_elapsed, events, num_events, app, usr_ptr);
    }
}
