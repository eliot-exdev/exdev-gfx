#ifndef EXDEVGFX_BASE_UI_H
#define EXDEVGFX_BASE_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <exdevgfx/framebuffer_8bit.h>
#include <exdevgfx/events.h>
#include <exdevgfx/window.h>
#include <exdevgfx/helper.h>

#define PEN_INDEX_BLACK 0
#define PEN_INDEX_WHITE 1

#define PEN_INDEX_DARK_RED 2
#define PEN_INDEX_RED 3

#define PEN_INDEX_GREEN 4
#define PEN_INDEX_DARK_GREEN 5

#define PEN_INDEX_DARK_BLUE 6
#define PEN_INDEX_BLUE 7

#define PEN_INDEX_DARK_YELLOW 8
#define PEN_INDEX_YELLOW 9

#define PEN_INDEX_DARK_GRAY 10
#define PEN_INDEX_GRAY 11

#define PEN_INDEX_CYAN 12

//--- UIComponent ---//
enum ui_component_type {
    UI_COMPONENT_BASE,
    UI_COMPONENT_ICON,
    UI_COMPONENT_SCROLL,
    UI_COMPONENT_CUSTOM
};

typedef enum ui_component_type UIComponentType_t;

struct UIComponent;

struct UIComponentList {
    struct UIComponent **components;
    int size;
};
typedef struct UIComponentList UIComponentList_t;

void ui_component_list_init(UIComponentList_t *self);

void ui_component_list_destroy(UIComponentList_t *self);

void ui_component_list_add(UIComponentList_t *self, struct UIComponent *component);

typedef void (*destroy_function)(void *self);

typedef int (*paint_function)(void *self, Framebuffer8Bit_t *fb, int x_offset, int y_offset, int width, int height);

typedef void (*update_function)(void *self, exdev_timestamp_t time_elapsed, const Event_t *events, int num_events);

struct UIComponent {
    UIComponentType_t type;
    int subtype;
    struct {
        int x;// relative to parent
        int y;// relative to parent
        int width;
        int height;
        Color8Bit_t background_color;
        Color8Bit_t border_color;
    } properties;
    struct {
        int dirty_flag;
        int enabled_flag;
        int fill_background;
        int draw_border;
    } flags;

    struct {
        destroy_function destroy_func;
        paint_function paint_func;
        update_function update_func;
    } functions;

    struct UIComponent *parent;
    struct UIComponentList childs;
};

typedef struct UIComponent UIComponent_t;

void ui_component_init(UIComponent_t *self, int x, int y, int width, int height);

UIComponent_t *ui_component_create(int x, int y, int width, int height);

void ui_component_destroy(UIComponent_t *self);

int ui_component_paint(UIComponent_t *self, Framebuffer8Bit_t *fb, int x_offset, int y_offset, int width, int height);

void ui_component_update(UIComponent_t *self, exdev_timestamp_t time_elapsed, const Event_t *events, int num_events);

int ui_component_is_inside(const UIComponent_t *self, int x, int y);

void ui_component_connect(void *parent, void *child);

void ui_component_get_absolute_position(const UIComponent_t *self, int *x, int *y);

//--- UIIcon ---//
struct UIIcon;

typedef void (*on_focus_function)(struct UIIcon *self);
typedef void (*on_clicked_function)(struct UIIcon *self);

struct UIIcon {
    UIComponent_t base;
    Framebuffer8Bit_t *icon;
    struct {
        int clickable;
    } properties;

    struct {
        int focused;
        int clicked;
    } flags;

    struct {
        on_focus_function on_focus;
        on_clicked_function on_clicked;
    } functions;
};

typedef struct UIIcon UIIcon_t;

void ui_icon_init(UIIcon_t *self, int x, int y, Framebuffer8Bit_t *fb);

UIIcon_t *ui_icon_create(int x, int y, Framebuffer8Bit_t *fb);

UIIcon_t *ui_icon_create_with_path(int x, int y, const char *path);

void ui_icon_destroy(UIIcon_t *self);

int ui_icon_paint(UIIcon_t *self, Framebuffer8Bit_t *fb, int x_offset, int y_offset, int width, int height);

void ui_icon_update(UIIcon_t *self, exdev_timestamp_t time_elapsed, const Event_t *events, int num_events);

//--- UIApplication ---//
struct UIApplication {
    Window_t *window;
    UIComponent_t root;
    Palette8Bit_t palette;
    int resume;
};

typedef struct UIApplication UIApplication_t;

void application_init(UIApplication_t *self, int width, int height);

void application_destroy(UIApplication_t *self);

int application_run(UIApplication_t *self, exdev_timestamp_t wait_ms);

#ifdef __cplusplus
}
#endif

#endif//EXDEVGFX_BASE_UI_H
