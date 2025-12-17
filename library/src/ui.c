#include "exdevgfx/ui/ui.h"
#include "exdevgfx/helper.h"

#define EXDEVGFX2_LOG_LEVEL 1

#include "exdevgfx/logger.h"

#include <assert.h>
#include <stdlib.h>

void ui_component_list_init(UIComponentList_t *self) {
    assert(self);

    self->components = NULL;
    self->size = 0;
}

void ui_component_list_destroy(UIComponentList_t *self) {
    assert(self);

    for (int i = 0; i < self->size; ++i) {
        self->components[i]->functions.destroy_func(self->components[i]);
        free(self->components[i]);
        self->components[i] = NULL;
    }
    free(self->components);
}

void ui_component_list_add(UIComponentList_t *self, UIComponent_t *component) {
    assert(self);
    assert(component);

    if (self->size > 0) {
        self->components = realloc(self->components, sizeof(UIComponent_t *) * (self->size + 1));
        self->size += 1;
    } else {
        self->components = malloc(sizeof(UIComponent_t *));
        self->size = 1;
    }
    self->components[self->size - 1] = component;
}

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
    self->functions.update_func = (void (*)(void *, exdev_timestamp_t, const Event_t *, int)) &ui_component_update;

    self->parent = NULL;
    ui_component_list_init(&self->childs);
}

void ui_component_destroy(UIComponent_t *self) {
    ui_component_list_destroy(&self->childs);
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

    for (int i = 0; i < self->childs.size; ++i) {
        if (self->childs.components[i]->functions.paint_func(self->childs.components[i], fb, x, y, self->properties.width, self->properties.height)) {
            res = 1;
        }
    }
    return res;
}

