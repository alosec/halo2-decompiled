// Exact Microsoft-ABI scalar helpers used by dormant DirectSound channel
// filter and LFO parameter submission.  No stream or device work is
// performed here.

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include <math.h>

struct s_sound_random_parameter
{
	real lower;
	real upper;
	real offset;
	real variation;
};

// @retail 0x21e330
long function_21e330(real decibels)
{
	long value = real_truncate(
		(real)((exp(decibels * 0.115129255f) - 1.0f) * 8192.0f));
	if (value >= 0)
	{
		if (value > 0x7fff)
			return 0x7fff;
		return value;
	}

	long negative = 0xc000 - value;
	if (negative < 0xc000)
		return 0xc000;
	return negative > 0xdfff ? 0xdfff : negative;
}

// @retail 0x219880
real function_219880(
	s_sound_random_parameter const *parameter,
	dword *seed,
	real interpolation)
{
	real value = parameter->lower +
		(parameter->upper - parameter->lower) * interpolation;

	*seed = *seed * 0x19660d + 0x3c6ef35f;
	real random = -parameter->variation +
		(parameter->variation - -parameter->variation) *
		((real)(*seed >> 16) * (1.0f / 65535.0f));

	return random + parameter->offset + value;
}

static_assert(sizeof(s_sound_random_parameter) == 0x10,
	"sound random parameter layout changed");
