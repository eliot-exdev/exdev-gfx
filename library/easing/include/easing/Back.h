#ifndef PENNER_BACK_H
#define PENNER_BACK_H

#ifdef __cplusplus
extern "C" {
#endif

float easing_back_easeIn(float t, float b, float c, float d);

float easing_back_easeOut(float t, float b, float c, float d);

float easing_back_easeInOut(float t, float b, float c, float d);

#ifdef __cplusplus
}
#endif

#endif
