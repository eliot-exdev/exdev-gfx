/**
 * Copyright 2025 Andre Geisler (andre@exdev.de)
 */


#ifndef EXDEVGFX_BASE_FPMATH_H
#define EXDEVGFX_BASE_FPMATH_H

#define FPT_WBITS 17

#include <fpmath/fptc.h>

typedef fpt exdev_fp_t;

// constants
#define EXDEV_FP_ZERO FPT_ZERO
#define EXDEV_FP_ONE FPT_ONE
#define EXDEV_FP_MINUS_ONE (-EXDEV_FP_ONE)

#define EXDEV_FP_PI FPT_PI
#define EXDEV_FP_HALF_PI FPT_HALF_PI
#define EXDEV_FP_TWO_PI FPT_TWO_PI

// conversions
#define exdev_float_to_fp(R) fl2fpt(R)
#define exdev_int_to_fp(I) i2fpt(I)
#define exdev_fp_to_int(F) fpt2i(F)
#define exdev_fp_to_float(F) fpt2fl(F)

// simple math
#define exdev_fp_add(A, B) fpt_add(A,B)
#define exdev_fp_sub(A, B) fpt_sub(A,B)
#define exdev_fp_mul(A, B) fpt_mul(A,B)
#define exdev_fp_div(A, B) fpt_div(A,B)

// math
#define exdev_fp_sin(F) fpt_sin(F)
#define exdev_fp_cos(F) fpt_cos(F)
#define exdev_fp_sqrt(A) fpt_sqrt(A)
#define exdev_fp_abs(A) fpt_abs(A)

#endif //EXDEVGFX_BASE_FPMATH_H
