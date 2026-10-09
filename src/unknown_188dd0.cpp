// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_188DD0.CPP: unit vectors packed into 32 bits (11, 11 and 10 bits)
   for the sound code */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"

real function_30bf0(vector3f *v);

static __forceinline long float_to_int_nearest(real value)
{
#ifdef H2_NATIVE_PORT
	long result = (long)value;
	real fraction = value - (real)result;
	if (fraction > 0.5f || (fraction == 0.5f && (result & 1)))
		result++;
	else if (fraction < -0.5f || (fraction == -0.5f && (result & 1)))
		result--;
	return result;
#else
	long result;

	__asm
	{
		fld value
		fistp result
	}
	return result;
#endif
}

#define PIN(value, minimum, maximum) ((value) < (minimum) ? (minimum) : ((value) > (maximum) ? (maximum) : (value)))

static inline long quantize_real(real value, real minimum, real maximum, long maximum_value)
{
	return float_to_int_nearest((value - minimum) * ((real)maximum_value / (maximum - minimum)));
}

static inline real dequantize_real(long value, real minimum, real maximum, long maximum_value)
{
	if (value == 0)
	{
		return minimum;
	}
	if (value >= maximum_value)
	{
		return maximum;
	}
	return ((real)value * maximum + (real)(maximum_value - value) * minimum) * (1.0f / maximum_value);
}

// @retail 0x188dd0
dword vector3d_compress(vector3f const *vector)
{
	real k = PIN(vector->k, -1.0f, 1.0f);
	real j = PIN(vector->j, -1.0f, 1.0f);
	real i = PIN(vector->i, -1.0f, 1.0f);
	long qk = quantize_real(k, -1.0f, 1.0f, 0x3ff);
	long qj = quantize_real(j, -1.0f, 1.0f, 0x7ff);
	long qi = quantize_real(i, -1.0f, 1.0f, 0x7ff);

	return (((qk << 11) | qj) << 11) | qi;
}

// @retail 0x188ea0
vector3f *vector3d_decompress(dword compressed, vector3f *vector)
{
	vector->i = dequantize_real(compressed & 0x7ff, -1.0f, 1.0f, 0x7ff);
	vector->j = dequantize_real((compressed >> 11) & 0x7ff, -1.0f, 1.0f, 0x7ff);
	vector->k = dequantize_real(compressed >> 22, -1.0f, 1.0f, 0x3ff);
	function_30bf0(vector);
	return vector;
}
