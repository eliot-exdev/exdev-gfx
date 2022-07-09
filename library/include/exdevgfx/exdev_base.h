/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#ifndef EXDEVGFX2_EXDEV_BASE_H
#define EXDEVGFX2_EXDEV_BASE_H

extern int exdev_base_initiated;

#if defined(__amigaos4__) || defined(__MORPHOS__) || defined(__AROS__)
#include <proto/exec.h>
#include <proto/intuition.h>

#include <proto/graphics.h>

#include <proto/gadtools.h>

#include <proto/cybergraphics.h>

#include <proto/console.h>
#include <proto/dos.h>
extern struct IOStdReq ioreq;

#include <proto/timer.h>
extern struct IORequest timer_ioreq;

#endif

int exdev_base_init();

int exdev_base_deinit();

#endif //EXDEVGFX2_EXDEV_BASE_H
