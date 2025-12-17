/**
* Copyright 2025 Andre Geisler (andre@exdev.de)
 */

#include "exdevgfx/ui/ui.h"
#include "exdevgfx/helper.h"

#define EXDEVGFX2_LOG_LEVEL 2
#include "exdevgfx/logger.h"

#include <assert.h>
#include <stdlib.h>

void ui_horizontal_scroll_bar_init(UIHorizontalScrollBar_t *self, const int x, const int y, const int width, const int height) {
    assert(self);

    ui_component_init(&self->base, x, y, width, height);
    self->base.type = UI_COMPONENT_SCROLL_BAR_HORIZONTAL;

    self->base.functions.destroy_func = (void (*)(void *)) &ui_horizontal_scroll_bar_destroy;
    self->base.functions.paint_func = (int (*)(void *, Framebuffer8Bit_t *, int, int, int, int)) &ui_horizontal_scroll_bar_paint;
    self->base.functions.update_func = (void (*)(void *, exdev_timestamp_t, const Event_t *, int)) &ui_horizontal_scroll_bar_update;

    self->properties.x_pos = 0;
    self->properties.x_width_total = 0;
    self->properties.x_width_visible = 0;

    self->functions.on_x_pos = NULL;
}

UIHorizontalScrollBar_t *ui_horizontal_scroll_bar_create(const int x, const int y, const int width, const int height) {
    UIHorizontalScrollBar_t *self = malloc(sizeof(UIHorizontalScrollBar_t));
    ui_horizontal_scroll_bar_init(self, x, y, width, height);
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
        const float f1 = (float) self->properties.x_width_visible / (float) self->properties.x_width_total;
        const float f2 = (float) self->base.properties.width / (float) self->properties.x_width_visible;
        const int bar_width_half = (int) (f1 * f2 * (float) self->base.properties.width * 0.5);
        int x_bar_middle = self->properties.x_pos - bar_width_half;
        if (x_bar_middle < bar_width_half) {
            x_bar_middle = bar_width_half;
        } else if (x_bar_middle > self->base.properties.width - bar_width_half) {
            x_bar_middle = self->base.properties.width - bar_width_half;
        }
        framebuffer_8bit_fill_rect(fb, x + x_bar_middle - bar_width_half, y, bar_width_half * 2, SCROLL_BAR_SIZE, PEN_INDEX_BLUE);
    }
    return res;
}

void ui_horizontal_scroll_bar_update(UIHorizontalScrollBar_t *self, const exdev_timestamp_t, const Event_t *events, const int num_events) {
    assert(self);
    assert(events);

    for (int i = 0; i < num_events; ++i) {
        if (events[i].type == EVENT_MOUSE) {
            if (events[i].mouse_event.button == MOUSE_BUTTON_0 && events[i].mouse_event.event == MOUSE_EVENT_BUTTON_PRESSED) {
                log_info("mouse event pressed");
                if (ui_component_is_inside(&self->base, events[i].mouse_event.position_x, events[i].mouse_event.position_y)) {
                    const float f1 = (float) self->properties.x_width_visible / (float) self->properties.x_width_total;
                    const float f2 = (float) self->base.properties.width / (float) self->properties.x_width_visible;
                    const int bar_width = (int) (f1 * f2 * (float) self->base.properties.width);

                    int x = events[i].mouse_event.position_x;
                    int y = events[i].mouse_event.position_y;
                    ui_component_get_relative_position(&self->base, &x, &y);
                    self->properties.x_pos = x;
                    self->base.flags.dirty_flag = 1;
                    log_info_fmt("updated x to: %d", self->properties.x_pos);
                }
            }
        }
    }
}
