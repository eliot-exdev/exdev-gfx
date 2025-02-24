/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#include "exdevgfx/helper.h"
#include <sys/time.h>

static struct timeval tv;

EXDEV_TIMESTAMP now() {
    gettimeofday(&tv, NULL);
    return (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
}
