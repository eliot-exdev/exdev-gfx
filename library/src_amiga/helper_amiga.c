#include "exdevgfx/helper.h"
#include "exdevgfx/exdev_base.h"

static struct timeval t;
static struct timeval a, b;

TIMESTAMP now(){
  GetSysTime(&a);
  b = a;
  SubTime(&b, &t);
  t = a;

  return b.tv_secs*1000 + b.tv_micro/1000;
}