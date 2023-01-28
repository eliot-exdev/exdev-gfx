/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#include "exdevgfx/helper.h"
#include "exdevgfx/exdev_base.h"

static struct timeval a;

TIMESTAMP now(){
  GetSysTime(&a);
  return a.tv_secs*1000 + a.tv_micro/1000;
}