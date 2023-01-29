/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#include "exdevgfx/window.h"

#define EXDEVGFX2_LOG_LEVEL 2

#include "exdevgfx/logger.h"
#include "exdevgfx/exdev_base.h"
#include "exdevgfx/amiga/framebuffer_8bit_amiga.h"

#include <intuition/intuition.h>
#include <intuition/screens.h>
#include <cybergraphx/cybergraphics.h>

#include <dos/dos.h>
#include <devices/inputevent.h>
#include <devices/keymap.h>
#include <proto/console.h>

#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct NativeWindow
{
    struct Window *window;
    struct Screen *screen;
};
typedef struct NativeWindow NativeWindow_t;

Window_t *window_create(const int width, const int height, const char *title, const enum FULLSCREEN fs)
{
    NativeWindow_t *w = (NativeWindow_t *) malloc(sizeof(NativeWindow_t));

    if (fs == FS_8_BIT || fs == FS_24_BIT)
    {
        int depth = 24;
        if (fs == FS_8_BIT)
        {
            depth = 8;
        }
        const unsigned long id = BestCModeIDTags(CYBRBIDTG_Depth, depth,
                                 CYBRBIDTG_NominalWidth, width,
                                 CYBRBIDTG_NominalHeight, height,
                                 TAG_DONE);
        log_info_fmt("screen id=0x%08lx", id);
        if (id == INVALID_ID)
        {
            free(w);
            return NULL;
        }
        w->screen = OpenScreenTags(NULL,
                                   SA_Left, 0,
                                   SA_Top, 0,
                                   SA_Width, width,
                                   SA_Height, height,
                                   SA_Depth, depth,
                                   SA_Type, CUSTOMSCREEN,
                                   SA_DisplayID, id,
                                   SA_Title, title,
                                   SA_Exclusive, TRUE,
                                   SA_SharePens, TRUE,
                                   TAG_DONE);

        w->window = OpenWindowTags(NULL,
                                   WA_Left, 0,
                                   WA_Top, 0,
                                   WA_Width, width,
                                   WA_Height, height,
                                   WA_CustomScreen, w->screen,
                                   WA_IDCMP, IDCMP_CLOSEWINDOW | IDCMP_RAWKEY,
                                   WA_Flags, WFLG_ACTIVATE | WFLG_SIMPLE_REFRESH | WFLG_BORDERLESS,
                                   WA_Title, title,
                                   TAG_DONE);
    }
    else
    {
        w->screen = NULL;

        w->window = OpenWindowTags(NULL,
                                   WA_Left, 50,
                                   WA_Top, 50,
                                   WA_Width, width,
                                   WA_Height, height,
                                   WA_IDCMP, IDCMP_CLOSEWINDOW | IDCMP_RAWKEY,
                                   WA_Flags, WFLG_ACTIVATE | WFLG_SIMPLE_REFRESH | WFLG_BORDERLESS | WFLG_DRAGBAR,
                                   WA_Title, title,
                                   TAG_DONE);
    }

    //OpenDevice("console.device", -1, (struct IORequest *) &ioreq, 0);
    //ConsoleDevice = (struct Library *) ioreq.io_Device;
    return (Window_t *) w;
}

void window_destroy(Window_t *win)
{
    NativeWindow_t *w = (NativeWindow_t *) win;
    if (w->window)
    {
        CloseWindow(w->window);
    }
    if (w->screen)
    {
        CloseScreen(w->screen);
    }
    //CloseDevice((struct IORequest *) &ioreq);
    w->window = NULL;
    w->screen = NULL;

    free(w);
    win = NULL;
}

int window_get_width(const Window_t *win)
{
    NativeWindow_t *w = (NativeWindow_t *) win;
    return w->window->Width;
}

int window_get_height(const Window_t *win)
{
    NativeWindow_t *w = (NativeWindow_t *) win;
    return w->window->Height;
}

int window_get_inner_width(const Window_t *win)
{
    NativeWindow_t *w = (NativeWindow_t *) win;
    return w->window->Width - w->window->BorderLeft - w->window->BorderRight;
}

int window_get_inner_height(const Window_t *win)
{
    NativeWindow_t *w = (NativeWindow_t *) win;
    return w->window->Height - w->window->BorderTop - w->window->BorderBottom;
}

void window_fill(Window_t *win, const Framebuffer_t *gb)
{
    NativeWindow_t *w = (NativeWindow_t *) win;

    WritePixelArray(gb->buffer,
                    0, 0,
                    gb->width * 3,
                    w->window->RPort,
                    0,
                    0,
                    gb->width,
                    gb->height,
                    RECTFMT_RGB);

}

void window_fill_8bit_amiga(Window_t *win, Framebuffer8BitAmiga_t *fb, Palette8Bit_t *p, const int update_palette)
{
    NativeWindow_t *w = (NativeWindow_t *) win;
    if (update_palette)
    {
        for (int i = 0; i < p->numPens; ++i)
        {
            const Pen_t *pen = palette_8bit_get_pen(p, i);
            SetRGB32(&w->screen->ViewPort, i, pen->r, pen->g, pen->b);
        }
    }

    BltBitMapRastPort(fb->bm, 0, 0, w->window->RPort,
                      0,
                      0,
                      fb->width, fb->height,
                      (ABNC | ABC));
}

