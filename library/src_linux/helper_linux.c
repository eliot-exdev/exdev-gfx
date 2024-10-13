#include "exdevgfx/helper.h"
#include <sys/time.h>

static struct timeval tv;

TIMESTAMP now() {
    gettimeofday(&tv, NULL);
    return (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
}
