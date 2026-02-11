/**
 * Copyright 2025 Andre Geisler (andre@exdev.de)
 */

#ifndef EXDEVGFX_BASE_UI_H
#define EXDEVGFX_BASE_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <exdevgfx/window.h>
#include <exdevgfx/framebuffer_8bit.h>
#include <exdevgfx/events.h>

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
    UI_COMPONENT_TEXT,
    UI_COMPONENT_SCROLL_PANE,
    UI_COMPONENT_SCROLL_BAR_HORIZONTAL,
    UI_COMPONENT_SCROLL_BAR_VERTICAL,
    UI_COMPONENT_CUSTOM
};

typedef enum ui_component_type UIComponentType_t;

struct UIComponent;
struct UIApplication;

struct UIComponentList {
    struct UIComponent **components;
    int size;
};
typedef struct UIComponentList UIComponentList_t;

void ui_component_list_init(UIComponentList_t *self);

void ui_component_list_destroy(UIComponentList_t *self);

void ui_component_list_add(UIComponentList_t *self, struct UIComponent *component);

typedef void (*destroy_function)(void *self);

typedef int (*paint_function)(void *self, Framebuffer8Bit_t *fb, int x_offset, int y_offset, int width, int height, void *usr_ptr);

typedef void (*update_function)(void *self, long time_elapsed, const Event_t *events, int num_events, struct UIApplication *app, void *usr_ptr);

typedef void (*prepare_function)(void *self, void *usr_ptr);

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
        uint8_t dirty_flag;
        uint8_t enabled_flag;
        uint8_t fill_background;
        uint8_t draw_border;
    } flags;

    struct {
        destroy_function destroy_func;
        prepare_function prepare_func;
        paint_function paint_func;
        update_function update_func;
    } functions;

    struct UIComponent *parent;
    struct UIComponentList children;
};

typedef struct UIComponent UIComponent_t;

void ui_component_init(UIComponent_t *self, int x, int y, int width, int height);

UIComponent_t *ui_component_create(int x, int y, int width, int height);

void ui_component_destroy(UIComponent_t *self);

void ui_component_prepare(UIComponent_t *self, void *usr_ptr);

int ui_component_paint(UIComponent_t *self, Framebuffer8Bit_t *fb, int x_offset, int y_offset, int width, int height, void *usr_ptr);

void ui_component_update(UIComponent_t *self, long time_elapsed, const Event_t *events, int num_events, struct UIApplication *app, void *usr_ptr);

int ui_component_is_inside(const UIComponent_t *self, int x, int y);

void ui_component_set_dirty(UIComponent_t *self);

void ui_component_set_enable(UIComponent_t *self, uint8_t b);

void ui_component_connect(void *parent, void *child);

void ui_component_get_absolute_position(const UIComponent_t *self, int *x, int *y);

void ui_component_get_relative_position(const UIComponent_t *self, int *x, int *y);

//--- UIIcon ---//
struct UIIcon;

typedef void (*on_focus_function)(struct UIIcon *, struct UIApplication *, void *);

typedef void (*on_click_function)(struct UIIcon *, struct UIApplication *, void *);

struct UIIcon {
    UIComponent_t base;

    struct {
        uint8_t clickable;
    } properties;

    struct {
        uint8_t focused;
        uint8_t clicked;
    } flags;

    struct {
        on_focus_function on_focus;
        on_click_function on_clicked;
    } functions;

    Framebuffer8Bit_t *icon_enabled;
    Framebuffer8Bit_t *icon_disabled;
};

typedef struct UIIcon UIIcon_t;

void ui_icon_init(UIIcon_t *self, int x, int y, Framebuffer8Bit_t *icon_enabled, Framebuffer8Bit_t *icon_disabled);

UIIcon_t *ui_icon_create(int x, int y, Framebuffer8Bit_t *icon_enabled, Framebuffer8Bit_t *icon_disabled);

UIIcon_t *ui_icon_create_with_path(int x, int y, const char *path_icon_enabled, const char *path_icon_disabled);

void ui_icon_destroy(UIIcon_t *self);

int ui_icon_paint(UIIcon_t *self, Framebuffer8Bit_t *fb, int x_offset, int y_offset, int width, int height, void *usr_ptr);

void ui_icon_update(UIIcon_t *self, long time_elapsed, const Event_t *events, int num_events, struct UIApplication *app, void *usr_ptr);

//--- UIText ---//
struct UIText {
    UIComponent_t base;

    struct {
        char *text;
        const Font_t *font;
        Color8Bit_t text_color;
    } properties;
};

typedef struct UIText UIText_t;

void ui_text_init(UIText_t *self, int x, int y, int width, int height, const char *text, const Font_t *font);

UIText_t *ui_text_create(int x, int y, int width, int height, const char *text, const Font_t *font);

void ui_text_destroy(UIText_t *self);

int ui_text_paint(UIText_t *self, Framebuffer8Bit_t *fb, int x_offset, int y_offset, int width, int height, void *usr_ptr);

void ui_text_update_text(UIText_t *self, const char *text);

//--- UIScrollContainer ---//
#define SCROLL_BAR_SIZE 10

struct UIScrollContainer;
struct UIHorizontalScrollBar;
struct UIVerticalScrollBar;

