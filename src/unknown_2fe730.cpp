// @flags /O2 /GL-
// Havok (third-party physics library): the bounding box of a strided array
// of points.
#include <xmmintrin.h>

struct bounds_2fe730
{
	__m128 lower;
	__m128 upper;
};

inline float larger_2fe730(float a, float b)
{
	return a > b ? a : b;
}

inline float smaller_2fe730(float a, float b)
{
	return a < b ? a : b;
}

// the box starts as the first point (fourth float 0) and grows to take in
// every point, stride bytes apart
// @retail 0x2fe730
void __cdecl function_2fe730(const void *points, long count, long stride, void *output)
{
	float const *point = (float const *)points;
	bounds_2fe730 &out = *(bounds_2fe730 *)output;
	out.lower.m128_f32[0] = point[0];
	out.lower.m128_f32[1] = point[1];
	out.lower.m128_f32[2] = point[2];
	out.lower.m128_f32[3] = 0.0f;
	out.upper = out.lower;
	for (long i = 0; i < count; i++)
	{
		out.upper.m128_f32[0] = larger_2fe730(out.upper.m128_f32[0], point[0]);
		out.upper.m128_f32[1] = larger_2fe730(out.upper.m128_f32[1], point[1]);
		out.upper.m128_f32[2] = larger_2fe730(out.upper.m128_f32[2], point[2]);
		out.lower.m128_f32[0] = smaller_2fe730(out.lower.m128_f32[0], point[0]);
		out.lower.m128_f32[1] = smaller_2fe730(out.lower.m128_f32[1], point[1]);
		out.lower.m128_f32[2] = smaller_2fe730(out.lower.m128_f32[2], point[2]);
		point = (float const *)((char const *)point + stride);
	}
}
