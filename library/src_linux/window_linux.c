/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#include "exdevgfx/window.h"
#include "exdevgfx/framebuffer_rgba.h"

#define EXDEVGFX2_LOG_LEVEL 1

#include "exdevgfx/logger.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>

#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct X11Window {
    Display *display;
    int screen;
    Window window;
    XEvent event;
    Visual *visual;
    Atom delWindow;
    XImage *img;
    FramebufferRGBA_t fb;
    const Palette8Bit_t *palette;
};

typedef struct X11Window X11Window_t;

//static void setFullscreen(Display *dpy, Window win) {
//    Atom atoms[2] = {XInternAtom(dpy, "_NET_WM_STATE_FULLSCREEN", False), None};
//    XChangeProperty(
//            dpy,
//            win,
//            XInternAtom(dpy, "_NET_WM_STATE", False),
//            XA_ATOM, 32, PropModeReplace, atoms, 1
//    );
//}

//static void disableDecorytions(Display *display, Window window){
//    MWMHints mwmhints;
//    Atom prop;
//    memset(&mwmhints, 0, sizeof(mwmhints));
//    prop = XInternAtom(display, "_MOTIF_WM_HINTS", False);
//    mwmhints.flags = MWM_HINTS_DECORATIONS;
//    mwmhints.decorations = 0;
//    XChangeProperty(display, window, prop, prop, 32,
//                    PropModeReplace, (unsigned char *) &mwmhints,
//                    PROP_MWM_HINTS_ELEMENTS);
//}

typedef struct hints {
    unsigned long flags;
    unsigned long functions;
    unsigned long decorations;
    long inputMode;
    unsigned long status;
} hints_t;


Window_t *window_create(const int width, const int height, const char *title, const enum FULLSCREEN fs) {
    X11Window_t *x11_w = (X11Window_t *) malloc(sizeof(X11Window_t));
    x11_w->display = XOpenDisplay(NULL);
    if (x11_w->display == NULL) {
        log_warning("[ERROR] XOpenDisplay failed!");
        return NULL;
    }
    x11_w->screen = DefaultScreen(x11_w->display);
    x11_w->visual = DefaultVisual(x11_w->display, x11_w->screen);

    x11_w->window = XCreateSimpleWindow(x11_w->display, RootWindow(x11_w->display, x11_w->screen), 0, 0, width,
                                        height, 0,
                                        BlackPixel(x11_w->display, x11_w->screen),
                                        WhitePixel(x11_w->display, x11_w->screen));
    XStoreName(x11_w->display, x11_w->window, title);

    x11_w->img = NULL;
    x11_w->fb.buffer = NULL;
    x11_w->palette = NULL;

    x11_w->delWindow = XInternAtom(x11_w->display, "WM_DELETE_WINDOW", 0);
    XSetWMProtocols(x11_w->display, x11_w->window, &x11_w->delWindow, 1);

    XSelectInput(x11_w->display, x11_w->window, ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask);

    XMapWindow(x11_w->display, x11_w->window);
    XAutoRepeatOff(x11_w->display);

    // disable window decorations
#ifndef __APPLE__
    const hints_t hints = {2, 0, 0, 0, 0};
    Atom property = XInternAtom(x11_w->display, "_MOTIF_WM_HINTS", True);
    XChangeProperty(x11_w->display, x11_w->window, property, property, 32, PropModeReplace, (unsigned char *) &hints, 5);
#endif

    if (fs != FS_NO) {
        log_warning("full screen is currently not supported on linux");
//        setFullscreen(x11_w->display,x11_w->window);
    }

    return (void *) x11_w;
}

void window_destroy(Window_t *w) {
    X11Window_t *x11_w = (X11Window_t *) w;
    XAutoRepeatOn(x11_w->display);
    XDestroyImage(x11_w->img);
    x11_w->img = NULL;
    XDestroyWindow(x11_w->display, x11_w->window);
    XCloseDisplay(x11_w->display);
//    framebuffer_rgba_deinit(&x11_w->fb); // is not needed, because XDestroyImage clears the buffer
    free(x11_w);
}

int window_get_width(const Window_t *w) {
    X11Window_t *x11_w = (X11Window_t *) w;
    void *ptr = NULL;
    int x = 0, y = 0;
    unsigned int width = 0, height = 0, border = 0, depth = 0;
    XGetGeometry(x11_w->display, x11_w->window, ptr, &x, &y, &width, &height, &border, &depth);
    return (int) width;
}

