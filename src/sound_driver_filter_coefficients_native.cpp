// Exact Microsoft-ABI scalar reconstruction of the two DirectSound filter
// coefficient encoders.  This isolates retail's x86 truncation instruction
// from unrelated Xbox DirectSound declarations without submitting state.

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include <math.h>

// @retail 0x21e270
long sound_filter_frequency_coefficient(real frequency)
{
	if (frequency < 30.0f)
		return 0x8000;
	if (frequency > 8000.0f)
		return 0;

	real local_0 = frequency / 48000.0f;
	volatile real *local_1 = &frequency;
	*local_1 = logf(2.0f * sinf(local_0 * 3.14159265f)) * 5909.2788f;
	return real_truncate(frequency) & 0xffff;
}

// @retail 0x21e2d0
dword sound_filter_gain_coefficient(real decibels)
{
	if (decibels > 22.5f)
		decibels = 22.5f;

	dword coefficient = (dword)(expf(decibels * -0.115129255f) * 32768.0f);
	if (coefficient > 0xffff)
		coefficient = 0xffff;
	return coefficient;
}
