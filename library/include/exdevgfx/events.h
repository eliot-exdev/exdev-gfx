/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#ifndef EXDEVGFX_BASE_EVENTS_H
#define EXDEVGFX_BASE_EVENTS_H

enum key_event {
    KEY_EVENT_INVALID,
    KEY_EVENT_PRESSED,
    KEY_EVENT_RELEASED
};

enum key_type {
    KEY_TYPE_INVALID,
    KEY_TYPE_CODE,
    KEY_TYPE_UP,
    KEY_TYPE_DOWN,
    KEY_TYPE_LEFT,
    KEY_TYPE_RIGHT,
    KEY_TYPE_ESC,
    KEY_TYPE_F1,
    KEY_TYPE_F2,
    KEY_TYPE_F3,
    KEY_TYPE_F4,
    KEY_TYPE_F5,
    KEY_TYPE_F6
};

struct KeyEvent {
    enum key_event event;
    enum key_type key;
    unsigned char code;
};

typedef struct KeyEvent KeyEvent_t;

enum mouse_event {
    MOUSE_EVENT_INVALID,
    MOUSE_EVENT_BUTTON_PRESSED,
    MOUSE_EVENT_BUTTON_RELEASED,
    MOUSE_EVENT_MOVED
};

enum mouse_button_type {
    MOUSE_BUTTON_NONE,
    MOUSE_BUTTON_0,
    MOUSE_BUTTON_1
};

struct MouseEvent {
    enum mouse_event event;
    enum mouse_button_type button;
    int position_x;
    int position_y;
};

typedef struct MouseEvent MouseEvent_t;

enum event_type{
    EVENT_INVALID,
    EVENT_KEY,
    EVENT_MOUSE
};

struct Event{
    enum event_type type;
    KeyEvent_t key_event;
    MouseEvent_t mouse_event;
};

typedef struct Event Event_t;

void key_event_init(KeyEvent_t *events, int num);

void mouse_event_init(MouseEvent_t *events, int num);

void event_init(Event_t *events, int num);

#endif //EXDEVGFX_BASE_EVENTS_H