int window_get_height(const Window_t *w) {
    X11Window_t *x11_w = (X11Window_t *) w;
    void *ptr = NULL;
    int x = 0, y = 0;
    unsigned int width = 0, height = 0, border = 0, depth = 0;
    XGetGeometry(x11_w->display, x11_w->window, ptr, &x, &y, &width, &height, &border, &depth);
    return (int) height;
}

int window_get_inner_width(const Window_t *w) {
    X11Window_t *x11_w = (X11Window_t *) w;
    void *ptr = NULL;
    int x = 0, y = 0;
    unsigned int width = 0, height = 0, border = 0, depth = 0;
    XGetGeometry(x11_w->display, x11_w->window, ptr, &x, &y, &width, &height, &border, &depth);
    return (int) width;
}

int window_get_inner_height(const Window_t *w) {
    X11Window_t *x11_w = (X11Window_t *) w;
    void *ptr = NULL;
    int x = 0, y = 0;
    unsigned int width = 0, height = 0, border = 0, depth = 0;
    XGetGeometry(x11_w->display, x11_w->window, ptr, &x, &y, &width, &height, &border, &depth);
    return (int) height;
}

void window_fill(Window_t *w, const Framebuffer_t *gb) {
    X11Window_t *x11_w = (X11Window_t *) w;

    if (!x11_w->fb.buffer) {
        framebuffer_rgba_init(&x11_w->fb, gb->width, gb->height);
    }
    if (x11_w->fb.width != gb->width || x11_w->fb.height != gb->height) {
        log_warning("width or height differ");
        return;
    }
    framebuffer_rgba_fill(&x11_w->fb, gb, 0xff);
    framebuffer_rgba_swap(&x11_w->fb);

    if (!x11_w->img) {
        x11_w->img = XCreateImage(x11_w->display, x11_w->visual, 24, ZPixmap,
                                  0, (char *) x11_w->fb.buffer, x11_w->fb.width, x11_w->fb.height, 32,
                                  framebuffer_rgba_num_bytes_per_line(&x11_w->fb));
    }
    XClearArea(x11_w->display, x11_w->window, 0, 0, 1, 1, 1);
    XSync(x11_w->display, 0);
}

void window_update_palette(Window_t *w, const Palette8Bit_t *p) {
    assert(w);
    assert(p);
    X11Window_t *x11_w = (X11Window_t *) w;
    x11_w->palette = p;
}

void window_fill_8bit(Window_t *w, const Framebuffer8Bit_t *gb) {
    X11Window_t *x11_w = (X11Window_t *) w;
    assert(x11_w->palette);

    if (!x11_w->fb.buffer) {
        framebuffer_rgba_init(&x11_w->fb, gb->width, gb->height);
    }
    if (x11_w->fb.width != gb->width || x11_w->fb.height != gb->height) {
        log_warning("width or height differ");
        return;
    }
    framebuffer_rgba_fill_8bit(&x11_w->fb, gb, x11_w->palette);
    framebuffer_rgba_swap(&x11_w->fb);

    if (!x11_w->img) {
        x11_w->img = XCreateImage(x11_w->display, x11_w->visual, 24, ZPixmap,
                                  0, (char *) x11_w->fb.buffer, x11_w->fb.width, x11_w->fb.height, 32,
                                  framebuffer_rgba_num_bytes_per_line(&x11_w->fb));
    }
    XClearArea(x11_w->display, x11_w->window, 0, 0, 1, 1, 1);
    XSync(x11_w->display, 0);
}

