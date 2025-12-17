/**
* Copyright 2025 Andre Geisler (andre@exdev.de)
 */

#include "exdevgfx/ui/ui.h"

#define EXDEVGFX2_LOG_LEVEL 3

#include "exdevgfx/logger.h"

#include <assert.h>
#include <stdlib.h>

void ui_scroll_on_x_offset(UIScrollPane_t *self, const int x_offset) {
    assert(self);

    self->properties.x_offset = x_offset;
    self->base.flags.dirty_flag = 1;
    if (self->v_bar) {
        self->v_bar->base.flags.dirty_flag = 1;
    }
}

void ui_scroll_on_y_offset(UIScrollPane_t *self, const int y_offset) {
    assert(self);

    self->properties.y_offset = y_offset;
    self->base.flags.dirty_flag = 1;
    if (self->h_bar) {
        self->h_bar->base.flags.dirty_flag = 1;
    }
}

void ui_scroll_init(UIScrollPane_t *self, int x, int y, int width, int height, const UIScrollingSupport_t scrolling_support) {
    assert(self);

    ui_component_init(&self->base, x, y, width, height);
    self->base.type = UI_COMPONENT_SCROLL_PANE;

    self->properties.x_offset = 0;
    self->properties.y_offset = 0;
    self->properties.scrolling_support = scrolling_support;

    self->base.functions.destroy_func = (void (*)(void *)) &ui_scroll_destroy;
    self->base.functions.prepare_func = (void (*)(void *)) &ui_scroll_prepare;
    self->base.functions.paint_func = (int (*)(void *, Framebuffer8Bit_t *, int, int, int, int)) &ui_scroll_paint;
    self->base.functions.update_func = (void (*)(void *, long, const Event_t *, int)) &ui_scroll_update;

    if (scrolling_support == UI_SCROLLING_SUPPORT_HORIZONTAL || scrolling_support == UI_SCROLLING_SUPPORT_HORIZONTAL_AND_VERTICAL) {
        self->functions.on_x_offset = ui_scroll_on_x_offset;
    } else {
        self->functions.on_x_offset = NULL;
    }

    if (scrolling_support == UI_SCROLLING_SUPPORT_VERTICAL || scrolling_support == UI_SCROLLING_SUPPORT_HORIZONTAL_AND_VERTICAL) {
        self->functions.on_y_offset = ui_scroll_on_y_offset;
    } else {
        self->functions.on_y_offset = NULL;
    }

    self->fb = NULL;
    self->h_bar = NULL;
    self->v_bar = NULL;
}

UIScrollPane_t *ui_scroll_create(const int x, const int y, const int width, const int height, const UIScrollingSupport_t scrolling_support) {
    UIScrollPane_t *self = malloc(sizeof(UIScrollPane_t));
    ui_scroll_init(self, x, y, width, height, scrolling_support);

    return self;
}

void ui_scroll_destroy(UIScrollPane_t *self) {
    assert(self);

    ui_component_destroy(&self->base);

    if (self->h_bar) {
        self->h_bar->base.functions.destroy_func(self->h_bar);
        free(self->h_bar);
        self->h_bar = NULL;
    }

    if (self->v_bar) {
        self->v_bar->base.functions.destroy_func(self->v_bar);
        free(self->v_bar);
        self->v_bar = NULL;
    }

    if (self->fb) {
        framebuffer_8bit_deinit(self->fb);
        free(self->fb);
        self->fb = NULL;
    }
}

void ui_scroll_prepare(UIScrollPane_t *self) {
    assert(self);

    ui_component_prepare(&self->base);

    // find biggest x and y
    int width = self->base.properties.width;
    int height = self->base.properties.height;
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
    if (self->properties.scrolling_support == UI_SCROLLING_SUPPORT_HORIZONTAL || self->properties.scrolling_support == UI_SCROLLING_SUPPORT_HORIZONTAL_AND_VERTICAL) {
        self->h_bar = ui_horizontal_scroll_bar_create(self,
                                                      2,
                                                      self->base.properties.width - SCROLL_BAR_SIZE - 2,
                                                      self->base.properties.width - SCROLL_BAR_SIZE - 4,
                                                      SCROLL_BAR_SIZE,
                                                      width,
                                                      self->base.properties.width - SCROLL_BAR_SIZE - 6);
    }
    if (self->properties.scrolling_support == UI_SCROLLING_SUPPORT_VERTICAL || self->properties.scrolling_support == UI_SCROLLING_SUPPORT_HORIZONTAL_AND_VERTICAL) {
        self->v_bar = ui_vertical_scroll_bar_create(self,
                                                    self->base.properties.width - SCROLL_BAR_SIZE - 2,
                                                    2,
                                                    SCROLL_BAR_SIZE,
                                                    self->base.properties.height - SCROLL_BAR_SIZE - 4,
                                                    height,
                                                    self->base.properties.height - SCROLL_BAR_SIZE - 6);
    }
}

int ui_scroll_paint(UIScrollPane_t *self, Framebuffer8Bit_t *fb, const int x_offset, const int y_offset, const int, const int) {
    assert(self);
    assert(fb);

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
                                   self->base.properties.width - SCROLL_BAR_SIZE - 8,
                                   self->base.properties.height - SCROLL_BAR_SIZE - 8,
                                   x + 2,
                                   y + 2);

        self->base.flags.dirty_flag = 0;
    }

    // draw h bar
    if (self->h_bar) {
        res |= self->h_bar->base.functions.paint_func(self->h_bar, fb, x, y, self->base.properties.width, self->base.properties.height);
    }

    // draw v bar
    if (self->v_bar) {
        res |= self->v_bar->base.functions.paint_func(self->v_bar, fb, x, y, self->base.properties.width, self->base.properties.height);
    }
    return res;
}

void ui_scroll_update(UIScrollPane_t *self, const long time_elapsed, const Event_t *events, const int num_events) {
    assert(self);
    log_info_fmt("ui_scroll_update: %d", num_events);
    ui_component_update(&self->base, time_elapsed, events, num_events);
    log_info_fmt("ui_scroll_update after base: %d", num_events);
    if (self->h_bar) {
        self->h_bar->base.functions.update_func(self->h_bar, time_elapsed, events, num_events);
    }
    log_info_fmt("ui_scroll_update after horizontal bar: %d", num_events);
    if (self->v_bar) {
        self->v_bar->base.functions.update_func(self->v_bar, time_elapsed, events, num_events);
    }
    log_info_fmt("ui_scroll_update after vertical bar: %d", num_events);
}
