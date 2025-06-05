/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#include "exdevgfx/exdev_base.h"

#define EXDEVGFX2_LOG_LEVEL 1

#include "exdevgfx/logger.h"

//#include <devices/console.h>
//#include <proto/console.h>
//#include <devices/timer.h>
#include <proto/timer.h>
#include <proto/exec.h>
//#include <libraries/asl.h>
//#include <utility/tagitem.h>
//#include <proto/asl.h>

int exdev_base_initiated = 0;

struct IntuitionBase *IntuitionBase = 0;
struct GfxBase *GfxBase = 0;
struct Library *GadToolsBase = 0;
//struct Library *CyberGfxBase = 0;
struct Library *AslBase = 0;
//#define DEVICE_TYPE Library

struct IOStdReq console_ioreq;
#ifdef __MORPHOS__
struct Library *ConsoleDevice = 0;
#elif __AMIGA__
struct Device *ConsoleDevice = 0;
#endif

struct IORequest timer_ioreq;
#ifdef __MORPHOS__
struct Library *TimerBase = 0;
#elif __AMIGA__
struct Device *TimerBase = 0;
#endif

#ifdef USE_C2P
struct Library *C2PBase;
#endif

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

    AslBase = (struct Library *) OpenLibrary("asl.library", 40L);
    if (!AslBase) {
        return 4;
    }

    //    CyberGfxBase = (struct Library *) OpenLibrary("cybergraphics.library", 41L);
    //    if (!CyberGfxBase) {
    //        return 4;
    //    }

    OpenDevice("console.device", -1, (struct IORequest *) &console_ioreq, 0);
#ifdef __MORPHOS__
    ConsoleDevice = (struct Library *) console_ioreq.io_Device;
#elif __AMIGA__
    ConsoleDevice = (struct Device *) console_ioreq.io_Device;
#endif

    OpenDevice("timer.device", 0, &timer_ioreq, 0);
#ifdef __MORPHOS__
    TimerBase = (struct Library *) timer_ioreq.io_Device;
#elif __AMIGA__
    TimerBase = (struct Device *) timer_ioreq.io_Device;
#endif

#ifdef USE_C2P
    C2PBase = OpenLibrary("c2p.library", 0);
    if (C2PBase == NULL) {
        log_warning("could not open c2p.library");
        return 1;
    }
#endif

    exdev_base_initiated = 1;
    log_info("<-- exdev_base_init()");
    return 0;
}

int exdev_base_deinit() {
    if (!exdev_base_initiated) {
        return 0;
    }
    log_info("--> exdev_base_deinit()");

#ifdef USE_C2P
    if (C2PBase) {
        CloseLibrary(C2PBase);
        C2PBase = NULL;
    }
#endif

    CloseDevice(&timer_ioreq);

    CloseDevice((struct IORequest *) &console_ioreq);

    //    if (CyberGfxBase) {
    //        CloseLibrary((struct Library *) CyberGfxBase);
    //        CyberGfxBase = 0;
    //    }

    if (AslBase) {
        CloseLibrary((struct Library *) AslBase);
        AslBase = NULL;
    }

    if (GadToolsBase) {
        CloseLibrary((struct Library *) GadToolsBase);
        GadToolsBase = NULL;
    }

    if (GfxBase) {
        CloseLibrary((struct Library *) GfxBase);
        GfxBase = NULL;
    }

    if (IntuitionBase) {
        CloseLibrary((struct Library *) IntuitionBase);
        IntuitionBase = NULL;
    }

    exdev_base_initiated = 0;
    log_info("<-- exdev_base_deinit()");
    return 0;
}