// TODO: support motion events
void window_poll_events(Window_t *w, char *closeEvent, KeyEvent_t *keyEvents, MouseEvent_t *mouseEvents, int maxEvents) {
    XEvent event;
    X11Window_t *x11_w = (X11Window_t *) w;
    *closeEvent = 0;
    int numKeyEvents = 0;
    int numMouseEvents = 0;

    key_event_init(keyEvents, maxEvents);
    mouse_event_init(mouseEvents, maxEvents);

    char buf[8];
    while (numKeyEvents < maxEvents &&
           XCheckWindowEvent(x11_w->display, x11_w->window, ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonPressMask, &event)) {
        if (event.type == Expose) {
            if (x11_w->img)
                XPutImage(x11_w->display,
                          x11_w->window,
                          DefaultGC(x11_w->display, x11_w->screen),
                          x11_w->img,
                          0, 0, 0, 0,
                          x11_w->fb.width,
                          x11_w->fb.height);
        } else if (event.type == KeyPress) {
            keyEvents[numKeyEvents].event = KEY_EVENT_PRESSED;
            KeySym keysym = XLookupKeysym(&event.xkey, 0);
            switch (keysym) {
                case XK_Escape:
                    keyEvents[numKeyEvents].type = KEY_TYPE_ESC;
                    break;
                case XK_Down:
                    keyEvents[numKeyEvents].type = KEY_TYPE_DOWN;
                    break;
                case XK_Up:
                    keyEvents[numKeyEvents].type = KEY_TYPE_UP;
                    break;
                case XK_Left:
                    keyEvents[numKeyEvents].type = KEY_TYPE_LEFT;
                    break;
                case XK_Right:
                    keyEvents[numKeyEvents].type = KEY_TYPE_RIGHT;
                    break;
                case XK_F1:
                    keyEvents[numKeyEvents].type = KEY_TYPE_F1;
                    break;
                case XK_F2:
                    keyEvents[numKeyEvents].type = KEY_TYPE_F2;
                    break;
                case XK_F3:
                    keyEvents[numKeyEvents].type = KEY_TYPE_F3;
                    break;
                case XK_F4:
                    keyEvents[numKeyEvents].type = KEY_TYPE_F4;
                    break;
                case XK_F5:
                    keyEvents[numKeyEvents].type = KEY_TYPE_F5;
                    break;
                case XK_F6:
                    keyEvents[numKeyEvents].type = KEY_TYPE_F6;
                    break;
                default:
                    if (XLookupString(&event.xkey, buf, 8, NULL, NULL)) {
                        keyEvents[numKeyEvents].type = KEY_TYPE_CODE;
                        keyEvents[numKeyEvents].code = buf[0];
                    }
            }
            ++numKeyEvents;
        } else if (event.type == KeyRelease) {
            keyEvents[numKeyEvents].event = KEY_EVENT_RELEASED;
            KeySym keysym = XLookupKeysym(&event.xkey, 0);
            switch (keysym) {
                case XK_Escape:
                    keyEvents[numKeyEvents].type = KEY_TYPE_ESC;
                    break;
                case XK_Down:
                    keyEvents[numKeyEvents].type = KEY_TYPE_DOWN;
                    break;
                case XK_Up:
                    keyEvents[numKeyEvents].type = KEY_TYPE_UP;
                    break;
                case XK_Left:
                    keyEvents[numKeyEvents].type = KEY_TYPE_LEFT;
                    break;
                case XK_Right:
                    keyEvents[numKeyEvents].type = KEY_TYPE_RIGHT;
                    break;
                default:
                    if (XLookupString(&event.xkey, buf, 8, NULL, NULL)) {
                        keyEvents[numKeyEvents].type = KEY_TYPE_CODE;
                        keyEvents[numKeyEvents].code = buf[0];
                    }
            }
            ++numKeyEvents;
        } else if (event.type == ButtonPress || event.type == ButtonRelease) {
            if (event.xbutton.type == ButtonPress) {
                mouseEvents[numMouseEvents].event = MOUSE_EVENT_BUTTON_PRESSED;
            } else if (event.xbutton.type == ButtonRelease) {
                mouseEvents[numMouseEvents].event = MOUSE_EVENT_BUTTON_RELEASED;
            } else {
                continue;
            }
            if (event.xbutton.button == 1) {
                mouseEvents[numMouseEvents].button = MOUSE_BUTTON_0;
            } else if (event.xbutton.button == 3) {
                mouseEvents[numMouseEvents].button = MOUSE_BUTTON_1;
            } else {
                mouseEvents[numMouseEvents].button = MOUSE_BUTTON_NONE;
            }
            mouseEvents[numMouseEvents].position_x = event.xbutton.x;
            mouseEvents[numMouseEvents].position_y = event.xbutton.y;
            ++numMouseEvents;
        } else if (event.type == ClientMessage) {
            *closeEvent = 1;
        }
    }

    XSync(x11_w->display, 0);
}
