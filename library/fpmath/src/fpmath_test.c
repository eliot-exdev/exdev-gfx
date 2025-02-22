/**
 * Copyright 2025 Andre Geisler (andre@exdev.de)
 */

#include "fpmath/fpmath.h"

#include <stdio.h>
#include <math.h>
#include <assert.h>

#define compare_floats(A, B) (A-B<0.1f && A-B > -0.1f)

int main() {
    printf("begin test fpmath ...\n");

    // test integer conversion
    {
        printf("-> integer\n");
        const int i = 42;
        const exdev_fp_t fp = exdev_int_to_fp(i);
        const int ii = exdev_fp_to_int(fp);
        assert(i == ii);
        printf("i: %d, ii=%d\n", i, ii);
    }

    // test float conversion
    {
        printf("-> float\n");
        const float f = 0.5f;
        const exdev_fp_t fp = exdev_float_to_fp(f);
        const float ff = exdev_fp_to_float(fp);
        assert(compare_floats(f, ff));
        printf("f: %f, ff: %f\n", f, ff);
    }

    // test add
    {
        printf("-> add\n");
        const float f = 42.5f;
        const float ff = 33.3f;
        const float fff = f + ff;

        const exdev_fp_t fp = exdev_float_to_fp(f);
        const exdev_fp_t fpp = exdev_float_to_fp(ff);
        const exdev_fp_t fppp = exdev_fp_add(fp, fpp);
        const float ffff = exdev_fp_to_float(fppp);
        assert(compare_floats(fff, ffff));
        printf("fff: %f, ffff: %f\n", fff, ffff);
    }

    // test sub
    {
        printf("-> sub\n");
        const float f = 42.5f;
        const float ff = 33.3f;
        const float fff = f - ff;

        const exdev_fp_t fp = exdev_float_to_fp(f);
        const exdev_fp_t fpp = exdev_float_to_fp(ff);
        const exdev_fp_t fppp = exdev_fp_sub(fp, fpp);
        const float ffff = exdev_fp_to_float(fppp);
        assert(compare_floats(fff, ffff));
        printf("fff: %f, ffff: %f\n", fff, ffff);
    }

    // test mul
    {
        printf("-> mul\n");
        const float f = 42.5f;
        const float ff = 33.3f;
        const float fff = f * ff;

        const exdev_fp_t fp = exdev_float_to_fp(f);
        const exdev_fp_t fpp = exdev_float_to_fp(ff);
        const exdev_fp_t fppp = exdev_fp_mul(fp, fpp);
        const float ffff = exdev_fp_to_float(fppp);
        assert(compare_floats(fff, ffff));
        printf("fff: %f, ffff: %f\n", fff, ffff);
    }

    // test div
    {
        printf("-> div\n");
        const float f = 42.5f;
        const float ff = 33.3f;
        const float fff = f / ff;

        const exdev_fp_t fp = exdev_float_to_fp(f);
        const exdev_fp_t fpp = exdev_float_to_fp(ff);
        const exdev_fp_t fppp = exdev_fp_div(fp, fpp);
        const float ffff = exdev_fp_to_float(fppp);
        assert(compare_floats(fff, ffff));
        printf("fff: %f, ffff: %f\n", fff, ffff);
    }

    // test sin
    {
        printf("-> sin\n");
        const float f = sinf(M_PI);
        const float ff = sinf(2.0 * M_PI);
        const float fff = sinf(0.f);
        const float ffff = sinf(M_PI_2);

        const exdev_fp_t fp = exdev_fp_sin(EXDEV_FP_PI);
        const exdev_fp_t fpp = exdev_fp_sin(EXDEV_FP_TWO_PI);
        const exdev_fp_t fppp = exdev_fp_sin(EXDEV_FP_ZERO);
        const exdev_fp_t fpppp = exdev_fp_sin(EXDEV_FP_HALF_PI);

        const float f_f = exdev_fp_to_float(fp);
        const float ff_f = exdev_fp_to_float(fpp);
        const float fff_f = exdev_fp_to_float(fppp);
        const float ffff_f = exdev_fp_to_float(fpppp);

        assert(compare_floats(f, f_f));
        printf("f: %f, f_f: %f\n", f, f_f);

        assert(compare_floats(ff, ff_f));
        printf("ff: %f, ff_f: %f\n", ff, ff_f);

        assert(compare_floats(fff, fff_f));
        printf("fff: %f, fff_f: %f\n", fff, fff_f);

        assert(compare_floats(ffff, ffff_f));
        printf("ffff: %f, ffff_f: %f\n", ffff, ffff_f);
    }

    // test cos
    {
        printf("-> cos\n");
        const float f = cosf(M_PI);
        const float ff = cosf(2.0 * M_PI);
        const float fff = cosf(0.f);
        const float ffff = cosf(M_PI_2);

        const exdev_fp_t fp = exdev_fp_cos(EXDEV_FP_PI);
        const exdev_fp_t fpp = exdev_fp_cos(EXDEV_FP_TWO_PI);
        const exdev_fp_t fppp = exdev_fp_cos(EXDEV_FP_ZERO);
        const exdev_fp_t fpppp = exdev_fp_cos(EXDEV_FP_HALF_PI);

        const float f_f = exdev_fp_to_float(fp);
        const float ff_f = exdev_fp_to_float(fpp);
        const float fff_f = exdev_fp_to_float(fppp);
        const float ffff_f = exdev_fp_to_float(fpppp);

        assert(compare_floats(f, f_f));
        printf("f: %f, f_f: %f\n", f, f_f);

        assert(compare_floats(ff, ff_f));
        printf("ff: %f, ff_f: %f\n", ff, ff_f);

        assert(compare_floats(fff, fff_f));
        printf("fff: %f, fff_f: %f\n", fff, fff_f);

        assert(compare_floats(ffff, ffff_f));
        printf("ffff: %f, ffff_f: %f\n", ffff, ffff_f);
    }

    // test sqrt
    {
        printf("-> sqrt\n");
        const float f = 10.89f;
        const float ff = sqrtf(f);

        const exdev_fp_t fp = exdev_float_to_fp(f);
        const exdev_fp_t fpp = exdev_fp_sqrt(fp);
        const float fff = exdev_fp_to_float(fpp);

        assert(compare_floats(ff, fff));
        printf("ff: %f, fff: %f\n", ff, fff);
    }

    // test abs
    {
        printf("-> abs\n");
        const float f = -10.8933f;
        const float ff = fabsf(f);

        const exdev_fp_t fp = exdev_float_to_fp(f);
        const exdev_fp_t fpp = exdev_fp_abs(fp);
        const float fff = exdev_fp_to_float(fpp);

        assert(compare_floats(ff, fff));
        printf("ff: %f, fff: %f\n", ff, fff);
    }

    printf("... end test fpmath!\n");
    return 0;
}
