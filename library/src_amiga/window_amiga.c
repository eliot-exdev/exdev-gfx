/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#include "exdevgfx/window.h"

#define EXDEVGFX2_LOG_LEVEL 2

#include "exdevgfx/logger.h"
#include "exdevgfx/exdev_base.h"

#include <intuition/intuition.h>
#include <proto/intuition.h>
#include <intuition/screens.h>
//#include <cybergraphx/cybergraphics.h>
//#include <proto/cybergraphics.h>
#include <proto/graphics.h>
#include <devices/inputevent.h>
#include <devices/keymap.h>
#include <proto/console.h>
#include <proto/gadtools.h>
#include <libraries/asl.h>
#include <utility/tagitem.h>
#include <proto/asl.h>

#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct NativeWindow {
    struct Screen *screen;
    struct Window *window;
};
typedef struct NativeWindow NativeWindow_t;

#define NATIVE_WINDOW_CAST(w) ((NativeWindow_t *)w)
#define NATIVE_WINDOW_CAST_CONST(w) ((const  NativeWindow_t *)w)

Window_t *window_create(const int width, const int height, const char *title, const enum FULLSCREEN fs) {
    NativeWindow_t *w = (NativeWindow_t *) malloc(sizeof(NativeWindow_t));

    if (fs == FS_8_BIT || fs == FS_24_BIT) {
        int depth = 24;
        if (fs == FS_8_BIT) {
            depth = 8;
        }

        char TITLE_TEXT[256];
        sprintf(TITLE_TEXT, "Select screen mode (%dx%dx%d)", width, height, depth);

        struct TagItem smrtags[5];
        smrtags[0].ti_Tag = ASLSM_TitleText;
        smrtags[0].ti_Data = (ULONG) TITLE_TEXT;
        smrtags[1].ti_Tag = ASLSM_MinWidth;
        smrtags[1].ti_Data = width;
        smrtags[2].ti_Tag = ASLSM_MinHeight;
        smrtags[2].ti_Data = height;
        smrtags[3].ti_Tag = ASLSM_MinDepth;
        smrtags[3].ti_Data = depth;
        smrtags[4].ti_Tag = TAG_END;

        struct ScreenModeRequester *smr = 0;
        unsigned long screen_id = (unsigned long) INVALID_ID;
        int screen_width = 0;
        int screen_height = 0;
        int screen_depth = 0;
        smr = (struct ScreenModeRequester *) AllocAslRequest(ASL_ScreenModeRequest, smrtags);
        if (AslRequest(smr, 0L)) {
            screen_id = smr->sm_DisplayID;
            screen_width = smr->sm_DisplayWidth;
            screen_height = smr->sm_DisplayHeight;
            screen_depth = smr->sm_DisplayDepth;
        } else {
            log_warning("no screen mode selected by user");
            FreeAslRequest(smr);
            free(w);
            return NULL;
        }

        FreeAslRequest(smr);

//        const unsigned long id = BestModeID(BIDTAG_Depth, depth, BIDTAG_DesiredWidth, width, BIDTAG_DesiredHeight, height, TAG_DONE);
        if (screen_id == (unsigned long) INVALID_ID) {
            free(w);
            log_warning("invalid screen id");
            return NULL;
        }
        log_info_fmt("screen_id=0x%08lx", screen_id);
        log_info_fmt("screen_width=%d", screen_width);
        log_info_fmt("screen_height=%d", screen_height);
        log_info_fmt("screen_depth=%d", screen_depth);

        if (screen_width < width) {
            free(w);
            log_warning("screen width is to small");
            return NULL;
        }
        if (screen_height < height) {
            free(w);
            log_warning("screen height is to small");
            return NULL;
        }
        if (screen_depth < depth) {
            free(w);
            log_warning("screen depth is to small");
            return NULL;
        }

        w->screen = OpenScreenTags(NULL, SA_Left, 0, SA_Top, 0, SA_Width, screen_width, SA_Height, screen_height, SA_Depth, screen_depth,
                                   SA_Type, CUSTOMSCREEN, SA_DisplayID, screen_id,
                                   SA_Title, title, SA_Exclusive, TRUE, SA_SharePens, TRUE, SA_ShowTitle, FALSE, TAG_DONE);

        w->window = OpenWindowTags(NULL, WA_Left, 0, WA_Top, 0, WA_Width, width, WA_Height, height,
                                   WA_CustomScreen, w->screen,
                                   WA_IDCMP, IDCMP_CLOSEWINDOW | IDCMP_RAWKEY | IDCMP_MOUSEBUTTONS | IDCMP_MOUSEMOVE,
                                   WA_Flags, WFLG_ACTIVATE | WFLG_SIMPLE_REFRESH | WFLG_BORDERLESS | WFLG_REPORTMOUSE | WFLG_RMBTRAP,
                                   WA_Title, title,
                                   TAG_DONE);
    } else {
        w->screen = NULL;

        w->window = OpenWindowTags(NULL, WA_Left, 50, WA_Top, 50, WA_Width, width, WA_Height, height,
                                   WA_IDCMP, IDCMP_CLOSEWINDOW | IDCMP_RAWKEY | IDCMP_MOUSEBUTTONS | IDCMP_MOUSEMOVE,
                                   WA_Flags, WFLG_ACTIVATE | WFLG_SIMPLE_REFRESH | WFLG_BORDERLESS | WFLG_DRAGBAR | WFLG_REPORTMOUSE | WFLG_RMBTRAP,
                                   WA_Title, title,
                                   TAG_DONE);
    }

    return (Window_t *) w;
}

