#ifndef PENNER_LINEAR_H
#define PENNER_LINEAR_H

#ifdef __cplusplus
extern "C" {
#endif

float easing_linear_easeNone(float t, float b, float c, float d);

float easing_linear_easeIn(float t, float b, float c, float d);

float easing_linear_easeOut(float t, float b, float c, float d);

float easing_linear_easeInOut(float t, float b, float c, float d);

#ifdef __cplusplus
}
#endif

#endif
