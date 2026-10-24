#ifndef H2_PORT_CRT_MATH_H
#define H2_PORT_CRT_MATH_H

/* Freestanding declarations for the recovered guest. Implementations are
 * supplied by the native CRT bridge; this header intentionally contains no
 * platform SDK material. */
/* As in the Microsoft CRT, <math.h> also declares abs. */
#ifdef H2_PORT_SHIMS
#define abs h2_guest_abs
#endif

#ifdef __cplusplus
extern "C" {
#endif

int abs(int value);
double acos(double value);
double asin(double value);
double atan2(double y, double x);
double ceil(double value);
double atan(double value);
double cos(double value);
double exp(double value);
double fabs(double value);
double floor(double value);
double fmod(double numerator, double denominator);
double log(double value);
double log10(double value);
double pow(double base, double exponent);
double sin(double value);
double sqrt(double value);
double tan(double value);

float cosf(float value);
float expf(float value);
float fabsf(float value);
float logf(float value);
float sinf(float value);
float sqrtf(float value);

static inline float tanf(float value) { return (float)tan(value); }
static inline float powf(float base, float exponent) { return (float)pow(base, exponent); }

#ifdef __cplusplus
}
#endif

#endif