void ui_component_update(UIComponent_t *self, const exdev_timestamp_t time_elapsed, const Event_t *events, const int num_events) {
    assert(self);

    if (!self->flags.enabled_flag) {
        return;
    }

    for (int i = 0; i < self->childs.size; ++i) {
        self->childs.components[i]->functions.update_func(self->childs.components[i], time_elapsed, events, num_events);
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

    ui_component_list_add(&parent->childs, child);
    child->parent = parent;
}

UIIcon_t *ui_icon_create(const int x, const int y, Framebuffer8Bit_t *fb) {
    assert(fb);

    UIIcon_t *self = malloc(sizeof(UIIcon_t));
    ui_icon_init(self, x, y, fb);

    return self;
}

UIIcon_t *ui_icon_create_with_path(const int x, const int y, const char *path) {
    assert(path);

    Framebuffer8Bit_t *icon_fb = malloc(sizeof(Framebuffer8Bit_t));
    framebuffer_8bit_read_from_dat(icon_fb, path);
    return ui_icon_create(x, y, icon_fb);
}

void ui_icon_init(UIIcon_t *self, const int x, const int y, Framebuffer8Bit_t *fb) {
    assert(self);
    assert(fb);

    ui_component_init(&self->base, x, y, fb->width + 2, fb->height + 2);
    self->base.type = UI_COMPONENT_ICON;
    self->base.functions.destroy_func = (void (*)(void *)) &ui_icon_destroy;
    self->base.functions.paint_func = (int (*)(void *, Framebuffer8Bit_t *, int, int, int, int)) &ui_icon_paint;
    self->base.functions.update_func = (void (*)(void *, exdev_timestamp_t, const Event_t *, int)) &ui_icon_update;

    self->icon = fb;
    self->flags.clickable = 1;
    self->flags.focus = 0;

    self->functions.on_clicked = NULL;
    self->functions.on_focus = NULL;
}

void ui_icon_destroy(UIIcon_t *self) {
    assert(self);
    ui_component_destroy(&self->base);
    framebuffer_8bit_deinit(self->icon);
    free(self->icon);
    self->icon = NULL;
}

int ui_icon_paint(UIIcon_t *self, Framebuffer8Bit_t *fb, const int x_offset, const int y_offset, const int width, const int height) {
    assert(self);
    assert(fb);

    const int res = self->base.flags.dirty_flag;
    const int x = self->base.properties.x + x_offset;
    const int y = self->base.properties.y + y_offset;

    // draw base
    ui_component_paint(&self->base, fb, x_offset, y_offset, width, height);
    if (res) {
        framebuffer_8bit_draw_framebuffer(fb, x + 1, y + 1, self->icon);
    }
    return res;
}

void ui_icon_update(UIIcon_t *self, const exdev_timestamp_t time_elapsed, const Event_t *events, const int num_events) {
    assert(self);

    if (!self->base.flags.enabled_flag) {
        return;
    }

    ui_component_update(&self->base, time_elapsed, events, num_events);

    if (!self->flags.clickable) {
        return;
    }

    for (int i = 0; i < num_events; ++i) {
        if (events[i].type == EVENT_MOUSE && events[i].mouse_event.event == MOUSE_EVENT_MOVED) {
            if (ui_component_is_inside(&self->base, events[i].mouse_event.position_x, events[i].mouse_event.position_y)) {
                if (!self->flags.focus) {
                    self->flags.focus = 1;
                    self->base.properties.border_color = PEN_INDEX_DARK_YELLOW;
                    self->base.flags.dirty_flag = 1;
                    if (self->functions.on_focus) {
                        self->functions.on_focus(self);
                    }
                }
            } else {
                if (self->flags.focus) {
                    self->flags.focus = 0;
                    self->base.properties.border_color = PEN_INDEX_BLUE;
                    self->base.flags.dirty_flag = 1;
                    if (self->functions.on_focus) {
                        self->functions.on_focus(self);
                    }
                }
            }
        } else if (events[i].type == EVENT_MOUSE && events[i].mouse_event.event == MOUSE_EVENT_BUTTON_PRESSED) {
            if (ui_component_is_inside(&self->base, events[i].mouse_event.position_x, events[i].mouse_event.position_y)) {
                self->flags.clicked = 1;
                self->base.properties.border_color = PEN_INDEX_YELLOW;
                self->base.flags.dirty_flag = 1;
                if (self->functions.on_clicked) {

                    self->functions.on_clicked(self);
                }
            }
        } else if (events[i].type == EVENT_MOUSE && events[i].mouse_event.event == MOUSE_EVENT_BUTTON_RELEASED) {
            if (ui_component_is_inside(&self->base, events[i].mouse_event.position_x, events[i].mouse_event.position_y)) {
                self->flags.clicked = 0;
                self->base.properties.border_color = PEN_INDEX_DARK_YELLOW;
                self->base.flags.dirty_flag = 1;
                if (self->functions.on_clicked) {
                    self->functions.on_clicked(self);
                }
            }
        }
    }
}

void application_init(Application_t *self, const int width, const int height) {
    assert(self);

    ui_component_init(&self->root, 0, 0, width, height);
    palette_8bit_init(&self->palette, 0);
    self->resume = 1;
}

void application_destroy(Application_t *self) {
    assert(self);

    window_destroy(self->window);
    self->window = NULL;
    ui_component_destroy(&self->root);
    self->resume = 0;
}

#define MAX_EVENTS 4

int application_run(Application_t *self, const exdev_timestamp_t wait_ms) {
    assert(self);

    char close_event = 0;
    Event_t events[MAX_EVENTS];
    int num_events = 0;
    exdev_timestamp_t begin_ms = 0;
    exdev_timestamp_t loop_time_ms = 0;
    exdev_timestamp_t sleep_ms = 0;
    exdev_timestamp_t end_ms = 0;

    // open window
    self->window = window_create(self->root.properties.width, self->root.properties.height, "app", FS_8_BIT);

    // setup palette
    palette_8bit_set_pen(&self->palette, &PEN_BLACK, PEN_INDEX_BLACK);
    palette_8bit_set_pen(&self->palette, &PEN_WHITE, PEN_INDEX_WHITE);

    palette_8bit_set_pen(&self->palette, &PEN_DARK_RED, PEN_INDEX_DARK_RED);
    palette_8bit_set_pen(&self->palette, &PEN_RED, PEN_INDEX_RED);

    palette_8bit_set_pen(&self->palette, &PEN_DARK_GREEN, PEN_INDEX_DARK_GREEN);
    palette_8bit_set_pen(&self->palette, &PEN_GREEN, PEN_INDEX_GREEN);

    palette_8bit_set_pen(&self->palette, &PEN_DARK_BLUE, PEN_INDEX_DARK_BLUE);// background
    palette_8bit_set_pen(&self->palette, &PEN_BLUE, PEN_INDEX_BLUE);          // border

    palette_8bit_set_pen(&self->palette, &PEN_DARK_YELLOW, PEN_INDEX_DARK_YELLOW);
    palette_8bit_set_pen(&self->palette, &PEN_YELLOW, PEN_INDEX_YELLOW);

    palette_8bit_set_pen(&self->palette, &PEN_DARK_GRAY, PEN_INDEX_DARK_GRAY);
    palette_8bit_set_pen(&self->palette, &PEN_GRAY, PEN_INDEX_GRAY);

    palette_8bit_set_pen(&self->palette, &PEN_CYAN, PEN_INDEX_CYAN);

    window_update_palette(self->window, &self->palette);

    // loop
    while (self->resume) {
        begin_ms = now();
        // ui events
        num_events = window_poll_events(self->window, &close_event, events, MAX_EVENTS - 1);
        for (int it_events = 0; it_events < num_events; ++it_events) {
            if (events[it_events].type == EVENT_KEY && events[it_events].key_event.event == KEY_EVENT_PRESSED) {
                switch (events[it_events].key_event.key) {
                    case KEY_TYPE_ESC:
                        self->resume = 0;
                        break;
                    default:
                        break;
                }
            }
        }

        if (window_get_mouse_position(self->window, &events[num_events])) {
            ++num_events;
        }
        // update ui
        self->root.functions.update_func(&self->root, end_ms - begin_ms, events, num_events);

        // paint ui
        if (self->root.functions.paint_func(&self->root,
                                            window_get_chunky_buffer(self->window),
                                            0,
                                            0,
                                            window_get_chunky_buffer(self->window)->width,
                                            window_get_chunky_buffer(self->window)->height)) {
            // blit ui to screen
            log_debug("blit to screen required");
            window_blit_chunky_buffer(self->window);
        }


        // wait some time
        loop_time_ms = now() - begin_ms;
        if (wait_ms > 0) {
            sleep_ms = wait_ms - loop_time_ms;
            if (sleep_ms <= 0) {
                log_warning("running out of time - will sleep anyway");
                sleep_ms = 10;
            }
            sleep_for_ms(sleep_ms);
        }
        end_ms = now();
    }

    return 0;
}
