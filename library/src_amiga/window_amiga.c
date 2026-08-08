/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#include "exdevgfx/window.h"

#define EXDEVGFX2_LOG_LEVEL 2

#include "exdevgfx/logger.h"
#include "exdevgfx/exdev_base.h"
#include "exdevgfx/helper.h"

#include <intuition/intuition.h>
#include <proto/intuition.h>
#include <intuition/screens.h>

#include <proto/graphics.h>
#include <devices/inputevent.h>
#include <proto/console.h>
#include <proto/gadtools.h>
#include <libraries/asl.h>
#include <utility/tagitem.h>
#include <proto/asl.h>

#ifdef USE_C2P
#include <c2p.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct NativeWindow {
    struct Screen *screen;
    struct Window *window;
    Framebuffer8Bit_t chunky_buffer;
#ifdef USE_C2P
    APTR C2P_context;
#endif
};

typedef struct NativeWindow NativeWindow_t;

#define NATIVE_WINDOW_CAST(w) ((NativeWindow_t *) w)
#define NATIVE_WINDOW_CAST_CONST(w) ((const NativeWindow_t *) w)


#ifdef USE_C2P
Window_t *window_create(const int width, const int height, const char *title, const enum FULLSCREEN fs) {
    NativeWindow_t *w = malloc(sizeof(NativeWindow_t));

    const int depth = (int) FS_8_BIT; // we always use 256 colors, no window mode!

    char TITLE_TEXT[128];
    memset(TITLE_TEXT, 0, 128);
    sprintf(TITLE_TEXT, "Select screen mode (%dx%dx%d)", width, height, depth);

    struct TagItem smrtags[8];
    smrtags[0].ti_Tag = ASLSM_TitleText;
    smrtags[0].ti_Data = (ULONG) TITLE_TEXT;

    smrtags[1].ti_Tag = ASLSM_MinWidth;
    smrtags[1].ti_Data = width;

    smrtags[2].ti_Tag = ASLSM_MinHeight;
    smrtags[2].ti_Data = height;

    smrtags[3].ti_Tag = ASLSM_MinDepth;
    smrtags[3].ti_Data = depth;

    smrtags[4].ti_Tag = ASLSM_InitialDisplayWidth;
    smrtags[4].ti_Data = width;

    smrtags[5].ti_Tag = ASLSM_InitialDisplayHeight;
    smrtags[5].ti_Data = height;

    smrtags[6].ti_Tag = ASLSM_InitialDisplayDepth;
    smrtags[6].ti_Data = depth;

    smrtags[7].ti_Tag = TAG_DONE;

    unsigned long screen_id = (unsigned long) INVALID_ID;
    int screen_width = 0;
    int screen_height = 0;
    int screen_depth = 0;
    struct ScreenModeRequester *smr = (struct ScreenModeRequester *) AllocAslRequest(ASL_ScreenModeRequest, smrtags);
    if (AslRequest(smr, 0L)) {
        screen_id = smr->sm_DisplayID;
        screen_width = (int) smr->sm_DisplayWidth;
        screen_height = (int) smr->sm_DisplayHeight;
        screen_depth = smr->sm_DisplayDepth;
    } else {
        log_warning("no screen mode selected by user");
        FreeAslRequest(smr);
        free(w);
        return NULL;
    }

    FreeAslRequest(smr);

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

    w->C2P_context = C2P_CreateContext();
    C2P_SetContextParameter(w->C2P_context, C2P_CONTEXT_PARAMETER_TYPE, C2P_CONTEXT_TYPE_BITMAP);
    C2P_SetContextParameter(w->C2P_context, C2P_CONTEXT_PARAMETER_WIDTH, width);
    C2P_SetContextParameter(w->C2P_context, C2P_CONTEXT_PARAMETER_HEIGHT, height);
    C2P_SetContextParameter(w->C2P_context, C2P_CONTEXT_PARAMETER_PLANAR_FORMAT, C2P_CONTEXT_PLANAR_FORMAT_8_BIT);
    C2P_InitializeContext(w->C2P_context);
    w->chunky_buffer.buffer = (Color8Bit_t *) C2P_GetContextParameter(w->C2P_context, C2P_CONTEXT_PARAMETER_CHUNKY);
    w->chunky_buffer.width = width;
    w->chunky_buffer.height = height;
    APTR bmp = (APTR) C2P_GetContextParameter(w->C2P_context, C2P_CONTEXT_PARAMETER_BITMAP);

    w->screen = OpenScreenTags(NULL,
                               SA_Left, 0,
                               SA_Top, 0,
                               SA_Width, screen_width,
                               SA_Height, screen_height,
                               SA_Depth, screen_depth,
                               SA_Type, CUSTOMSCREEN | CUSTOMBITMAP,
                               SA_BitMap, bmp,
                               SA_DisplayID, screen_id,
                               SA_Title, title,
                               SA_Exclusive, TRUE,
                               SA_SharePens, TRUE,
                               SA_ShowTitle, FALSE,
                               SA_AutoScroll, FALSE,
                               SA_Draggable, FALSE,
                               TAG_DONE);

    w->window = OpenWindowTags(NULL,
                               WA_Left, 0,
                               WA_Top, 0,
                               WA_Width, width,
                               WA_Height, height,
                               WA_ScreenTitle, title,
                               WA_CustomScreen, w->screen,
                               WA_IDCMP, IDCMP_CLOSEWINDOW | IDCMP_RAWKEY | IDCMP_MOUSEBUTTONS | IDCMP_MOUSEMOVE,
                               WA_Flags, WFLG_ACTIVATE | WFLG_SIMPLE_REFRESH | WFLG_BORDERLESS | WFLG_REPORTMOUSE | WFLG_RMBTRAP | WFLG_BACKDROP,
                               WA_Title, title,
                               TAG_DONE);

    return (Window_t *) w;
}
#else
Window_t *window_create(const int width, const int height, const char *title, const enum FULLSCREEN fs) {
    NativeWindow_t *w = malloc(sizeof(NativeWindow_t));

    const int depth = (int) fs;

    if (fs != FS_DISABLED) {
        char TITLE_TEXT[128] = {0};
        sprintf(TITLE_TEXT, "Select screen mode (%dx%dx%d)", width, height, depth);

        struct TagItem smrtags[8];
        smrtags[0].ti_Tag = ASLSM_TitleText;
        smrtags[0].ti_Data = (ULONG) TITLE_TEXT;

        smrtags[1].ti_Tag = ASLSM_MinWidth;
        smrtags[1].ti_Data = width;

        smrtags[2].ti_Tag = ASLSM_MinHeight;
        smrtags[2].ti_Data = height;

        smrtags[3].ti_Tag = ASLSM_MinDepth;
        smrtags[3].ti_Data = depth;

        smrtags[4].ti_Tag = ASLSM_InitialDisplayWidth;
        smrtags[4].ti_Data = width;

        smrtags[5].ti_Tag = ASLSM_InitialDisplayHeight;
        smrtags[5].ti_Data = height;

        smrtags[6].ti_Tag = ASLSM_InitialDisplayDepth;
        smrtags[6].ti_Data = depth;

        smrtags[7].ti_Tag = TAG_DONE;

        unsigned long screen_id = (unsigned long) INVALID_ID;
        int screen_width = 0;
        int screen_height = 0;
        int screen_depth = 0;
        struct ScreenModeRequester *smr = (struct ScreenModeRequester *) AllocAslRequest(ASL_ScreenModeRequest, smrtags);
        if (AslRequest(smr, 0L)) {
            screen_id = smr->sm_DisplayID;
            screen_width = (int) smr->sm_DisplayWidth;
            screen_height = (int) smr->sm_DisplayHeight;
            screen_depth = smr->sm_DisplayDepth;
        } else {
            log_warning("no screen mode selected by user");
            FreeAslRequest(smr);
            free(w);
            return NULL;
        }

        FreeAslRequest(smr);

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

        w->screen = OpenScreenTags(NULL,
                                   SA_Left, 0,
                                   SA_Top, 0,
                                   SA_Width, screen_width,
                                   SA_Height, screen_height,
                                   SA_Depth, screen_depth,
                                   SA_Type, CUSTOMSCREEN,
                                   SA_DisplayID, screen_id,
                                   SA_Title, title,
                                   SA_Exclusive, TRUE,
                                   SA_SharePens, TRUE,
                                   SA_ShowTitle, FALSE,
                                   SA_AutoScroll, FALSE,
                                   SA_Draggable, FALSE,
                                   TAG_DONE);

        w->window = OpenWindowTags(NULL,
                                   WA_Left, 0,
                                   WA_Top, 0,
                                   WA_Width, width,
                                   WA_Height, height,
                                   WA_ScreenTitle, title,
                                   WA_CustomScreen, w->screen,
                                   WA_IDCMP, IDCMP_CLOSEWINDOW | IDCMP_RAWKEY | IDCMP_MOUSEBUTTONS | IDCMP_MOUSEMOVE,
                                   WA_Flags, WFLG_ACTIVATE | WFLG_SIMPLE_REFRESH | WFLG_BORDERLESS | WFLG_REPORTMOUSE | WFLG_RMBTRAP | WFLG_BACKDROP,
                                   WA_Title, title,
                                   TAG_DONE);
    } else {
        log_info("window mode");
        w->screen = NULL;
        w->window = OpenWindowTags(NULL,
                                   WA_Left, 30,
                                   WA_Top, 30,
                                   WA_Width, width,
                                   WA_Height, height,
                                   WA_PubScreen, NULL,
                                   WA_ScreenTitle, title,
                                   WA_IDCMP, IDCMP_CLOSEWINDOW | IDCMP_RAWKEY | IDCMP_MOUSEBUTTONS | IDCMP_MOUSEMOVE,
                                   WA_Flags, WFLG_ACTIVATE | WFLG_SIMPLE_REFRESH | WFLG_REPORTMOUSE | WFLG_RMBTRAP,
                                   WA_DragBar, TRUE,
                                   WA_Title, title,
                                   TAG_DONE);
        WindowToFront(w->window);
    }

    framebuffer_8bit_init(&w->chunky_buffer, width, height);

    return (Window_t *) w;
}
#endif

