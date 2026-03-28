/**
* Copyright 2026 Andre Geisler (andre@exdev.de)
 */

#include "exdevgfx/ui/ui.h"

#include <assert.h>
#include <stdlib.h>

void ui_layered_container_init(UILayeredContainer_t *self, const int x, const int y, const int width, const int height) {
    assert(self);

    ui_component_init(&self->base, x, y, width, height);

    self->base.type = UI_COMPONENT_LAYERED_CONTAINER;
    self->base.functions.paint_func = (int (*)(void *, Framebuffer8Bit_t *, int, int, int, int, void *)) &ui_layered_container_paint;
    self->base.functions.update_func = (void (*)(void *, long, const Event_t *, int, UIApplication_t *, void *)) &ui_layered_container_update;
    self->base.functions.dirty_function = (void (*)(void *)) ui_layered_container_set_dirty;

    self->properties.active_child = 0;
    self->properties.bar_height = SCROLL_BAR_SIZE;
}

UILayeredContainer_t *ui_layered_container_create(const int x, const int y, const int width, const int height) {
    UILayeredContainer_t *self = malloc(sizeof(UILayeredContainer_t));
    ui_layered_container_init(self, x, y, width, height);
    return self;
}

int ui_layered_container_paint(UILayeredContainer_t *self, Framebuffer8Bit_t *fb, const int x_offset, const int y_offset, const int width, const int height, void *usr_ptr) {

    int res = self->base.flags.dirty_flag;
    const int x = self->base.properties.x + x_offset;
    const int y = self->base.properties.y + y_offset;

    if (res) {
        if (self->base.flags.fill_background) {
            framebuffer_8bit_fill_rect(fb, x, y, self->base.properties.width, self->base.properties.height, self->base.properties.background_color);
        }

        if (self->base.flags.draw_border) {
            framebuffer_8bit_draw_rect(fb, x, y, self->base.properties.width, self->base.properties.height, self->base.properties.border_color);
        }

        self->base.flags.dirty_flag = 0;

        const int bar_num = self->base.children.size;
        const int bar_width = self->base.properties.width / bar_num;
        const int bar_height = self->properties.bar_height;
        const int bar_y_pos = y + self->base.properties.height - self->properties.bar_height;
        for (int i = 0; i < bar_num; ++i) {
            const int bar_x_pos = x + i * bar_width;
            if (self->properties.active_child == i) {
                framebuffer_8bit_fill_rect(fb, bar_x_pos, bar_y_pos, bar_width, bar_height, self->base.properties.border_color);
            } else {
                framebuffer_8bit_draw_rect(fb, bar_x_pos, bar_y_pos, bar_width, bar_height, self->base.properties.border_color);
            }
        }
    }

    if (self->base.children.size != 0) {
        if (self->base.children.components[self->properties.active_child]->functions.paint_func(self->base.children.components[self->properties.active_child], fb, x, y, self->base.properties.width, self->base.properties.height - self->properties.bar_height, usr_ptr)) {
            res = 1;
        }
    }

    return res;
}

void ui_layered_container_update(UILayeredContainer_t *self, const long time_elapsed, const Event_t *events, const int num_events, struct UIApplication *app, void *usr_ptr) {
    assert(self);
    assert(events);

    if (self->base.children.size != 0) {
        self->base.children.components[self->properties.active_child]->functions.update_func(self->base.children.components[self->properties.active_child], time_elapsed, events, num_events, app, usr_ptr);
    }

    for (int i = 0; i < num_events; ++i) {
        if (events[i].type == EVENT_MOUSE && events[i].mouse_event.event == MOUSE_EVENT_BUTTON_PRESSED) {
            int x = 0, y = 0;
            ui_component_get_absolute_position(&self->base, &x, &y);
            const int bar_num = self->base.children.size;
            const int bar_width = self->base.properties.width / bar_num;
            const int bar_height = self->properties.bar_height;
            const int bar_y_pos = y + self->base.properties.height - self->properties.bar_height;

            for (int ii = 0; ii < bar_num; ++ii) {
                const int bar_x_pos = x + ii * bar_width;
                if (is_inside(bar_x_pos, bar_y_pos, bar_width, bar_height, events[i].mouse_event.position_x, events[i].mouse_event.position_y)) {
                    ui_layered_container_set_active_child(self, ii);
                    break;
                }
            }
        }
    }
}

void ui_layered_container_set_active_child(UILayeredContainer_t *self, const uint8_t child) {
    assert(self);
    assert(child < self->base.children.size);

    if (self->base.children.size == 0) {
        return;
    }
    if (self->properties.active_child == child) {
        return;
    }

    self->base.children.components[self->properties.active_child]->flags.enabled_flag = 0;

    self->properties.active_child = child;
    self->base.children.components[child]->flags.enabled_flag = 1;

    self->base.functions.dirty_function(self);
}

void ui_layered_container_set_dirty(UILayeredContainer_t *self) {
    assert(self);

    self->base.children.components[self->properties.active_child]->functions.dirty_function(self->base.children.components[self->properties.active_child]);
    self->base.flags.dirty_flag = 1;
}