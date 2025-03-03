/**
 * Copyright 2025 Andre Geisler (andre@exdev.de)
 */


#ifndef EXDEVGFX_BASE_FPMATH_H
#define EXDEVGFX_BASE_FPMATH_H

#ifdef __cplusplus
extern "C" {
#endif

#define FIXMATH_NO_ROUNDING
#include <fpmath/fix16.h>

typedef int32_t exdev_fp_t;

// conversions
#define exdev_float_to_fp(F) fix16_from_float(F)
#define exdev_int_to_fp(I) fix16_from_int(I)
#define exdev_fp_to_int(F) fix16_to_int(F)
#define exdev_fp_to_float(F) fix16_to_float(F)

// simple math
#define exdev_fp_add(A, B) fix16_add(A, B)
#define exdev_fp_sub(A, B) fix16_sub(A, B)
#define exdev_fp_mul(A, B) fix16_mul(A, B)
#define exdev_fp_div(A, B) fix16_div(A,B)

// math
#define exdev_fp_sin(F) fix16_sin(F)
#define exdev_fp_cos(F) fix16_cos(F)
#define exdev_fp_sqrt(F) fix16_sqrt(F)
#define exdev_fp_abs(F) fix16_abs(F)

// constants
#define EXDEV_FP_ZERO 0
#define EXDEV_FP_ONE fix16_one
#define EXDEV_FP_ONE_HALF exdev_fp_div(EXDEV_FP_ONE, exdev_int_to_fp(2))
#define EXDEV_FP_MINUS_ONE (-EXDEV_FP_ONE)

#define EXDEV_FP_PI fix16_pi
#define EXDEV_FP_HALF_PI exdev_fp_div(EXDEV_FP_PI, exdev_int_to_fp(2))
#define EXDEV_FP_TWO_PI exdev_fp_mul(EXDEV_FP_PI, exdev_int_to_fp(2))

#ifdef __cplusplus
}
#endif

#endif //EXDEVGFX_BASE_FPMATH_H