void window_destroy(Window_t *win) {
    NativeWindow_t *nwin = NATIVE_WINDOW_CAST(win);

    if (nwin->window) {
        CloseWindow(nwin->window);
    }
    if (nwin->screen) {
        CloseScreen(nwin->screen);
    }

#ifdef USE_C2P
    C2P_DestroyContext(nwin->C2P_context);
    nwin->chunky_buffer.buffer = NULL;
#else
    framebuffer_8bit_deinit(&nwin->chunky_buffer);
#endif

    nwin->window = NULL;
    nwin->screen = NULL;

    free(nwin);
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
    //#ifdef __MORPHOS__
    //    WritePixelArray(gb->buffer, 0, 0, gb->width * 3, &NATIVE_WINDOW_CAST(win)->screen->RastPort, 0, 0, gb->width, gb->height, RECTFMT_RGB);
    //#else
    WriteChunkyPixels(NATIVE_WINDOW_CAST(win)->window->RPort, 0, 0,
                      gb->width,
                      gb->height,
                      (unsigned char *) gb->buffer,
                      gb->width * 3);
    //#endif
}

Framebuffer8Bit_t *window_get_chunky_buffer(Window_t *win) {
    return &NATIVE_WINDOW_CAST(win)->chunky_buffer;
}

void window_blit_chunky_buffer(Window_t *win) {
    //#ifdef LOW_RESOLUTION
    //    WaitTOF();
    //#endif

#ifdef USE_C2P
    C2P_Chunky2Planar(NATIVE_WINDOW_CAST(win)->C2P_context);
#else
    WriteChunkyPixels(NATIVE_WINDOW_CAST(win)->window->RPort, 0, 0,
                      NATIVE_WINDOW_CAST(win)->chunky_buffer.width,
                      NATIVE_WINDOW_CAST(win)->chunky_buffer.height,
                      NATIVE_WINDOW_CAST(win)->chunky_buffer.buffer,
                      NATIVE_WINDOW_CAST(win)->chunky_buffer.width);
#endif
}

