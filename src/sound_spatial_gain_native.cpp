#include "unknown_11c920.h"
#include <math.h>

#ifdef H2_NATIVE_PORT

/* Exact recovered signed-range sound gain curve helper. */

real function_2195f0(real decibels);
long function_2197f0(real gain);

static long sound_gain_interpolate_linear(real t)
{
	real fraction = t;
	if (0.0f > t)
		fraction = 0.0f;
	else if (t > 1.0f)
		fraction = 1.0f;
	real const lower = function_2195f0(-64.0f);
	real const upper = function_2195f0(0.0f);
	return function_2197f0((upper - lower) * fraction + lower);
}

static long sound_gain_interpolate_power(real t)
{
	real fraction = t;
	if (0.0f > t)
		fraction = 0.0f;
	else if (t > 1.0f)
		fraction = 1.0f;
	real const lower = function_2195f0(-64.0f);
	real const upper = function_2195f0(0.0f);
	if (upper > lower)
		fraction = static_cast<real>(sqrt(fraction));
	else
		fraction = 1.0f - static_cast<real>(sqrt(1.0f - fraction));
	return function_2197f0((upper - lower) * fraction + lower);
}

// @retail 0x12a6d0
long function_12a6d0(short curve, real value, real range)
{
	real t = static_cast<real>(fabs(value / range));
	if (t < 0.0f)
		t = 0.0f;
	else if (t > 1.0f)
		t = 1.0f;
	if (0.0f > range)
		t = 1.0f - t;
	long result;
	if (curve == 0)
		result = sound_gain_interpolate_linear(t);
	else if (curve == 1)
		result = sound_gain_interpolate_power(t);
	return result;
}

#endif
