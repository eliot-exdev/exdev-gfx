#include "exdevgfx/ui/ui.h"

#include <assert.h>
#include <stdlib.h>

void ui_component_list_init(UIComponentList_t *self) {
    assert(self);

    self->components = NULL;
    self->size = 0;
}
void ui_component_list_destroy(UIComponentList_t *self) {
    assert(self);

    for (int i = 0; i < self->size; ++i) {
        ui_component_destroy(self->components + i);
        free(self->components + 1);
    }
}
void ui_component_list_add(UIComponentList_t *self, UIComponent_t *component) {
    assert(self);
    assert(component);


}

void ui_component_init(UIComponent_t *self, const int x, const int y, const int width, const int height, UIComponent_t *parent) {
    assert(self);
    self->type = UI_COMPONENT_CONTAINER;
    self->subtype = 0;
    self->properties.x = x;
    self->properties.y = y;
    self->properties.width = width;
    self->properties.height = height;
    self->properties.parent = parent;

    self->flags.dirty_flag = 1;
    self->flags.enabled_flag = 1;
    self->flags.fill_background = 1;
    self->flags.draw_border = 1;
}

void ui_component_destroy(UIComponent_t *) {}

int ui_component_get_x_abs(const UIComponent_t *self) {
    assert(self);

    if (self->parent) {
        return ui_component_get_x_abs(self->parent) + self->properties.x;
    }
    return self->properties.x;
}
int ui_component_get_y_abs(const UIComponent_t *self) {
    assert(self);

    if (self->parent) {
        return ui_component_get_y_abs(self->parent) + self->properties.y;
    }
    return self->properties.y;
}

void ui_component_paint(UIComponent_t *self, Framebuffer8Bit_t *fb) {
    assert(self);
    assert(fb);

    if (!self->flags.dirty_flag) {
        return;
    }

    const int x = ui_component_get_x_abs(self);
    const int y = ui_component_get_y_abs(self);

    if (self->flags.fill_background) {
        framebuffer_8bit_fill_rect(fb, x, y, self->properties.width, self->properties.height, self->properties.background_color);
    }

    if (self->flags.draw_border) {
        framebuffer_8bit_draw_rect(fb, x, y, self->properties.width, self->properties.height, self->properties.border_color);
    }

    self->flags.dirty_flag = 0;
}