void window_fill_8bit(Window_t *win, const Framebuffer8Bit_t *gb) {
    WriteChunkyPixels(NATIVE_WINDOW_CAST(win)->window->RPort, 0, 0,
                      gb->width,
                      gb->height,
                      gb->buffer,
                      gb->width);
}

void window_update_palette(Window_t *win, const Palette8Bit_t *p) {
    assert(w->window);
    for (int i = 0; i < p->numPens; ++i) {
        const Pen_t *pen = palette_8bit_get_pen_const(p, i);
        SetRGB32(&NATIVE_WINDOW_CAST(win)->window->WScreen->ViewPort, i, pen->r, pen->g, pen->b);
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
    struct IntuiMessage *msg = NULL; // since V39 it should be struct ExtIntuiMessage *
    struct InputEvent ievent;
    char buffer[KEY_BUFFER_SIZE];

    int numEvents = 0;
    event_init(events, maxEvents);
    Event_t *mouse_motion = NULL;
    memset(&ievent, 0, sizeof(struct InputEvent));

    //    Wait(1L << w->window->UserPort->mp_SigBit);

    while (numEvents < maxEvents && (msg = GT_GetIMsg(NATIVE_WINDOW_CAST(win)->window->UserPort))) {
        log_debug("--> got message");
        switch (msg->Class) {
            case IDCMP_CLOSEWINDOW:
                *closeEvent = TRUE;
                log_debug("window close event");
                break;
            case IDCMP_REFRESHWINDOW:
                BeginRefresh((NATIVE_WINDOW_CAST(win)->window));
                EndRefresh((NATIVE_WINDOW_CAST(win)->window), TRUE);
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
                if (!mouse_motion) {
                    events[numEvents].type = EVENT_MOUSE;
                    events[numEvents].mouse_event.event = MOUSE_EVENT_MOVED;
                    events[numEvents].mouse_event.button = MOUSE_BUTTON_NONE;
                    events[numEvents].mouse_event.position_x = msg->MouseX;
                    events[numEvents].mouse_event.position_y = msg->MouseY;
                    mouse_motion = events + numEvents;
                    ++numEvents;
                } else {
                    mouse_motion->mouse_event.position_x = msg->MouseX;
                    mouse_motion->mouse_event.position_y = msg->MouseY;
                }
                break;
        }
        GT_ReplyIMsg(msg);
    }
    return numEvents;
}

int window_get_mouse_position(Window_t *w, Event_t *event) {
    assert(w);
    assert(event);

    const int x = NATIVE_WINDOW_CAST(w)->window->WScreen->MouseX;
    const int y = NATIVE_WINDOW_CAST(w)->window->WScreen->MouseY;
    const int w_x = NATIVE_WINDOW_CAST(w)->window->LeftEdge;
    const int w_y = NATIVE_WINDOW_CAST(w)->window->TopEdge;
    const int w_width = NATIVE_WINDOW_CAST(w)->window->Width;
    const int w_height = NATIVE_WINDOW_CAST(w)->window->Height;

    if (x >= w_x && y >= w_y && x < w_width && y < w_height) {
        event->type = EVENT_MOUSE;
        event->mouse_event.event = MOUSE_EVENT_MOVED;
        event->mouse_event.position_x = x + w_x;
        event->mouse_event.position_y = y + w_y;
        return 1;
    }
    return 0;
}
