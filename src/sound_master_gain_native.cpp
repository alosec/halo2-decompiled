#include "unknown_11c920.h"
#include <math.h>

#ifdef H2_NATIVE_PORT

/* Exact recovered conversion from clamped linear gain to decibel bits. */

static long decibels_pin_bits(real decibels)
{
	if (decibels < -64.0f)
		return static_cast<long>(0xc2800000u);
	if (decibels > 0.0f)
		return 0;
	return *reinterpret_cast<long *>(&decibels);
}

// @retail 0x2197f0
long function_2197f0(real gain)
{
	real value = gain < 0.0f ? 0.0f : (gain > 1.0f ? 1.0f : gain);
	real decibels;
	if (gain > 0.0f &&
		(value = static_cast<real>(20.0f * log10(value)), !(value < -64.0f)))
	{
		decibels = value > 0.0f ? 0.0f : value;
	}
	else
	{
		decibels = -64.0f;
	}
	return decibels_pin_bits(decibels);
}

#endif
