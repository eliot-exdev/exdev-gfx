/**
* Copyright 2025 Andre Geisler (andre@exdev.de)
 */

#include "exdevgfx/ui/ui.h"

#define EXDEVGFX2_LOG_LEVEL 2

#include "exdevgfx/logger.h"

#include <assert.h>
#include <stdlib.h>

void ui_horizontal_scroll_bar_init(UIHorizontalScrollBar_t *self, UIScrollPane_t *parent, const int x, const int y, const int width, const int height, const int x_width_total, const int x_width_visible) {
    assert(self);
    assert(parent);

    ui_component_init(&self->base, x, y, width, height);
    self->base.type = UI_COMPONENT_SCROLL_BAR_HORIZONTAL;
    self->base.parent = (UIComponent_t *) parent;

    self->base.functions.destroy_func = (void (*)(void *)) &ui_horizontal_scroll_bar_destroy;
    self->base.functions.paint_func = (int (*)(void *, Framebuffer8Bit_t *, int, int, int, int)) &ui_horizontal_scroll_bar_paint;
    self->base.functions.update_func = (void (*)(void *, long, const Event_t *, int)) &ui_horizontal_scroll_bar_update;

    self->properties.x_pos = 0;
    self->properties.x_width_total = x_width_total;
    self->properties.x_width_visible = x_width_visible;

    self->flags.dragged = 0;

    self->functions.on_x_offset = parent->functions.on_x_offset;

    self->f1 = (float) self->properties.x_width_visible / (float) self->properties.x_width_total;
    self->f2 = (float) self->base.properties.width / (float) self->properties.x_width_visible;
    self->bar_width = (int) (self->f1 * self->f2 * (float) self->base.properties.width);
}

UIHorizontalScrollBar_t *ui_horizontal_scroll_bar_create(UIScrollPane_t *parent, const int x, const int y, const int width, const int height, const int x_width_total, const int x_width_visible) {
    UIHorizontalScrollBar_t *self = malloc(sizeof(UIHorizontalScrollBar_t));
    ui_horizontal_scroll_bar_init(self, parent, x, y, width, height, x_width_total, x_width_visible);
    return self;
}

void ui_horizontal_scroll_bar_destroy(UIHorizontalScrollBar_t *self) {
    assert(self);

    ui_component_destroy(&self->base);
}

int ui_horizontal_scroll_bar_paint(UIHorizontalScrollBar_t *self, Framebuffer8Bit_t *fb, const int x_offset, const int y_offset, const int width, const int height) {
    assert(self);
    assert(fb);

    int res = self->base.flags.dirty_flag;
    const int x = self->base.properties.x + x_offset;
    const int y = self->base.properties.y + y_offset;

    ui_component_paint(&self->base, fb, x_offset, y_offset, width, height);

    // draw bar
    if (res) {
        framebuffer_8bit_fill_rect(fb, x + self->properties.x_pos, y, self->bar_width, SCROLL_BAR_SIZE, PEN_INDEX_BLUE);
    }
    return res;
}

void ui_horizontal_scroll_bar_update(UIHorizontalScrollBar_t *self, const long, const Event_t *events, const int num_events) {
    assert(self);
    assert(events);

    for (int i = 0; i < num_events; ++i) {
        if (events[i].type == EVENT_MOUSE) {
            if (events[i].mouse_event.button == MOUSE_BUTTON_0 && events[i].mouse_event.event == MOUSE_EVENT_BUTTON_PRESSED) {
                if (ui_component_is_inside(&self->base, events[i].mouse_event.position_x, events[i].mouse_event.position_y)) {
                    int x = events[i].mouse_event.position_x;
                    int y = events[i].mouse_event.position_y;
                    ui_component_get_relative_position(&self->base, &x, &y);
                    if (x != self->properties.x_pos) {
                        self->properties.x_pos = x;
                        if (self->properties.x_pos > self->base.properties.width - self->bar_width) {
                            self->properties.x_pos = self->base.properties.width - self->bar_width;
                        }
                        self->base.flags.dirty_flag = 1;
                        self->flags.dragged = 1;
                        if (self->functions.on_x_offset) {
                            self->functions.on_x_offset((UIScrollPane_t *) self->base.parent, (int) ((float) self->properties.x_pos / self->f1 / self->f2));
                        }
                    }
                }
            } else if (self->flags.dragged && events[i].mouse_event.button == MOUSE_BUTTON_0 && events[i].mouse_event.event == MOUSE_EVENT_BUTTON_RELEASED) {
                self->flags.dragged = 0;
            } else if (self->flags.dragged && events[i].mouse_event.event == MOUSE_EVENT_MOVED) {
                int x = events[i].mouse_event.position_x;
                int y = events[i].mouse_event.position_y;
                ui_component_get_relative_position(&self->base, &x, &y);

                if (x < 0) {
                    x = 0;
                } else if (x > self->base.properties.width - self->bar_width) {
                    x = self->base.properties.width - self->bar_width;
                }

                if (x != self->properties.x_pos) {
                    self->properties.x_pos = x;
                    self->base.flags.dirty_flag = 1;
                    if (self->functions.on_x_offset) {
                        self->functions.on_x_offset((UIScrollPane_t *) self->base.parent, (int) ((float) self->properties.x_pos / self->f1 / self->f2));
                    }
                }
            }
        }
    }
}
