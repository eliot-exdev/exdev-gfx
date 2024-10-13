#ifndef PENNER_ELASTIC_H
#define PENNER_ELASTIC_H

#ifdef __cplusplus
}
#endif

float easing_elastic_easeIn(float t, float b, float c, float d);

float easing_elastic_easeOut(float t, float b, float c, float d);

float easing_elastic_easeInOut(float t, float b, float c, float d);

#ifdef __cplusplus
extern "C" {
#endif

#endif
