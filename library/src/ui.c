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

static Color8Bit_t BACKGROUND_COLOR = 0;
static Color8Bit_t BORDER_COLOR = 0;
void ui_component_init(UIComponent_t *self, const int x, const int y, const int width, const int height, UIComponent_t *parent) {
    assert(self);
    self->type = UI_COMPONENT_BASE;
    self->subtype = 0;
    self->properties.x = x;
    self->properties.y = y;
    self->properties.width = width;
    self->properties.height = height;

    self->flags.dirty_flag = 1;
    self->flags.enabled_flag = 1;
    self->flags.fill_background = 1;
    self->flags.draw_border = 1;

    self->functions.destroy_func = (void (*)(void *)) &ui_component_destroy;
    self->functions.paint_func = (void (*)(void *, Framebuffer8Bit_t *)) &ui_component_paint;
    self->functions.update_func = (void (*)(void *, exdev_timestamp_t, const Event_t *, int)) &ui_component_update;

    self->parent = parent;
    ui_component_list_init(&self->childs);
}

void ui_component_destroy(UIComponent_t *self) {
    ui_component_list_destroy(&self->childs);
}

void ui_component_paint(UIComponent_t *self, Framebuffer8Bit_t *fb) {
    assert(self);
    assert(fb);

    if (self->flags.dirty_flag) {
        if (self->flags.fill_background) {
            framebuffer_8bit_fill_rect(fb, self->properties.x, self->properties.y, self->properties.width, self->properties.height, BACKGROUND_COLOR);
        }

        if (self->flags.draw_border) {
            framebuffer_8bit_draw_rect(fb, self->properties.x, self->properties.y, self->properties.width, self->properties.height, BORDER_COLOR);
        }

        self->flags.dirty_flag = 0;
    }

    for (int i = 0; i < self->childs.size; ++i) {
        self->childs.components[i]->functions.paint_func(self->childs.components[i], fb);
    }
}

void ui_component_update(UIComponent_t *self, const exdev_timestamp_t time_elapsed, const Event_t *events, const int num_events) {
    assert(self);

    for (int i = 0; i < self->childs.size; ++i) {
        self->childs.components[i]->functions.update_func(self->childs.components[i], time_elapsed, events, num_events);
    }
}

int ui_component_is_inside(const UIComponent_t *self, const int x, const int y) {
    assert(self);

    return x >= self->properties.x && x < self->properties.x + self->properties.width &&
           y >= self->properties.y && y < self->properties.y + self->properties.height;
}

void ui_icon_init(UIIcon_t *self, int x, int y, Framebuffer8Bit_t *fb, UIComponent_t *parent) {
    assert(self);
    assert(fb);

    ui_component_init(&self->base, x, y, fb->width + 2, fb->height + 2, parent);
    self->base.type = UI_COMPONENT_ICON;
    self->base.functions.destroy_func = (void (*)(void *)) &ui_icon_destroy;
    self->base.functions.paint_func = (void (*)(void *, Framebuffer8Bit_t *)) &ui_icon_paint;
    self->base.functions.update_func = (void (*)(void *, exdev_timestamp_t, const Event_t *, int)) &ui_icon_update;

    self->icon = fb;
    self->flags.clickable = 1;
}

void ui_icon_destroy(UIIcon_t *self) {
    assert(self);
    ui_component_destroy(&self->base);
    framebuffer_8bit_deinit(self->icon);
    free(self->icon);
    self->icon = NULL;
}

void ui_icon_paint(UIIcon_t *self, Framebuffer8Bit_t *fb) {
    assert(self);
    assert(fb);

    const int tmp = self->base.flags.dirty_flag;
    ui_component_paint(&self->base, fb);

    if (tmp) {
        framebuffer_8bit_draw_framebuffer(fb, self->base.properties.x + 1, self->base.properties.y + 1, self->icon);
    }
}

void ui_icon_update(UIIcon_t *self, const exdev_timestamp_t time_elapsed, const Event_t *events, const int num_events) {
    assert(self);

    ui_component_update(&self->base, time_elapsed, events, num_events);
}

void application_init(Application_t *self, const int width, const int height) {
    assert(self);

    ui_component_init(&self->root, 0, 0, width, height, NULL);
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

#define MAX_EVENTS 64

int application_run(Application_t *self, exdev_timestamp_t wait_ms) {
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
    palette_8bit_add_pen(&self->palette, &PEN_BLACK);
    palette_8bit_add_pen(&self->palette, &PEN_WHITE);
    palette_8bit_add_pen(&self->palette, &PEN_RED);
    palette_8bit_add_pen(&self->palette, &PEN_DARK_RED);
    palette_8bit_add_pen(&self->palette, &PEN_GREEN);
    palette_8bit_add_pen(&self->palette, &PEN_DARK_GREEN);
    BORDER_COLOR = palette_8bit_add_pen(&self->palette, &PEN_BLUE);
    BACKGROUND_COLOR = palette_8bit_add_pen(&self->palette, &PEN_DARK_BLUE);
    palette_8bit_add_pen(&self->palette, &PEN_YELLOW);
    palette_8bit_add_pen(&self->palette, &PEN_DARK_YELLOW);
    palette_8bit_add_pen(&self->palette, &PEN_CYAN);
    window_update_palette(self->window, &self->palette);

    // loop
    while (self->resume) {
        begin_ms = now();
        // ui events
        num_events = window_poll_events(self->window, &close_event, events, MAX_EVENTS);
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

        // update ui
        self->root.functions.update_func(&self->root, end_ms - begin_ms, events, num_events);

        // paint ui
        self->root.functions.paint_func(&self->root, window_get_chunky_buffer(self->window));

        // blit ui to screen
        window_blit_chunky_buffer(self->window);

        // wait some time
        loop_time_ms = now() - begin_ms;
        if (wait_ms > 0) {
            sleep_ms = wait_ms - loop_time_ms;
            if (sleep_ms <= 0) {
                log_warning("running out of time - will sleep anyway");
                sleep_ms = 10;
            }
            log_debug_fmt("sleep for: %dms", sleep_ms);
            sleep_for_ms(sleep_ms);
        }
        end_ms = now();
    }

    return 0;
}
