#include "easing/Sine.h"

#include <math.h>

#ifndef PI
#define PI  3.14159265
#endif

float easing_sine_easeIn(float t, float b, float c, float d) {
    return -c * cos(t / d * (PI / 2)) + c + b;
}

float easing_sine_easeOut(float t, float b, float c, float d) {
    return c * sin(t / d * (PI / 2)) + b;
}

float easing_sine_easeInOut(float t, float b, float c, float d) {
    return -c / 2 * (cos(PI * t / d) - 1) + b;
}
