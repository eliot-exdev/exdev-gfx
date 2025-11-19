/**
* Copyright 2025 Andre Geisler (andre@exdev.de)
 */

#include "exdevgfx/ui/ui.h"
#include "exdevgfx/helper.h"
#define EXDEVGFX2_LOG_LEVEL 3
#include "exdevgfx/logger.h"

#include <assert.h>
#include <stdlib.h>

void ui_scroll_on_x_offset(UIScrollPane_t *self, const int x_offset) {
    assert(self);

    self->properties.x_offset = x_offset;
    self->base.flags.dirty_flag = 1;
    log_info_fmt("x offset: %d", x_offset);
}

void ui_scroll_init(UIScrollPane_t *self, int x, int y, int width, int height) {
    assert(self);

    ui_component_init(&self->base, x, y, width, height);
    self->base.type = UI_COMPONENT_SCROLL_PANE;

    self->properties.x_offset = 0;
    self->properties.y_offset = 0;

    self->base.functions.destroy_func = (void (*)(void *)) &ui_scroll_destroy;
    self->base.functions.paint_func = (int (*)(void *, Framebuffer8Bit_t *, int, int, int, int)) &ui_scroll_paint;
    self->base.functions.update_func = (void (*)(void *, exdev_timestamp_t, const Event_t *, int)) &ui_scroll_update;

    self->children.h_bar = NULL;
    self->functions.on_x_offset = ui_scroll_on_x_offset;
    self->fb = NULL;
}

UIScrollPane_t *ui_scroll_create(const int x, const int y, const int width, const int height) {
    UIScrollPane_t *self = malloc(sizeof(UIScrollPane_t));
    ui_scroll_init(self, x, y, width, height);

    return self;
}

void ui_scroll_destroy(UIScrollPane_t *self) {
    assert(self);

    ui_component_destroy(&self->base);

    ui_horizontal_scroll_bar_destroy(self->children.h_bar);
    free(self->children.h_bar);
    self->children.h_bar = NULL;

    if (self->fb) {
        framebuffer_8bit_deinit(self->fb);
        free(self->fb);
        self->fb = NULL;
    }
}

int ui_scroll_paint(UIScrollPane_t *self, Framebuffer8Bit_t *fb, const int x_offset, const int y_offset, const int, const int) {
    assert(self);
    assert(fb);

    int res = self->base.flags.dirty_flag;
    const int x = self->base.properties.x + x_offset;
    const int y = self->base.properties.y + y_offset;

    if (!self->fb) {
        // find biggest x and y
        int width = x;
        int height = y;
        for (int i = 0; i < self->base.children.size; ++i) {
            const UIComponent_t *child = self->base.children.components[i];
            const int x_total = child->properties.x + child->properties.width;
            const int y_total = child->properties.y + child->properties.height;
            if (x_total > width) {
                width = x_total;
            }
            if (y_total > height) {
                height = y_total;
            }
        }

        // setup fb
        self->fb = malloc(sizeof(Framebuffer8Bit_t));
        framebuffer_8bit_init(self->fb, width, height);
        framebuffer_8bit_fill(self->fb, self->base.properties.background_color);

        // setup h bar
        self->children.h_bar = ui_horizontal_scroll_bar_create(self, 2, self->base.properties.width - SCROLL_BAR_SIZE - 2, self->base.properties.height - SCROLL_BAR_SIZE - 4, SCROLL_BAR_SIZE, width, self->base.properties.width);
    }

    if (res) {
        if (self->base.flags.fill_background) {
            framebuffer_8bit_fill_rect(fb, x, y, self->base.properties.width, self->base.properties.height, self->base.properties.background_color);
        }

        if (self->base.flags.draw_border) {
            framebuffer_8bit_draw_rect(fb, x, y, self->base.properties.width, self->base.properties.height, self->base.properties.border_color);
        }
    }

    // draw children to back buffer
    for (int i = 0; i < self->base.children.size; ++i) {
        if (self->base.children.components[i]->functions.paint_func(self->base.children.components[i], self->fb, 0, 0, self->fb->width, self->fb->height)) {
            res = 1;
        }
    }

    if (res) {
        // blit fb
        framebuffer_8bit_blit_8bit(fb,
                                   self->fb,
                                   self->properties.x_offset,
                                   self->properties.y_offset,
                                   self->base.properties.width - SCROLL_BAR_SIZE - 4,
                                   self->base.properties.height - SCROLL_BAR_SIZE - 6,
                                   x + 2,
                                   y + 2);

        self->base.flags.dirty_flag = 0;
    }

    // draw h bar
    if (self->children.h_bar->base.functions.paint_func(self->children.h_bar, fb, x, y, self->base.properties.width, self->base.properties.height)) {
        res = 1;
    }

    return res;
}

void ui_scroll_update(UIScrollPane_t *self, const exdev_timestamp_t time_elapsed, const Event_t *events, const int num_events) {
    assert(self);

    ui_component_update(&self->base, time_elapsed, events, num_events);

    if (self->children.h_bar) {
        self->children.h_bar->base.functions.update_func(self->children.h_bar, time_elapsed, events, num_events);
    }
}
