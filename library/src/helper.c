/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#include "exdevgfx/helper.h"

#include <stdio.h>
#include <string.h>
//#include <math.h>

#define PI_ 3.141593f

//#define HALF_PI_ 1.570796f
//static const float TWO_PI_ = 6.283185f;
//static const float THREE_HALF_PI_ = 4.7123889f;
//static const float INV_TWO_PI_ = 0.1591549f;

//#define C1 0.99940307f
//#define C2 -0.49558072f
//#define C3 0.03679168f

int read_line(FILE *fp, char *line, const size_t size) {
    size_t res = 0;
    size_t read_bytes = 0;
    memset(line, 0, size);
    do {
        res = fread(line + read_bytes, 1, 1, fp);
        if (res == 1) {
            ++read_bytes;
        } else {
            return read_bytes;
        }
    } while (line[read_bytes - 1] != '\n');
    return read_bytes;
}

float deg_to_rad(const float deg) {
    return deg * PI_ / 180.0f;
}

float rad_to_deg(const float rad) {
    return rad * 180.0f / PI_;
}

// https://github.com/divideconcept/FastTrigo/blob/master/fasttrigo.cpp
//float fast_cos_(const float f) {
//    const float x2 = f * f;
//    return (C1 + x2 * (C2 + C3 * x2));
//}

//float fast_cos(float angle) {
//    return cos(angle);
////    angle = angle - ((int) (angle * INV_TWO_PI_)) * TWO_PI_;
////    angle = angle > 0.f ? angle : -angle;
////
////    if (angle < HALF_PI_)
////        return fast_cos_(angle);
////    if (angle < PI_)
////        return -fast_cos_(PI_ - angle);
////    if (angle < THREE_HALF_PI_)
////        return -fast_cos_(angle - PI_);
////    return fast_cos_(TWO_PI_ - angle);
//}

//float fast_sin(const float f) {
////    return fast_cos(HALF_PI_ - f);
//    return sin(f);
//}

// https://stackoverflow.com/questions/43120045/how-does-this-float-square-root-approximation-work
//float fast_sqrt(float f) {
////    const int32_t result = 0x1fbb4000 + (*(int32_t *) &f >> 1);
////    return *(float *) &result;
//    return sqrt(f);
//}

// https://stackoverflow.com/questions/3380628/fast-arc-cos-algorithm
//#define C 0.10501094f

//float fast_acos(float a) {
////    float r, s, t, u;
////    t = (a < 0) ? (-a) : a;  // handle negative arguments
////    u = 1.0f - t;
////    s = fast_sqrt(u + u);
////    r = C * u * s + s;  // or fmaf (C * u, s, s) if FMA support in hardware
////    if (a < 0) r = PI_ - r;  // handle negative arguments
////    return r;
//    return acos(a);
//}

#if defined(__AMIGA__) || defined(__MORPHOS__)
void swap_bytes_int(int *i) {(void)i;}
#else
void swap_bytes_int(int *i) {
    const int tmp = *i;
    const char *in = (char *) &tmp;
    char *out = (char *) i;

    out[0] = in[3];
    out[1] = in[2];
    out[2] = in[1];
    out[3] = in[0];
}
#endif

float normalize_float(float f, const float d) {
    if (f < 0) {
        while (f < 0) {
            f += d;
        }
    } else {
        while (f > d) {
            f -= d;
        }
    }
    return f;
}
