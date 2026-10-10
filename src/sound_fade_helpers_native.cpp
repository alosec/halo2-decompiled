#include "unknown_11c920.h"
#include <math.h>

#ifdef H2_NATIVE_PORT

/* Exact recovered bounded interpolation used by playback fades. */

// @retail 0x12aff0
real function_12aff0(real a, real b, real c, bool flag)
{
	if (!(0.0001f > fabs(a - b)))
	{
		real t = (c - a) / (b - a);
		if (t < 0.0f)
			t = 0.0f;
		else if (t > 1.0f)
			t = 1.0f;
		return t;
	}

	return (a > c) == flag ? 0.0f : 1.0f;
}

#endif