void window_update_palette(Window_t *win, const Palette8Bit_t *p)
{
    NativeWindow_t *w = (NativeWindow_t *) win;
    assert(w->screen);
    for (int i = 0; i < p->numPens; ++i)
    {
        const Pen_t *pen = palette_8bit_get_pen_const(p, i);
        SetRGB32(&w->screen->ViewPort, i, pen->r, pen->g, pen->b);
    }

}

void window_fill_8bit(Window_t *win, const Framebuffer8Bit_t *gb)
{
    NativeWindow_t *w = (NativeWindow_t *) win;
    assert(w->screen);

    WritePixelArray(gb->buffer,
                    0, 0,
                    gb->width,
                    w->window->RPort,
                    0,
                    0,
                    gb->width,
                    gb->height,
                    RECTFMT_LUT8);
}

LONG deadKeyConvert(struct IntuiMessage *msg, UBYTE *kbuffer, LONG kbsize, struct InputEvent *ievent)
{
    if (msg->Class != IDCMP_RAWKEY)
        return (-2);

    ievent->ie_Class = IECLASS_RAWKEY;
    ievent->ie_Code = msg->Code & ~IECODE_UP_PREFIX;
    ievent->ie_Qualifier = msg->Qualifier;
    ievent->ie_position.ie_addr = *((APTR *) msg->IAddress);
    return RawKeyConvert(ievent, kbuffer, kbsize, NULL);
}

#define BUFFER_SIZE 8

int window_poll_events(Window_t *win, char *closeEvent, KeyEvent_t *events, const int max)
{
    NativeWindow_t *w = (NativeWindow_t *) win;
    struct IntuiMessage *msg = NULL;
    struct InputEvent ievent;
    UBYTE buffer[BUFFER_SIZE];

    ULONG msgClass;

    int numEvents = 0;

    key_event_init(events, max);

    memset(&ievent, 0, sizeof(struct InputEvent));
    memset(buffer, 0, BUFFER_SIZE);

    //Wait(1L << w->window->UserPort->mp_SigBit);

    while (numEvents < max && (msg = GT_GetIMsg(w->window->UserPort)))
    {
        log_debug("got message");
        msgClass = msg->Class;
        switch (msgClass)
        {
        case IDCMP_CLOSEWINDOW:
            *closeEvent = TRUE;
            log_debug("window close event");
            break;
        case IDCMP_RAWKEY:
            if (!(msg->Qualifier & IEQUALIFIER_REPEAT))
            {
                events[numEvents].event = msg->Code & IECODE_UP_PREFIX ? KEY_EVENT_RELEASED : KEY_EVENT_PRESSED;
                if ((msg->Code & ~IECODE_UP_PREFIX) == 0x4C)
                {
                    events[numEvents].type = KEY_TYPE_UP;
                    log_debug("arrow up");
                    ++numEvents;
                }
                else if ((msg->Code & ~IECODE_UP_PREFIX) == 0x4D)
                {
                    events[numEvents].type = KEY_TYPE_DOWN;
                    log_debug("arrow down");
                    ++numEvents;
                }
                else if ((msg->Code & ~IECODE_UP_PREFIX) == 0x4F)
                {
                    events[numEvents].type = KEY_TYPE_LEFT;
                    log_debug("arrow left");
                    ++numEvents;
                }
                else if ((msg->Code & ~IECODE_UP_PREFIX) == 0x4E)
                {
                    events[numEvents].type = KEY_TYPE_RIGHT;
                    log_debug("arrow right");
                    ++numEvents;
                }
                else if ((msg->Code & ~IECODE_UP_PREFIX) == 0x45)
                {
                    events[numEvents].type = KEY_TYPE_ESC;
                    log_debug("esc");
                    ++numEvents;
                }
                else if ((msg->Code & ~IECODE_UP_PREFIX) == 0x50)
                {
                    events[numEvents].type = KEY_TYPE_F1;
                    log_debug("F1");
                    ++numEvents;
                }
                else if ((msg->Code & ~IECODE_UP_PREFIX) == 0x51)
                {
                    events[numEvents].type = KEY_TYPE_F2;
                    log_debug("F2");
                    ++numEvents;
                }
                else if ((msg->Code & ~IECODE_UP_PREFIX) == 0x52)
                {
                    events[numEvents].type = KEY_TYPE_F3;
                    log_debug("F3");
                    ++numEvents;
                }
                else if ((msg->Code & ~IECODE_UP_PREFIX) == 0x53)
                {
                    events[numEvents].type = KEY_TYPE_F4;
                    log_debug("F4");
                    ++numEvents;
                }
                else if ((msg->Code & ~IECODE_UP_PREFIX) == 0x54)
                {
                    events[numEvents].type = KEY_TYPE_F5;
                    log_debug("F5");
                    ++numEvents;
                }
                else if ((msg->Code & ~IECODE_UP_PREFIX) == 0x55)
                {
                    events[numEvents].type = KEY_TYPE_F6;
                    log_debug("F6");
                    ++numEvents;
                }
                else
                {
                    long int numChars = deadKeyConvert(msg, buffer, BUFFER_SIZE, &ievent);
                    log_debug_fmt("numChars=%ld", numChars);
                    if (numChars == 1)
                    {
                        events[numEvents].type = KEY_TYPE_CODE;
                        events[numEvents].code = buffer[0];
                        log_debug_fmt("char=%c", buffer[0]);
                        ++numEvents;
                    }
                }
            }
            break;
        }
        GT_ReplyIMsg(msg);
    }
    return numEvents;
}
