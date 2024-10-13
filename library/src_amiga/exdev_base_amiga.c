/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#include "exdevgfx/exdev_base.h"

#define EXDEVGFX2_LOG_LEVEL 1

#include "exdevgfx/logger.h"

#include <devices/console.h>
#include <proto/console.h>
#include <devices/timer.h>
#include <proto/timer.h>
#include <proto/exec.h>

int exdev_base_initiated = 0;

struct IntuitionBase *IntuitionBase = 0;
struct GfxBase *GfxBase = 0;
struct Library *GadToolsBase = 0;
struct Library *CyberGfxBase = 0;

//#define DEVICE_TYPE Library

struct IOStdReq console_ioreq;
struct Library *ConsoleDevice = 0;

struct IORequest timer_ioreq;
struct Library *TimerBase = 0;

int exdev_base_init() {
    if (exdev_base_initiated) {
        return 0;
    }
    log_info("--> exdev_base_init()");
    IntuitionBase = (struct IntuitionBase *) OpenLibrary("intuition.library", 40L);
    if (!IntuitionBase) {
        return 1;
    }

    GfxBase = (struct GfxBase *) OpenLibrary("graphics.library", 40L);
    if (!GfxBase) {
        return 2;
    }

    GadToolsBase = (struct Library *) OpenLibrary("gadtools.library", 0L);
    if (!GadToolsBase) {
        return 3;
    }

    CyberGfxBase = (struct Library *) OpenLibrary("cybergraphics.library", 41L);
    if (!CyberGfxBase) {
        return 4;
    }

    OpenDevice("console.device", -1, (struct IORequest *) &console_ioreq, 0);
    ConsoleDevice = (struct Library *) console_ioreq.io_Device;

    OpenDevice("timer.device", 0, &timer_ioreq, 0);
    TimerBase = (struct Library *) timer_ioreq.io_Device;

    exdev_base_initiated = 1;
    log_info("<-- exdev_base_init()");
    return 0;
}

int exdev_base_deinit() {
    if (!exdev_base_initiated) {
        return 0;
    }
    log_info("--> exdev_base_deinit()");

    CloseDevice(&timer_ioreq);
    CloseDevice((struct IORequest *) &console_ioreq);

    if (CyberGfxBase) {
        CloseLibrary((struct Library *) CyberGfxBase);
        CyberGfxBase = 0;
    }

    if (GadToolsBase) {
        CloseLibrary((struct Library *) GadToolsBase);
        GadToolsBase = 0;
    }

    if (GfxBase) {
        CloseLibrary((struct Library *) GfxBase);
        GfxBase = 0;
    }

    if (IntuitionBase) {
        CloseLibrary((struct Library *) IntuitionBase);
        IntuitionBase = 0;
    }

    exdev_base_initiated = 0;
    log_info("<-- exdev_base_deinit()");
    return 0;
}
