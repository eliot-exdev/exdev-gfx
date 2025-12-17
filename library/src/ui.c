#include "exdevgfx/ui/ui.h"
#include "exdevgfx/helper.h"

#define EXDEVGFX2_LOG_LEVEL 2

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
        ui_component_destroy(self->components[i]);
        free(self->components[i]);
    }
}

void ui_component_list_add(UIComponentList_t *self, UIComponent_t *component) {
    assert(self);
    assert(component);

    if (self->size > 0) {
        self->components = realloc(self->components, sizeof(self->components) * (self->size + 1));
        self->size += 1;
    } else {
        self->components = malloc(sizeof(self->components));
        self->size = 1;
    }
    self->components[self->size - 1] = component;
}

void ui_component_init(UIComponent_t *self, const int x, const int y, const int width, const int height, UIComponent_t *parent) {
    assert(self);
    self->type = UI_COMPONENT_CONTAINER;
    self->subtype = 0;
    self->properties.x = x;
    self->properties.y = y;
    self->properties.width = width;
    self->properties.height = height;
    self->properties.background_color = 0;
    self->properties.border_color = 1;

    self->flags.dirty_flag = 1;
    self->flags.enabled_flag = 1;
    self->flags.fill_background = 1;
    self->flags.draw_border = 1;

    self->functions.destroy_func = (void (*)(void *)) &ui_component_destroy;
    self->functions.paint_func = (void (*)(void *, Framebuffer8Bit_t *, int, int)) &ui_component_paint;

    self->parent = parent;
    ui_component_list_init(&self->childs);
}

void ui_component_destroy(UIComponent_t *self) {
    for (int i = 0; i < self->childs.size; ++i) {
        ui_component_destroy(self->childs.components[i]);
        free(self->childs.components[i]);
        self->childs.components[i] = NULL;
    }
}

int ui_component_get_x_abs(const UIComponent_t *self) {
    assert(self);

    if (self->parent) {
        return ui_component_get_x_abs(self->parent) + self->properties.x;
    }
    return self->properties.x;
}

int ui_component_get_y_abs(const UIComponent_t *self) {
    assert(self);

    if (self->parent) {
        return ui_component_get_y_abs(self->parent) + self->properties.y;
    }
    return self->properties.y;
}

void ui_component_paint(UIComponent_t *self, Framebuffer8Bit_t *fb, const int x, const int y) {
    assert(self);
    assert(fb);

    if (self->flags.dirty_flag) {
        if (self->flags.fill_background) {
            framebuffer_8bit_fill_rect(fb, x, y, self->properties.width, self->properties.height, self->properties.background_color);
        }

        if (self->flags.draw_border) {
            framebuffer_8bit_draw_rect(fb, x, y, self->properties.width, self->properties.height, self->properties.border_color);
        }

        self->flags.dirty_flag = 0;
    }

    for (int i = 0; i < self->childs.size; ++i) {
        self->childs.components[i]->functions.paint_func(self,
                                                         fb,
                                                         x + self->childs.components[i]->properties.x,
                                                         y + self->childs.components[i]->properties.y);
    }
}

void application_init(Application_t *self, const char *name, const int width, const int height) {
    assert(self);

    self->window = window_create(width, height, name, FS_8_BIT);

    self->root = malloc(sizeof(UIComponent_t));
    ui_component_init(self->root, 0, 0, width, height, NULL);

    self->palette = malloc(sizeof(Palette8Bit_t));
    palette_8bit_init(self->palette, 2);
    palette_8bit_set_pen(self->palette, &PEN_BLACK, 0); // background
    palette_8bit_set_pen(self->palette, &PEN_WHITE, 1); // border

    window_update_palette(self->window, self->palette);

    self->resume = 1;
}

void application_destroy(Application_t *self) {
    assert(self);

    window_destroy(self->window);
    self->window = NULL;

    ui_component_destroy(self->root);
    free(self->root);
    self->root = NULL;

    free(self->palette);
    self->palette = NULL;

    self->resume = 0;
}

int application_run(Application_t *self, exdev_timestamp_t wait_ms) {
    assert(self);

    char close_event = 0;
    Event_t event;

    while (self->resume) {
        const exdev_timestamp_t before = now();

        // handle events
        window_poll_events(self->window, &close_event, &event, 1);
        if (event.type == EVENT_KEY && event.key_event.event == KEY_EVENT_PRESSED) {
            switch (event.key_event.key) {
                case KEY_TYPE_ESC:
                    self->resume = 0;
                    break;
                default:
                    break;
            }
        }

        // redraw ui
        self->root->functions.paint_func(self->root, window_get_chunky_buffer(self->window), 0, 0);
        window_blit_chunky_buffer(self->window);

        // wait some time
        const exdev_timestamp_t total = now() - before;
        if (wait_ms > 0) {
            exdev_timestamp_t sleep_ms = wait_ms - total;
            if (sleep_ms <= 0) {
                log_warning("running out of time - will sleep anyway");
                sleep_ms = 10;
            }
            log_debug_fmt("sleep for: %dms", sleep_ms);
            sleep_for_ms(sleep_ms);
        }
    }

    return 0;
}