void window_destroy(Window_t *win) {
    if (NATIVE_WINDOW_CAST(win)->window) {
        CloseWindow(NATIVE_WINDOW_CAST(win)->window);
    }
    if (NATIVE_WINDOW_CAST(win)->screen) {
        CloseScreen(NATIVE_WINDOW_CAST(win)->screen);
    }
    NATIVE_WINDOW_CAST(win)->window = NULL;
    NATIVE_WINDOW_CAST(win)->screen = NULL;

    free(NATIVE_WINDOW_CAST(win));
}

int window_get_width(const Window_t *win) {
    return NATIVE_WINDOW_CAST_CONST(win)->window->Width;
}

int window_get_height(const Window_t *win) {
    return NATIVE_WINDOW_CAST_CONST(win)->window->Height;
}

int window_get_inner_width(const Window_t *win) {
    return NATIVE_WINDOW_CAST_CONST(win)->window->Width - NATIVE_WINDOW_CAST_CONST(win)->window->BorderLeft - NATIVE_WINDOW_CAST_CONST(win)->window->BorderRight;
}

int window_get_inner_height(const Window_t *win) {
    return NATIVE_WINDOW_CAST_CONST(win)->window->Height - NATIVE_WINDOW_CAST_CONST(win)->window->BorderTop - NATIVE_WINDOW_CAST_CONST(win)->window->BorderBottom;
}

void window_fill(Window_t *win, const Framebuffer_t *gb) {
    //    WritePixelArray(gb->buffer, 0, 0, gb->width * 3, w->window->RPort, 0, 0, gb->width, gb->height, RECTFMT_RGB);
    WriteChunkyPixels(NATIVE_WINDOW_CAST(win)->window->RPort, 0, 0, gb->width * 3, gb->height * 3, (unsigned char *) gb->buffer, gb->width);
}

void window_fill_8bit(Window_t *win, const Framebuffer8Bit_t *gb) {
    assert(w->screen);
    //    WritePixelArray(gb->buffer, 0, 0, gb->width, w->window->RPort, 0, 0, gb->width, gb->height, RECTFMT_LUT8);
    WriteChunkyPixels(NATIVE_WINDOW_CAST(win)->window->RPort, 0, 0, gb->width, gb->height, gb->buffer, gb->width);
}

void window_update_palette(Window_t *win, const Palette8Bit_t *p) {
    assert(w->screen);
    for (int i = 0; i < p->numPens; ++i) {
        const Pen_t *pen = palette_8bit_get_pen_const(p, i);
        SetRGB32(&NATIVE_WINDOW_CAST(win)->screen->ViewPort, i, pen->r, pen->g, pen->b);
    }
}

