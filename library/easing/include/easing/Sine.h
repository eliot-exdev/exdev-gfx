#ifndef PENNER_SINE_H
#define PENNER_SINE_H

#ifdef __cplusplus
extern "C" {
#endif

float easing_sine_easeIn(float t, float b, float c, float d);

float easing_sine_easeOut(float t, float b, float c, float d);

float easing_sine_easeInOut(float t, float b, float c, float d);

#ifdef __cplusplus
}
#endif

#endif
