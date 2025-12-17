#include <exdevgfx/ui/ui.h>
#include <exdevgfx/exdev_base.h>

#define EXDEVGFX2_LOG_LEVEL 2

#include <exdevgfx/logger.h>

#define WIDTH 640
#define HEIGHT 480
#define UPDATE_INTERVAL 50// ms

void left_update(UIComponent_t *self, const exdev_timestamp_t ms, const Event_t *events, const int num_events) {
    ui_component_update(self, ms, events, num_events);// call update of base class

    // handle mouse event
    for (int it_events = 0; it_events < num_events; ++it_events) {
        if (events[it_events].type == EVENT_MOUSE) {
            if (ui_component_is_inside(self, events[it_events].mouse_event.position_x, events[it_events].mouse_event.position_y)) {
                switch (events[it_events].mouse_event.event) {
                    case MOUSE_EVENT_MOVED:
                        log_debug("component left got mouse move event");
                        break;
                    case MOUSE_EVENT_BUTTON_PRESSED:
                        log_debug("component left got mouse button pressed event");
                        break;
                    case MOUSE_EVENT_BUTTON_RELEASED:
                        log_debug("component left got mouse button released event");
                        break;
                    case MOUSE_EVENT_INVALID:
                        log_warning("component left got invalid mouse event");
                        break;
                }
            }
        }
    }
}

int right_paint(UIComponent_t *self, Framebuffer8Bit_t *fb) {
    const int tmp = self->flags.dirty_flag;
    int res = ui_component_paint(self, fb);
    if (tmp) {
        framebuffer_8bit_fill_rect(fb, self->properties.x + 2, self->properties.y, self->properties.width - 3, 10, PEN_INDEX_BLUE);
        self->flags.dirty_flag = 0;
        res = 1;
    }
    return res;
}

void right_update(UIComponent_t *self, const exdev_timestamp_t ms, const Event_t *events, const int num_events) {
    ui_component_update(self, ms, events, num_events);// call update of base class

    // handle mouse event
    for (int it_events = 0; it_events < num_events; ++it_events) {
        if (events[it_events].type == EVENT_MOUSE) {
            if (ui_component_is_inside(self, events[it_events].mouse_event.position_x, events[it_events].mouse_event.position_y)) {
                switch (events[it_events].mouse_event.event) {
                    case MOUSE_EVENT_MOVED:
                        log_debug("component right got mouse move event");
                        break;
                    case MOUSE_EVENT_BUTTON_PRESSED:
                        log_debug("component right got mouse button pressed event");
                        break;
                    case MOUSE_EVENT_BUTTON_RELEASED:
                        log_debug("component right got mouse button released event");
                        break;
                    case MOUSE_EVENT_INVALID:
                        log_warning("component right got invalid mouse event");
                        break;
                }
            }
        }
    }
}

int main() {
    exdev_base_init();

    // application
    Application_t app;
    application_init(&app, WIDTH, HEIGHT);
    palette_8bit_read_from_dat(&app.palette, "assets/amiga_logo_8bit.pal");

    // left component
    UIComponent_t *left = ui_component_create(2, 2, 538, 476);
    left->functions.update_func = (void (*)(void *, exdev_timestamp_t, const Event_t *, int)) &left_update;
    ui_component_connect(&app.root, left);

    // icon
    UIIcon_t *icon = ui_icon_create_with_path(4, 4, "assets/amiga_logo_8bit.dat");
    ui_component_connect(left, icon);

    // right component
    UIComponent_t *right = ui_component_create(542, 2, 96, 476);
    right->functions.paint_func = (int (*)(void *, Framebuffer8Bit_t *)) &right_paint;
    right->functions.update_func = (void (*)(void *, exdev_timestamp_t, const Event_t *, int)) &right_update;
    ui_component_connect(&app.root, right);

    application_run(&app, UPDATE_INTERVAL);
    application_destroy(&app);

    exdev_base_deinit();

    return 0;
}