/**
 * Copyright 2025 Andre Geisler (andre@exdev.de)
 */

#include "exdevgfx/ui/ui.h"

#define EXDEVGFX2_LOG_LEVEL 3

#include "exdevgfx/logger.h"

#include <assert.h>
#include <stdlib.h>

void ui_horizontal_scroll_bar_init(UIHorizontalScrollBar_t *self, UIScrollContainer_t *parent, const int x, const int y, const int width, const int height, const int x_width_total,
                                   const int x_width_visible) {
    assert(self);
    assert(parent);
    log_info_fmt("width visible: %d", x_width_visible);
    log_info_fmt("width total: %d", x_width_total);
    ui_component_init(&self->base, x, y, width, height);
    self->base.type = UI_COMPONENT_SCROLL_BAR_HORIZONTAL;
    self->base.parent = (UIComponent_t *) parent;

    self->base.functions.destroy_func = (void (*)(void *)) &ui_horizontal_scroll_bar_destroy;
    self->base.functions.paint_func = (int (*)(void *, Framebuffer8Bit_t *, int, int, int, int)) &ui_horizontal_scroll_bar_paint;
    self->base.functions.update_func = (void (*)(void *, long, const Event_t *, int)) &ui_horizontal_scroll_bar_update;

    self->properties.x_pos = 0;
    self->flags.dragged = 0;

    self->functions.on_x_offset = parent->functions.on_x_offset;

    self->f1 = (float) x_width_visible / ((float) x_width_total);
    self->bar_width = (int) ((float) width * self->f1);
    self->f1 = (float) (x_width_total - x_width_visible) / (float) (width - self->bar_width);
    if (self->bar_width > self->base.properties.width) {
        self->bar_width = self->base.properties.width;
    }
    self->x_last = 0;
}

UIHorizontalScrollBar_t *
ui_horizontal_scroll_bar_create(UIScrollContainer_t *parent, const int x, const int y, const int width, const int height, const int x_width_total, const int x_width_visible) {
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

void ui_horizontal_scroll_bar_update(UIHorizontalScrollBar_t *self, const long ms, const Event_t *events, const int num_events) {
    assert(self);
    assert(events);

    log_debug_fmt("ui_horizontal_scroll_bar_update: %d", num_events);
    for (int i = 0; i < num_events; ++i) {
        if (events[i].type == EVENT_MOUSE) {
            if (events[i].mouse_event.button == MOUSE_BUTTON_0 && events[i].mouse_event.event == MOUSE_EVENT_BUTTON_PRESSED) {
                if (ui_component_is_inside(&self->base, events[i].mouse_event.position_x, events[i].mouse_event.position_y)) {
                    int x = events[i].mouse_event.position_x;
                    int y = events[i].mouse_event.position_y;
                    ui_component_get_relative_position(&self->base, &x, &y);
                    self->flags.dragged = 1;
                    self->x_last = x;
                }
            } else if (self->flags.dragged && events[i].mouse_event.button == MOUSE_BUTTON_0 && events[i].mouse_event.event == MOUSE_EVENT_BUTTON_RELEASED) {
                self->flags.dragged = 0;
            } else if (self->flags.dragged && events[i].mouse_event.event == MOUSE_EVENT_MOVED) {
                int x = events[i].mouse_event.position_x;
                int y = events[i].mouse_event.position_y;
                ui_component_get_relative_position(&self->base, &x, &y);

                if (x > self->x_last) {
                    // move to right
                    const int x_offset = x - self->x_last;
                    self->properties.x_pos = self->properties.x_pos + x_offset;
                } else {
                    // move to left
                    const int x_offset = self->x_last - x;
                    self->properties.x_pos = self->properties.x_pos - x_offset;
                }
                self->x_last = x;

                if (self->properties.x_pos < 0) {
                    self->properties.x_pos = 0;
                } else if (self->properties.x_pos > self->base.properties.width - self->bar_width) {
                    self->properties.x_pos = self->base.properties.width - self->bar_width;
                }

                self->base.flags.dirty_flag = 1;
                if (self->functions.on_x_offset) {
                    self->functions.on_x_offset((UIScrollContainer_t *) self->base.parent, (int) ((float) self->properties.x_pos * self->f1));
                }
            }
        }
    }
}
