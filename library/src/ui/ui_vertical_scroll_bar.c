/**
* Copyright 2025 Andre Geisler (andre@exdev.de)
 */

#include "exdevgfx/ui/ui.h"

#define EXDEVGFX2_LOG_LEVEL 3

#include "exdevgfx/logger.h"

#include <assert.h>
#include <stdlib.h>

void ui_vertical_scroll_bar_init(UIVerticalScrollBar_t *self, UIScrollPane_t *parent, const int x, const int y, const int width, const int height, const int y_height_total,
                                 const int y_height_visible) {
    assert(self);
    assert(parent);

    ui_component_init(&self->base, x, y, width, height);
    self->base.type = UI_COMPONENT_SCROLL_BAR_VERTICAL;
    self->base.parent = (UIComponent_t *) parent;

    self->base.functions.destroy_func = (void (*)(void *)) &ui_vertical_scroll_bar_destroy;
    self->base.functions.paint_func = (int (*)(void *, Framebuffer8Bit_t *, int, int, int, int)) &ui_vertical_scroll_bar_paint;
    self->base.functions.update_func = (void (*)(void *, long, const Event_t *, int)) &ui_vertical_scroll_bar_update;

    self->properties.y_pos = 0;
    self->properties.y_height_total = y_height_total;
    self->properties.y_height_visible = y_height_visible;

    self->flags.dragged = 0;

    self->functions.on_y_offset = parent->functions.on_y_offset;

    self->f1 = (float) self->properties.y_height_visible / (float) self->properties.y_height_total;
    self->f1 = self->f1 * (float) self->base.properties.height / (float) self->properties.y_height_visible;
    self->bar_height = (int) (self->f1 * (float) self->base.properties.height);
    self->y_last = 0;
}

UIVerticalScrollBar_t *
ui_vertical_scroll_bar_create(UIScrollPane_t *parent, const int x, const int y, const int width, const int height, const int y_height_total, const int y_height_visible) {
    UIVerticalScrollBar_t *self = malloc(sizeof(UIVerticalScrollBar_t));
    ui_vertical_scroll_bar_init(self, parent, x, y, width, height, y_height_total, y_height_visible);
    return self;
}

void ui_vertical_scroll_bar_destroy(UIVerticalScrollBar_t *self) {
    assert(self);

    ui_component_destroy(&self->base);
}

int ui_vertical_scroll_bar_paint(UIVerticalScrollBar_t *self, Framebuffer8Bit_t *fb, const int x_offset, const int y_offset, const int width, const int height) {
    assert(self);
    assert(fb);

    int res = self->base.flags.dirty_flag;
    const int x = self->base.properties.x + x_offset;
    const int y = self->base.properties.y + y_offset;

    ui_component_paint(&self->base, fb, x_offset, y_offset, width, height);

    // draw bar
    if (res) {
        framebuffer_8bit_fill_rect(fb, x, y + self->properties.y_pos, SCROLL_BAR_SIZE, self->bar_height, PEN_INDEX_BLUE);
    }
    return res;
}

void ui_vertical_scroll_bar_update(UIVerticalScrollBar_t *self, const long ms, const Event_t *events, const int num_events) {
    assert(self);
    assert(events);

    log_info_fmt("ui_vertical_scroll_bar_update: %d", num_events);
    for (int i = 0; i < num_events; ++i) {
        if (events[i].type == EVENT_MOUSE) {
            if (events[i].mouse_event.button == MOUSE_BUTTON_0 && events[i].mouse_event.event == MOUSE_EVENT_BUTTON_PRESSED) {
                if (ui_component_is_inside(&self->base, events[i].mouse_event.position_x, events[i].mouse_event.position_y)) {
                    int x = events[i].mouse_event.position_x;
                    int y = events[i].mouse_event.position_y;
                    ui_component_get_relative_position(&self->base, &x, &y);
                    self->flags.dragged = 1;
                    self->y_last = y;
                }
            } else if (self->flags.dragged && events[i].mouse_event.button == MOUSE_BUTTON_0 && events[i].mouse_event.event == MOUSE_EVENT_BUTTON_RELEASED) {
                self->flags.dragged = 0;
            } else if (self->flags.dragged && events[i].mouse_event.event == MOUSE_EVENT_MOVED) {
                int x = events[i].mouse_event.position_x;
                int y = events[i].mouse_event.position_y;
                ui_component_get_relative_position(&self->base, &x, &y);

                if (y > self->y_last) {
                    // move down
                    const int y_offset = y - self->y_last;
                    self->properties.y_pos = self->properties.y_pos + y_offset;
                } else {
                    // move up
                    const int y_offset = self->y_last - y;
                    self->properties.y_pos = self->properties.y_pos - y_offset;
                }
                self->y_last = y;

                if (self->properties.y_pos < 0) {
                    self->properties.y_pos = 0;
                } else if (self->properties.y_pos > self->base.properties.height - self->bar_height) {
                    self->properties.y_pos = self->base.properties.height - self->bar_height;
                }

                self->base.flags.dirty_flag = 1;
                if (self->functions.on_y_offset) {
                    self->functions.on_y_offset((UIScrollPane_t *) self->base.parent, (int) ((float) self->properties.y_pos / self->f1));
                }
            }
        }
    }
}
