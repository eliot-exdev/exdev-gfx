/**
 * Copyright 2025 Andre Geisler (andre@exdev.de)
 */

#include "exdevgfx/ui/ui.h"

#include <assert.h>
#include <stdlib.h>

UIIcon_t *ui_icon_create(const int x, const int y, Framebuffer8Bit_t *icon_enabled, Framebuffer8Bit_t *icon_disabled) {
    assert(icon_enabled);

    UIIcon_t *self = malloc(sizeof(UIIcon_t));
    ui_icon_init(self, x, y, icon_enabled, icon_disabled);

    return self;
}

UIIcon_t *ui_icon_create_with_path(const int x, const int y, const char *path_icon_enabled, const char *path_icon_disabled) {
    assert(path_icon_enabled);

    Framebuffer8Bit_t *icon_enabled = malloc(sizeof(Framebuffer8Bit_t));
    framebuffer_8bit_read_from_dat(icon_enabled, path_icon_enabled);

    Framebuffer8Bit_t *icon_disabled = NULL;
    if (path_icon_disabled) {
        icon_disabled = malloc(sizeof(Framebuffer8Bit_t));
        framebuffer_8bit_read_from_dat(icon_enabled, path_icon_disabled);
    }
    return ui_icon_create(x, y, icon_enabled, icon_disabled);
}

void ui_icon_init(UIIcon_t *self, const int x, const int y, Framebuffer8Bit_t *icon_enabled, Framebuffer8Bit_t *icon_disabled) {
    assert(self);
    assert(icon_enabled);

    ui_component_init(&self->base, x, y, icon_enabled->width + 2, icon_enabled->height + 2);
    self->base.type = UI_COMPONENT_ICON;
    self->base.functions.destroy_func = (void (*)(void *)) &ui_icon_destroy;
    self->base.functions.paint_func = (int (*)(void *, Framebuffer8Bit_t *, int, int, int, int, void *)) &ui_icon_paint;
    self->base.functions.update_func = (void (*)(void *, long, const Event_t *, int, UIApplication_t *, void *)) &ui_icon_update;

    self->properties.clickable = 1;
    self->properties.alpha = 0;
    self->flags.focused = 0;
    self->flags.clicked = 0;

    self->functions.on_clicked = NULL;
    self->functions.on_focus = NULL;

    self->icon_enabled = icon_enabled;
    self->icon_disabled = icon_disabled;
}

void ui_icon_destroy(UIIcon_t *self) {
    assert(self);

    ui_component_destroy(&self->base);

    framebuffer_8bit_deinit(self->icon_enabled);
    free(self->icon_enabled);
    self->icon_enabled = NULL;
    if (self->icon_disabled) {
        framebuffer_8bit_deinit(self->icon_disabled);
        free(self->icon_disabled);
        self->icon_disabled = NULL;
    }
}

int ui_icon_paint(UIIcon_t *self, Framebuffer8Bit_t *fb, const int x_offset, const int y_offset, const int width, const int height, void *usr_ptr) {
    assert(self);
    assert(fb);

    const int res = self->base.flags.dirty_flag;
    const int x = self->base.properties.x + x_offset;
    const int y = self->base.properties.y + y_offset;

    // draw base
    ui_component_paint(&self->base, fb, x_offset, y_offset, width, height, usr_ptr);
    if (res) {
        if (!self->base.flags.enabled_flag && self->icon_disabled) {
            framebuffer_8bit_draw_framebuffer_with_alpha(fb, x + 1, y + 1, self->icon_disabled, self->properties.alpha);
        } else {
            framebuffer_8bit_draw_framebuffer_with_alpha(fb, x + 1, y + 1, self->icon_enabled, self->properties.alpha);
        }
    }
    return res;
}

void ui_icon_update(UIIcon_t *self, const long time_elapsed, const Event_t *events, const int num_events, UIApplication_t *app, void *usr_ptr) {
    assert(self);

    if (!self->base.flags.enabled_flag) {
        return;
    }

    ui_component_update(&self->base, time_elapsed, events, num_events, app, usr_ptr);

    if (!self->properties.clickable) {
        return;
    }

    for (int i = 0; i < num_events; ++i) {
        if (events[i].type == EVENT_MOUSE && events[i].mouse_event.event == MOUSE_EVENT_MOVED) {
            if (ui_component_is_inside(&self->base, events[i].mouse_event.position_x, events[i].mouse_event.position_y)) {
                if (!self->flags.focused) {
                    self->flags.focused = 1;
                    self->base.properties.border_color = PEN_INDEX_DARK_YELLOW;
                    self->base.flags.dirty_flag = 1;
                    if (self->functions.on_focus) {
                        self->functions.on_focus(self, app, usr_ptr);
                    }
                }
            } else {
                if (self->flags.focused) {
                    self->flags.focused = 0;
                    self->base.properties.border_color = PEN_INDEX_BLUE;
                    self->base.flags.dirty_flag = 1;
                    if (self->functions.on_focus) {
                        self->functions.on_focus(self, app, usr_ptr);
                    }
                }
            }
        } else if (events[i].type == EVENT_MOUSE && events[i].mouse_event.event == MOUSE_EVENT_BUTTON_PRESSED) {
            if (ui_component_is_inside(&self->base, events[i].mouse_event.position_x, events[i].mouse_event.position_y)) {
                self->flags.clicked = 1;
                self->base.properties.border_color = PEN_INDEX_YELLOW;
                self->base.flags.dirty_flag = 1;
                if (self->functions.on_clicked) {
                    self->functions.on_clicked(self, app, usr_ptr);
                }
            }
        } else if (events[i].type == EVENT_MOUSE && events[i].mouse_event.event == MOUSE_EVENT_BUTTON_RELEASED) {
            if (ui_component_is_inside(&self->base, events[i].mouse_event.position_x, events[i].mouse_event.position_y)) {
                self->flags.clicked = 0;
                self->base.properties.border_color = PEN_INDEX_DARK_YELLOW;
                self->base.flags.dirty_flag = 1;
                if (self->functions.on_clicked) {
                    self->functions.on_clicked(self, app, usr_ptr);
                }
            }
        }
    }
}
