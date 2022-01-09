#include "exdevgfx/exdev_base.h"

#define EXDEVGFX2_LOG_LEVEL 1

#include "exdevgfx/logger.h"

int exdev_base_initiated = 0;

int exdev_base_init() {
    if (exdev_base_initiated) {
        return 0;
    }
    log_info("--> exdev_base_init()");
    exdev_base_initiated = 1;
    log_info("<-- exdev_base_init()");
    return 0;
}

int exdev_base_deinit() {
    if (!exdev_base_initiated) {
        return 0;
    }
    log_info("--> exdev_base_deinit()");
    exdev_base_initiated = 0;
    log_info("<-- exdev_base_deinit()");
    return 0;
}