#define KEY_BUFFER_SIZE 8

static LONG deadKeyConvert(const struct IntuiMessage *msg, char *kbuffer, struct InputEvent *ievent) {
    if (msg->Class != IDCMP_RAWKEY)
        return (-2);

    ievent->ie_Class = IECLASS_RAWKEY;
    ievent->ie_Code = msg->Code & ~IECODE_UP_PREFIX;
    ievent->ie_Qualifier = msg->Qualifier;
    ievent->ie_position.ie_addr = *((APTR *) msg->IAddress);
    return RawKeyConvert(ievent, kbuffer, KEY_BUFFER_SIZE, NULL);
}

int window_poll_events(Window_t *win, char *closeEvent, Event_t *events, const int maxEvents) {
    struct IntuiMessage *msg = NULL;// since V39 it should be struct ExtIntuiMessage *
    struct InputEvent ievent;
    char buffer[KEY_BUFFER_SIZE];

    int numEvents = 0;
    event_init(events, maxEvents);

    memset(&ievent, 0, sizeof(struct InputEvent));

    //    Wait(1L << w->window->UserPort->mp_SigBit);

    while (numEvents < maxEvents && (msg = GT_GetIMsg(NATIVE_WINDOW_CAST(win)->window->UserPort))) {
        log_debug("--> got message");
        switch (msg->Class) {
            case IDCMP_CLOSEWINDOW:
                *closeEvent = TRUE;
                log_debug("window close event");
                break;
            case IDCMP_RAWKEY:
                log_debug("key event");
                events[numEvents].type = EVENT_KEY;
                if (!(msg->Qualifier & IEQUALIFIER_REPEAT)) {
                    events[numEvents].key_event.event = msg->Code & IECODE_UP_PREFIX ? KEY_EVENT_RELEASED : KEY_EVENT_PRESSED;
                    if ((msg->Code & ~IECODE_UP_PREFIX) == 0x4C) {
                        events[numEvents].key_event.key = KEY_TYPE_UP;
                        log_debug("arrow up");
                    } else if ((msg->Code & ~IECODE_UP_PREFIX) == 0x4D) {
                        events[numEvents].key_event.key = KEY_TYPE_DOWN;
                        log_debug("arrow down");
                    } else if ((msg->Code & ~IECODE_UP_PREFIX) == 0x4F) {
                        events[numEvents].key_event.key = KEY_TYPE_LEFT;
                        log_debug("arrow left");
                    } else if ((msg->Code & ~IECODE_UP_PREFIX) == 0x4E) {
                        events[numEvents].key_event.key = KEY_TYPE_RIGHT;
                        log_debug("arrow right");
                    } else if ((msg->Code & ~IECODE_UP_PREFIX) == 0x45) {
                        events[numEvents].key_event.key = KEY_TYPE_ESC;
                        log_debug("esc");
                    } else if ((msg->Code & ~IECODE_UP_PREFIX) == 0x50) {
                        events[numEvents].key_event.key = KEY_TYPE_F1;
                        log_debug("F1");
                    } else if ((msg->Code & ~IECODE_UP_PREFIX) == 0x51) {
                        events[numEvents].key_event.key = KEY_TYPE_F2;
                        log_debug("F2");
                    } else if ((msg->Code & ~IECODE_UP_PREFIX) == 0x52) {
                        events[numEvents].key_event.key = KEY_TYPE_F3;
                        log_debug("F3");
                    } else if ((msg->Code & ~IECODE_UP_PREFIX) == 0x53) {
                        events[numEvents].key_event.key = KEY_TYPE_F4;
                        log_debug("F4");
                    } else if ((msg->Code & ~IECODE_UP_PREFIX) == 0x54) {
                        events[numEvents].key_event.key = KEY_TYPE_F5;
                        log_debug("F5");
                    } else if ((msg->Code & ~IECODE_UP_PREFIX) == 0x55) {
                        events[numEvents].key_event.key = KEY_TYPE_F6;
                        log_debug("F6");
                    } else if ((msg->Code & ~IECODE_UP_PREFIX) == 0x56) {
                        events[numEvents].key_event.key = KEY_TYPE_F7;
                        log_debug("F7");
                    } else if ((msg->Code & ~IECODE_UP_PREFIX) == 0x57) {
                        events[numEvents].key_event.key = KEY_TYPE_F8;
                        log_debug("F8");
                    } else if ((msg->Code & ~IECODE_UP_PREFIX) == 0x58) {
                        events[numEvents].key_event.key = KEY_TYPE_F9;
                        log_debug("F9");
                    } else if ((msg->Code & ~IECODE_UP_PREFIX) == 0x59) {
                        events[numEvents].key_event.key = KEY_TYPE_F10;
                        log_debug("F10");
                    } else {
                        memset(buffer, 0, KEY_BUFFER_SIZE);
                        const long int numChars = deadKeyConvert(msg, buffer, &ievent);
                        log_debug_fmt("numChars=%ld", numChars);
                        if (numChars == 1) {
                            events[numEvents].key_event.key = KEY_TYPE_CODE;
                            events[numEvents].key_event.code = buffer[0];
                            log_debug_fmt("char=%c", buffer[0]);
                        }
                    }
                }
                ++numEvents;
                break;
            case IDCMP_MOUSEBUTTONS:
                log_debug("mouse event");
                switch (msg->Code) {
                    case SELECTDOWN:
                        events[numEvents].mouse_event.event = MOUSE_EVENT_BUTTON_PRESSED;
                        events[numEvents].mouse_event.button = MOUSE_BUTTON_0;
                        break;
                    case SELECTUP:
                        events[numEvents].mouse_event.event = MOUSE_EVENT_BUTTON_RELEASED;
                        events[numEvents].mouse_event.button = MOUSE_BUTTON_0;
                        break;
                    case MENUDOWN:
                        events[numEvents].mouse_event.event = MOUSE_EVENT_BUTTON_PRESSED;
                        events[numEvents].mouse_event.button = MOUSE_BUTTON_1;
                        break;
                    case MENUUP:
                        events[numEvents].mouse_event.event = MOUSE_EVENT_BUTTON_RELEASED;
                        events[numEvents].mouse_event.button = MOUSE_BUTTON_1;
                        break;
                    case MIDDLEDOWN:
                        events[numEvents].mouse_event.event = MOUSE_EVENT_BUTTON_PRESSED;
                        events[numEvents].mouse_event.button = MOUSE_BUTTON_2;
                        break;
                    case MIDDLEUP:
                        events[numEvents].mouse_event.event = MOUSE_EVENT_BUTTON_RELEASED;
                        events[numEvents].mouse_event.button = MOUSE_BUTTON_2;
                        break;
                    default:
                        events[numEvents].mouse_event.event = MOUSE_EVENT_INVALID;
                        events[numEvents].mouse_event.button = MOUSE_BUTTON_NONE;
                }
                events[numEvents].type = EVENT_MOUSE;
                events[numEvents].mouse_event.position_x = msg->MouseX;
                events[numEvents].mouse_event.position_y = msg->MouseY;
                log_debug_fmt("mouse event %d %d %d %d",
                              events[numEvents].mouse_event.event,
                              events[numEvents].mouse_event.button,
                              events[numEvents].mouse_event.position_x,
                              events[numEvents].mouse_event.position_y);
                ++numEvents;
                break;
            case IDCMP_MOUSEMOVE:
                events[numEvents].type = EVENT_MOUSE;
                events[numEvents].mouse_event.event = MOUSE_EVENT_MOVED;
                events[numEvents].mouse_event.button = MOUSE_BUTTON_NONE;
                events[numEvents].mouse_event.position_x = msg->MouseX;
                events[numEvents].mouse_event.position_y = msg->MouseY;
                ++numEvents;
                break;
        }
        GT_ReplyIMsg(msg);
    }
    return numEvents;
}
