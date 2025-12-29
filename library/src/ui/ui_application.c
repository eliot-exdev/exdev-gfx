/**
 * Copyright 2025 Andre Geisler (andre@exdev.de)
 */

#include "exdevgfx/ui/ui.h"
#include "exdevgfx/helper.h"

#define EXDEVGFX2_LOG_LEVEL 2
#include "exdevgfx/logger.h"

#include <assert.h>

void ui_application_init(UIApplication_t *self, const int width, const int height) {
    assert(self);

    ui_component_init(&self->root, 0, 0, width, height);
    palette_8bit_init(&self->palette, 0);
    self->resume = 1;
}

void ui_application_destroy(UIApplication_t *self) {
    assert(self);

    if (self->window) {
        window_destroy(self->window);
        self->window = NULL;
    }
    ui_component_destroy(&self->root);
    self->resume = 0;
}

void ui_application_prepare(UIApplication_t *self) {
    assert(self);

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
    self->root.functions.prepare_func(&self->root);
}

#define MAX_EVENTS 4

int ui_application_run(UIApplication_t *self, const char *title, const exdev_timestamp_t wait_ms) {
    assert(self);
    assert(title);
    assert(wait_ms > 0);

    char close_event = 0;
    Event_t events[MAX_EVENTS];
    int num_events = 0;
    exdev_timestamp_t begin_ms = 0;
    exdev_timestamp_t loop_time_ms = 0;
    exdev_timestamp_t sleep_ms = 0;
    exdev_timestamp_t end_ms = 0;

    // open window
    self->window = window_create(self->root.properties.width, self->root.properties.height, title, FS_8_BIT);
    if (!self->window) {
        log_warning("failed to create window");
        return 1;
    }
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
