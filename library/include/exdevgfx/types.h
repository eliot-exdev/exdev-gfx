/**
 * Copyright 2025 Andre Geisler (andre@exdev.de)
 */

#ifndef EXDEVGFX_BASE_TYPES_H
#define EXDEVGFX_BASE_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef EXDEV_FP_MATH
#include <fpmath/fpmath.h>
#else
typedef float exdev_fp_t;
#endif

#define EXDEV_FLOAT exdev_fp_t
#define EXDEV_TIMESTAMP long

#ifdef __cplusplus
}
#endif

#endif //EXDEVGFX_BASE_TYPES_H
