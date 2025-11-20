/**
* Copyright 2025 Andre Geisler (andre@exdev.de)
 */

#include "exdevgfx/ui/ui.h"

#include <assert.h>
#include <stdlib.h>

UIComponent_t *ui_component_create(const int x, const int y, const int width, const int height) {
    UIComponent_t *self = malloc(sizeof(UIComponent_t));
    ui_component_init(self, x, y, width, height);

    return self;
}

void ui_component_init(UIComponent_t *self, const int x, const int y, const int width, const int height) {
    assert(self);
    self->type = UI_COMPONENT_BASE;
    self->subtype = 0;
    self->properties.x = x;
    self->properties.y = y;
    self->properties.width = width;
    self->properties.height = height;
    self->properties.background_color = PEN_INDEX_DARK_BLUE;
    self->properties.border_color = PEN_INDEX_BLUE;

    self->flags.dirty_flag = 1;
    self->flags.enabled_flag = 1;
    self->flags.fill_background = 1;
    self->flags.draw_border = 1;

    self->functions.destroy_func = (void (*)(void *)) &ui_component_destroy;
    self->functions.paint_func = (int (*)(void *, Framebuffer8Bit_t *, int, int, int, int)) &ui_component_paint;
    self->functions.update_func = (void (*)(void *, long, const Event_t *, int)) &ui_component_update;

    self->parent = NULL;
    ui_component_list_init(&self->children);
}

void ui_component_destroy(UIComponent_t *self) {
    ui_component_list_destroy(&self->children);
}

int ui_component_paint(UIComponent_t *self, Framebuffer8Bit_t *fb, int const x_offset, const int y_offset, const int, const int) {
    assert(self);
    assert(fb);

    int res = self->flags.dirty_flag;
    const int x = self->properties.x + x_offset;
    const int y = self->properties.y + y_offset;

    if (res) {
        if (self->flags.fill_background) {
            framebuffer_8bit_fill_rect(fb, x, y, self->properties.width, self->properties.height, self->properties.background_color);
        }

        if (self->flags.draw_border) {
            framebuffer_8bit_draw_rect(fb, x, y, self->properties.width, self->properties.height, self->properties.border_color);
        }

        self->flags.dirty_flag = 0;
    }

    for (int i = 0; i < self->children.size; ++i) {
        if (self->children.components[i]->functions.paint_func(self->children.components[i], fb, x, y, self->properties.width, self->properties.height)) {
            res = 1;
        }
    }

    return res;
}

void ui_component_update(UIComponent_t *self, const long time_elapsed, const Event_t *events, const int num_events) {
    assert(self);

    if (!self->flags.enabled_flag) {
        return;
    }

    for (int i = 0; i < self->children.size; ++i) {
        self->children.components[i]->functions.update_func(self->children.components[i], time_elapsed, events, num_events);
    }
}

void ui_component_get_absolute_position(const UIComponent_t *self, int *x, int *y) {
    assert(self);
    assert(x);
    assert(y);

    *x += self->properties.x;
    *y += self->properties.y;

    if (self->parent) {
        ui_component_get_absolute_position(self->parent, x, y);
    }
}

void ui_component_get_relative_position(const UIComponent_t *self, int *x, int *y) {
    assert(self);
    assert(x);
    assert(y);

    *x -= self->properties.x;
    *y -= self->properties.y;

    if (self->parent) {
        ui_component_get_relative_position(self->parent, x, y);
    }
}

int ui_component_is_inside(const UIComponent_t *self, const int x, const int y) {
    assert(self);

    int x_ = 0;
    int y_ = 0;

    ui_component_get_absolute_position(self, &x_, &y_);

    return x >= x_ && x < (x_ + self->properties.width) && y >= y_ && y < (y_ + self->properties.height);
}

void ui_component_connect(void *parent_, void *child_) {
    assert(parent_);
    assert(child_);

    UIComponent_t *parent = parent_;
    UIComponent_t *child = child_;

    ui_component_list_add(&parent->children, child);
    child->parent = parent;
}
