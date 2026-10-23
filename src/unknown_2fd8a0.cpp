// @flags /O2 /GL-
// Havok (third-party physics library): the bounds of a box under a transform,
// grown by the box's radius and a tolerance (through the game's 0x1ce210).
#include <xmmintrin.h>
#ifndef H2_NATIVE_PORT

void __cdecl function_1ce210(const __m128 *matrix, const __m128 *extent,
	const __m128 *center, float expansion, __m128 *output);
#endif

class box_2fd8a0
{
public:
	char unknown0[0xc];
	float radius;
	__m128 extent;
	__m128 center;

	void function_2fd8a0(const __m128 *matrix, float tolerance, __m128 *output) const;
};

#ifdef H2_NATIVE_PORT
static float box_2fd8a0_absolute(float value)
{
	union
	{
		float real;
		unsigned int bits;
	} converted;
	converted.real = value;
	converted.bits &= 0x7fffffff;
	return converted.real;
}
#endif

// @retail 0x2fd8a0
void box_2fd8a0::function_2fd8a0(const __m128 *matrix, float tolerance, __m128 *output) const
{
#ifdef H2_NATIVE_PORT
	float combined_radius = tolerance + radius;
	for (int component = 0; component < 4; component++)
	{
		float x = box_2fd8a0_absolute(
			matrix[0].m128_f32[component] * extent.m128_f32[0]);
		float y = box_2fd8a0_absolute(
			matrix[1].m128_f32[component] * extent.m128_f32[1]);
		float z = box_2fd8a0_absolute(
			matrix[2].m128_f32[component] * extent.m128_f32[2]);
		float half_extent = (x + y) + (z + combined_radius);
		float transformed_center =
			matrix[0].m128_f32[component] * center.m128_f32[0] +
			matrix[1].m128_f32[component] * center.m128_f32[1] +
			matrix[2].m128_f32[component] * center.m128_f32[2] +
			matrix[3].m128_f32[component];
		output[1].m128_f32[component] = transformed_center + half_extent;
		output[0].m128_f32[component] = transformed_center - half_extent;
	}
#else
	function_1ce210(matrix, &extent, &center, tolerance + radius, output);
#endif
}
