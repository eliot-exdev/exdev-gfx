#ifndef EXDEVGFX_BASE_UI_H
#define EXDEVGFX_BASE_UI_H

#ifdef __cplusplus
extern "C" {
#endif
#include <exdevgfx/framebuffer_8bit.h>
#include <exdevgfx/events.h>

enum ui_component_type {
    UI_COMPONENT_CONTAINER,
    UI_COMPONENT_CUSTOM
};

struct UIComponentList_t;

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

    struct UIComponent *parent;
    struct UIComponentList_t *childs;
};

typedef struct UIComponent UIComponent_t;

void ui_component_init(UIComponent_t *self, int x, int y, int width, int height, UIComponent_t *parent);

void ui_component_destroy(UIComponent_t *self);

int ui_component_get_x_abs(const UIComponent_t *self);

int ui_component_get_y_abs(const UIComponent_t *self);

void ui_component_paint(UIComponent_t *self, Framebuffer8Bit_t *fb);

struct UIComponentList {
    UIComponent_t *components;
    int size;
};
typedef struct UIComponentList UIComponentList_t;

void ui_component_list_init(UIComponentList_t *self);

void ui_component_list_destroy(UIComponentList_t *self);

void ui_component_list_add(UIComponentList_t *self, UIComponent_t *component);

#ifdef __cplusplus
}
#endif

#endif//EXDEVGFX_BASE_UI_H
