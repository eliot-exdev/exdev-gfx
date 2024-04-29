#include "easing/Linear.h"

float easing_linear_easeNone(float t, float b, float c, float d) {
    return c * t / d + b;
}

float easing_linear_easeIn(float t, float b, float c, float d) {
    return c * t / d + b;
}

float easing_linear_easeOut(float t, float b, float c, float d) {
    return c * t / d + b;
}

float easing_linear_easeInOut(float t, float b, float c, float d) {
    return c * t / d + b;
}
