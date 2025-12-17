#include <exdevgfx/ui/ui.h>
#include <exdevgfx/exdev_base.h>

#define EXDEVGFX2_LOG_LEVEL 2

#include <exdevgfx/logger.h>

#define WIDTH 640
#define HEIGHT 480
#define UPDATE_INTERVAL 50// ms

void left_update(UIComponent_t *self, const long ms, const Event_t *events, const int num_events) {
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

int right_paint(UIComponent_t *self, Framebuffer8Bit_t *fb, const int x_offset, const int y_offset, int, int) {
    const int tmp = self->flags.dirty_flag;

    const int x = self->properties.x + x_offset;
    const int y = self->properties.y + y_offset;

    int res = ui_component_paint(self, fb, x_offset, y_offset, self->properties.width, self->properties.height);
    if (tmp) {
        framebuffer_8bit_fill_rect(fb, x + 2, y + 2, self->properties.width - 3, 10, PEN_INDEX_BLUE);
        self->flags.dirty_flag = 0;
        res = 1;
    }
    return res;
}

void right_update(UIComponent_t *self, const long ms, const Event_t *events, const int num_events) {
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

static void icon_clicked(UIIcon_t *self) {
    log_info_fmt("icon clicked: %d", self->flags.clicked);
}

static void icon_focus(UIIcon_t *self) {
    log_info_fmt("icon focus: %d", self->flags.focused);
}

#if defined(__MORPHOS__) || defined(__AMIGAOS__)
unsigned long __stack = (16 * 1024 * 1024);
#endif

int main() {
    exdev_base_init();

    // application
    UIApplication_t app;
    application_init(&app, WIDTH, HEIGHT);
    palette_8bit_read_from_dat(&app.palette, "assets/amiga_logo_8bit.pal");// read palette from file

    // left component
    UIComponent_t *left = ui_component_create(2, 2, 538, 476);
    left->functions.update_func = (void (*)(void *, long, const Event_t *, int)) &left_update;// custom event handling
    ui_component_connect(&app.root, left);

    // icon
    UIIcon_t *icon = ui_icon_create_with_path(2, 2, "assets/amiga_logo_8bit.dat");
    icon->functions.on_clicked = &icon_clicked;
    icon->functions.on_focus = &icon_focus;
    ui_component_connect(left, icon);

    // scroll component
    UIScrollPane_t *scroll = ui_scroll_create(100, 100, 100, 100, UI_SCROLLING_SUPPORT_HORIZONTAL_AND_VERTICAL);
    ui_component_connect(scroll, ui_icon_create_with_path(2, 2, "assets/amiga_logo_8bit.dat"));
    ui_component_connect(scroll, ui_icon_create_with_path(68, 68, "assets/amiga_logo_8bit.dat"));
    ui_component_connect(&app.root, scroll);

    // right component
    UIComponent_t *right = ui_component_create(542, 2, 96, 476);
    right->functions.paint_func = (int (*)(void *, Framebuffer8Bit_t *, int, int, int, int)) &right_paint;   // custom paint
    right->functions.update_func = (void (*)(void *, long, const Event_t *, int)) &right_update;// custom event handling
    ui_component_connect(&app.root, right);

    // run
    application_run(&app, UPDATE_INTERVAL);

    // cleanup
    application_destroy(&app);
    exdev_base_deinit();

    return 0;
}