#include "easing/Quad.h"

float easing_quad_easeIn(float t, float b, float c, float d) {
    return c * (t /= d) * t + b;
}

float easing_quad_easeOut(float t, float b, float c, float d) {
    return -c * (t /= d) * (t - 2) + b;
}

float easing_quad_easeInOut(float t, float b, float c, float d) {
    if ((t /= d / 2) < 1) return ((c / 2) * (t * t)) + b;
    return -c / 2 * (((t - 2) * (--t)) - 1) + b;

}