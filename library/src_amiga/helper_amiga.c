/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#include "exdevgfx/helper.h"
#include "exdevgfx/exdev_base.h"

#include <proto/timer.h>

static struct timeval a;

EXDEV_TIMESTAMP now() {
    GetSysTime(&a);
#ifdef EXDEV_FP_MATH
    return a.tv_secs * 1000 + exdev_fp_to_int(exdev_fp_div(exdev_int_to_fp(a.tv_micro), exdev_int_to_fp(1000)));
#else
    return a.tv_secs*1000 + a.tv_micro/1000;
#endif
}