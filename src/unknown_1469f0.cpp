// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1469F0.CPP: game seconds to ticks, rounded (the out-of-line copy;
   unknown_29f5b0.cpp and unknown_1c0290.cpp inline their own) */

#include "unknown_11c920.h"
#include "globals.h"

__declspec(noinline) long function_1469f0(real seconds);

// @retail 0x1469f0
long function_1469f0(real seconds)
{
	long ticks;

	seconds = (real)g_510c54->field_2_3 * seconds;
#ifdef H2_NATIVE_PORT
	/* Retail fistp uses nearest-even under the default x87 control word. */
	double value = seconds;
	long truncated = (long)value;
	double fraction = value - (double)truncated;
	if (fraction > 0.5 || (fraction == 0.5 && (truncated & 1)))
		++truncated;
	else if (fraction < -0.5 || (fraction == -0.5 && (truncated & 1)))
		--truncated;
	ticks = truncated;
#else
	__asm
	{
		fld seconds
		fistp ticks
	}
#endif
	return ticks;
}