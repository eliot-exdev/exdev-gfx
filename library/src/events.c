/**
 * Copyright 2023 Andre Geisler (andre@exdev.de)
 */

#include <exdevgfx/events.h>

void key_event_init(KeyEvent_t *events, const int num) {
    for (int i = 0; i < num; ++i) {
        events[i].event = KEY_EVENT_INVALID;
        events[i].type = KEY_TYPE_INVALID;
        events[i].code = 0;
    }
}

void mouse_event_init(MouseEvent_t *events, const int num) {
    for (int i = 0; i < num; ++i) {
        events[i].event = MOUSE_EVENT_INVALID;
        events[i].button = MOUSE_BUTTON_NONE;
        events[i].position_x = 0;
        events[i].position_y = 0;
    }
}