#ifndef H2_PORT_CRT_MATH_H
#define H2_PORT_CRT_MATH_H

/* Freestanding declarations for the recovered guest. Implementations are
 * supplied by the native CRT bridge; this header intentionally contains no
 * platform SDK material. */
#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif
