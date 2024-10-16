#ifndef PENNER_BOUNCE_H
#define PENNER_BOUNCE_H

#ifdef __cplusplus
extern "C" {
#endif

float easing_bounce_easeIn(float t, float b, float c, float d);

float easing_bounce_easeOut(float t, float b, float c, float d);

float easing_bounce_easeInOut(float t, float b, float c, float d);

#ifdef __cplusplus
}
#endif

#endif
