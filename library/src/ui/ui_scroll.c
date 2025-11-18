/**
* Copyright 2025 Andre Geisler (andre@exdev.de)
 */

#include "exdevgfx/ui/ui.h"
#include "exdevgfx/helper.h"

#include <assert.h>
#include <stdlib.h>

void ui_scroll_init(UIScroll_t *self, int x, int y, int width, int height) {
    assert(self);

    ui_component_init(&self->base, x, y, width, height);
    self->base.type = UI_COMPONENT_SCROLL;

    self->properties.x_pos = 0;
    self->properties.y_pos = 0;

    self->base.functions.destroy_func = (void (*)(void *)) &ui_scroll_destroy;
    self->base.functions.paint_func = (int (*)(void *, Framebuffer8Bit_t *, int, int, int, int)) &ui_scroll_paint;
    self->base.functions.update_func = (void (*)(void *, exdev_timestamp_t, const Event_t *, int)) &ui_scroll_update;

    self->fb = NULL;
}

UIScroll_t *ui_scroll_create(const int x, const int y, const int width, const int height) {
    UIScroll_t *self = malloc(sizeof(UIScroll_t));
    ui_scroll_init(self, x, y, width, height);

    return self;
}

void ui_scroll_destroy(UIScroll_t *self) {
    assert(self);

    ui_component_destroy(&self->base);
    if (self->fb) {
        framebuffer_8bit_deinit(self->fb);
        free(self->fb);
        self->fb = NULL;
    }
}

#define SCROLL_BAR_SIZE 10

int ui_scroll_paint(UIScroll_t *self, Framebuffer8Bit_t *fb, const int x_offset, const int y_offset, const int, const int) {
    assert(self);
    assert(fb);

    int res = self->base.flags.dirty_flag;
    const int x = self->base.properties.x + x_offset;
    const int y = self->base.properties.y + y_offset;

    if (!self->fb) {
        // find biggest x and y
        int width = x;
        int height = y;
        for (int i = 0; i < self->base.childs.size; ++i) {
            const UIComponent_t *child = self->base.childs.components[i];
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
    }

    if (res) {
        if (self->base.flags.fill_background) {
            framebuffer_8bit_fill_rect(fb, x, y, self->base.properties.width, self->base.properties.height, self->base.properties.background_color);
            framebuffer_8bit_fill(self->fb, self->base.properties.background_color);
        }

        if (self->base.flags.draw_border) {
            framebuffer_8bit_draw_rect(fb, x, y, self->base.properties.width, self->base.properties.height, self->base.properties.border_color);
        }
    }

    // draw children to back buffer
    for (int i = 0; i < self->base.childs.size; ++i) {
        if (self->base.childs.components[i]->functions.paint_func(self->base.childs.components[i], self->fb, 0, 0, self->fb->width, self->fb->height)) {
            res = 1;
        }
    }

    if (res) {
        // blit fb
        framebuffer_8bit_blit_8bit(fb,
                                   self->fb,
                                   self->properties.x_pos,
                                   self->properties.y_pos,
                                   self->base.properties.width - SCROLL_BAR_SIZE - 4,
                                   self->base.properties.height - SCROLL_BAR_SIZE - 4,
                                   x + 2,
                                   y + 2);

        self->base.flags.dirty_flag = 0;
    }

    return res;
}

void ui_scroll_update(UIScroll_t *self, const exdev_timestamp_t time_elapsed, const Event_t *events, const int num_events) {
    assert(self);

    ui_component_update(&self->base, time_elapsed, events, num_events);
}