typedef void (*on_x_offset_function)(struct UIScrollContainer *self, int x_offset);
typedef void (*on_y_offset_function)(struct UIScrollContainer *self, int y_offset);

enum ui_scrolling_support {
    UI_SCROLLING_SUPPORT_HORIZONTAL,
    UI_SCROLLING_SUPPORT_VERTICAL,
    UI_SCROLLING_SUPPORT_HORIZONTAL_AND_VERTICAL
};

typedef enum ui_scrolling_support UIScrollingSupport_t;

struct UIScrollContainer {
    UIComponent_t base;
    struct {
        int x_offset;
        int y_offset;
        UIScrollingSupport_t scrolling_support;
    } properties;

    struct {
        on_x_offset_function on_x_offset;
        on_y_offset_function on_y_offset;
    } functions;

    Framebuffer8Bit_t *fb;
    struct UIHorizontalScrollBar *h_bar;
    struct UIVerticalScrollBar *v_bar;
    int x_visible;
    int y_visible;
};

typedef struct UIScrollContainer UIScrollContainer_t;

void ui_scroll_container_init(UIScrollContainer_t *self, int x, int y, int width, int height, UIScrollingSupport_t scrolling_support);

UIScrollContainer_t *ui_scroll_container_create(int x, int y, int width, int height, UIScrollingSupport_t scrolling_su);

void ui_scroll_container_destroy(UIScrollContainer_t *self);

void ui_scroll_container_prepare(UIScrollContainer_t *self, void *usr_ptr);

int ui_scroll_container_paint(UIScrollContainer_t *self, Framebuffer8Bit_t *fb, int x_offset, int y_offset, int width, int height, void *usr_ptr);

void ui_scroll_container_update(UIScrollContainer_t *self, long time_elapsed, const Event_t *events, int num_events, struct UIApplication *app, void *usr_ptr);

void ui_scroll_container_on_x_offset(UIScrollContainer_t *self, int x_offset);

void ui_scroll_container_on_y_offset(UIScrollContainer_t *self, int y_offset);

//--- UIHorizontalSrollBar ---//
struct UIHorizontalScrollBar {
    UIComponent_t base;
    struct {
        int x_pos;
    } properties;

    struct {
        uint8_t dragged;
    } flags;

    struct {
        on_x_offset_function on_x_offset;
    } functions;

    float f1;
    int bar_width;
    int x_last;
};

typedef struct UIHorizontalScrollBar UIHorizontalScrollBar_t;

void ui_horizontal_scroll_bar_init(UIHorizontalScrollBar_t *self, UIScrollContainer_t *parent, int x, int y, int width, int height, int x_width_total, int x_width_visible);

UIHorizontalScrollBar_t *ui_horizontal_scroll_bar_create(UIScrollContainer_t *parent, int x, int y, int width, int height, int x_width_total, int x_width_visible);

void ui_horizontal_scroll_bar_destroy(UIHorizontalScrollBar_t *self);

int ui_horizontal_scroll_bar_paint(UIHorizontalScrollBar_t *self, Framebuffer8Bit_t *fb, int x_offset, int y_offset, int width, int height, void *usr_ptr);

void ui_horizontal_scroll_bar_update(UIHorizontalScrollBar_t *self, long time_elapsed, const Event_t *events, int num_events, struct UIApplication *app, void *usr_ptr);

//--- UIVerticalScrollBar ---//
struct UIVerticalScrollBar {
    UIComponent_t base;
    struct {
        int y_pos;
    } properties;

    struct {
        uint8_t dragged;
    } flags;

    struct {
        on_y_offset_function on_y_offset;
    } functions;

    float f1;
    int bar_height;
    int y_last;
};

typedef struct UIVerticalScrollBar UIVerticalScrollBar_t;

void ui_vertical_scroll_bar_init(UIVerticalScrollBar_t *self, UIScrollContainer_t *parent, int x, int y, int width, int height, int y_height_total, int y_height_visible);

UIVerticalScrollBar_t *ui_vertical_scroll_bar_create(UIScrollContainer_t *parent, int x, int y, int width, int height, int y_height_total, int y_height_visible);

void ui_vertical_scroll_bar_destroy(UIVerticalScrollBar_t *self);

int ui_vertical_scroll_bar_paint(UIVerticalScrollBar_t *self, Framebuffer8Bit_t *fb, int x_offset, int y_offset, int width, int height, void *usr_ptr);

void ui_vertical_scroll_bar_update(UIVerticalScrollBar_t *self, long time_elapsed, const Event_t *events, int num_events, struct UIApplication *app, void *usr_ptr);

//--- UIApplication ---//
struct UIApplication {
    Window_t *window;
    UIComponent_t root;
    Palette8Bit_t palette;
    int resume;
    void *usr_ptr;
};

typedef struct UIApplication UIApplication_t;

void ui_application_init(UIApplication_t *self, int width, int height, void *usr_ptr);

void ui_application_destroy(UIApplication_t *self);

void ui_application_prepare(UIApplication_t *self);

int ui_application_run(UIApplication_t *self, const char *title, long wait_ms);

void ui_application_quit(UIApplication_t *self);

#ifdef __cplusplus
}
#endif

#endif//EXDEVGFX_BASE_UI_H
