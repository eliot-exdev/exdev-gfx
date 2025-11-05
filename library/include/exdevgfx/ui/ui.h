#ifndef EXDEVGFX_BASE_UI_H
#define EXDEVGFX_BASE_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <exdevgfx/framebuffer_8bit.h>
#include <exdevgfx/events.h>
#include <exdevgfx/window.h>
#include <exdevgfx/helper.h>


enum ui_component_type {
    UI_COMPONENT_CONTAINER,
    UI_COMPONENT_CUSTOM
};

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

typedef void (*paint_function)(void *self, Framebuffer8Bit_t *fb);

struct UIComponent {
    enum ui_component_type type;
    int subtype;
    struct {
        int x;
        int y;
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
    } functions;

    struct UIComponent *parent;
    struct UIComponentList childs;
};

typedef struct UIComponent UIComponent_t;

void ui_component_init(UIComponent_t *self, int x, int y, int width, int height, UIComponent_t *parent);

void ui_component_destroy(UIComponent_t *self);

int ui_component_get_x_abs(const UIComponent_t *self);

int ui_component_get_y_abs(const UIComponent_t *self);

void ui_component_paint(UIComponent_t *self, Framebuffer8Bit_t *fb);

struct Application {
    Window_t *window;
    UIComponent_t *root;
    Palette8Bit_t *palette;
    int resume;
};

typedef struct Application Application_t;

void application_init(Application_t *self, const char* name, int width, int height);

void application_destroy(Application_t *self);

int application_run(Application_t *self, exdev_timestamp_t wait_ms);

#ifdef __cplusplus
}
#endif

#endif//EXDEVGFX_BASE_UI_H
