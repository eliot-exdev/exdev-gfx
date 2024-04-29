#ifndef PENNER_CUBIC_H
#define PENNER_CUBIC_H

#ifdef __cplusplus
extern "C" {
#endif

float easing_cubic_easeIn(float t, float b, float c, float d);

float easing_cubic_easeOut(float t, float b, float c, float d);

float easing_cubic_easeInOut(float t, float b, float c, float d);

#ifdef __cplusplus
}
#endif

#endif
