// @flags /O2 /GL-
// Havok (third-party physics library): the closest points of two segments,
// each given by a start and a direction, with flags saying which ends the
// answer was clamped to.
#include <math.h>
#include <xmmintrin.h>

// the closest point on the first segment, the offset from the closest point
// on the second, and the square of that offset's length
struct closest_points_308590
{
	__m128 first_point;
	__m128 offset;
	float distance_squared;
};

// flags: 1 the first segment's end, 2 its start, 4 the second segment's
// end, 8 its start
// @retail 0x308590
int __cdecl function_308590(__m128 const *first_start, __m128 const *first_direction, __m128 const *second_start, __m128 const *second_direction, closest_points_308590 *result)
{
	float second_fraction = 0.0f;
	__m128 const between = _mm_sub_ps(*second_start, *first_start);
	__m128 product = _mm_mul_ps(*first_direction, *second_direction);
	float directions;
	_mm_store_ss(&directions, _mm_add_ss(_mm_shuffle_ps(product, product, 0xaa), _mm_add_ss(_mm_shuffle_ps(product, product, 0x55), product)));
	product = _mm_mul_ps(*first_direction, between);
	float first_along;
	_mm_store_ss(&first_along, _mm_add_ss(_mm_shuffle_ps(product, product, 0xaa), _mm_add_ss(_mm_shuffle_ps(product, product, 0x55), product)));
	product = _mm_mul_ps(*second_direction, between);
	float second_along;
	_mm_store_ss(&second_along, _mm_add_ss(_mm_shuffle_ps(product, product, 0xaa), _mm_add_ss(_mm_shuffle_ps(product, product, 0x55), product)));
	product = _mm_mul_ps(*first_direction, *first_direction);
	float first_length_squared;
	_mm_store_ss(&first_length_squared, _mm_add_ss(_mm_shuffle_ps(product, product, 0xaa), _mm_add_ss(_mm_shuffle_ps(product, product, 0x55), product)));
	product = _mm_mul_ps(*second_direction, *second_direction);
	float second_length_squared;
	_mm_store_ss(&second_length_squared, _mm_add_ss(_mm_shuffle_ps(product, product, 0xaa), _mm_add_ss(_mm_shuffle_ps(product, product, 0x55), product)));

	float const lengths = second_length_squared * first_length_squared;
	float const directions_squared = directions * directions;
	float const determinant = (float)fabs(lengths - directions_squared);
	float numerator = second_length_squared * first_along - second_along * directions;
	int flags;
	float first_fraction;
	if (numerator * determinant >= determinant * determinant)
	{
		first_fraction = 1.0f;
		flags = 1;
	}
	else if (numerator <= 0.0f)
	{
		first_fraction = 0.0f;
		flags = 2;
	}
	else if ((directions_squared + lengths) * 9.5367431640625e-7f < determinant)
	{
		first_fraction = numerator / determinant;
		flags = 0;
	}
	else
	{
		first_fraction = 1.0f;
		flags = 1;
	}

	second_fraction = directions * first_fraction - second_along;
	if (second_fraction >= second_length_squared)
	{
		second_fraction = 1.0f;
		flags = 4;
	}
	else if (second_fraction <= 0.0f)
	{
		second_fraction = 0.0f;
		flags = 8;
	}
	else
	{
		second_fraction = second_fraction / second_length_squared;
		goto placed;
	}
	{
		// the second segment was clamped: the first point follows it
		numerator = directions * second_fraction + first_along;
		if (numerator <= 0.0f)
		{
			first_fraction = 0.0f;
			flags |= 2;
		}
		else if (numerator >= first_length_squared)
		{
			first_fraction = 1.0f;
			flags |= 1;
		}
		else
		{
			first_fraction = numerator / first_length_squared;
		}
	}
placed:
	__m128 const first_scale = _mm_set_ss(first_fraction);
	__m128 const first_point = _mm_add_ps(*first_start, _mm_mul_ps(_mm_shuffle_ps(first_scale, first_scale, 0), *first_direction));
	result->first_point = first_point;
	__m128 const second_scale = _mm_set_ss(second_fraction);
	__m128 &offset_field = result->offset;
	offset_field = _mm_sub_ps(first_point, _mm_add_ps(*second_start, _mm_mul_ps(_mm_shuffle_ps(second_scale, second_scale, 0), *second_direction)));
	__m128 const squares = _mm_mul_ps(result->offset, result->offset);
	float distance_squared;
	_mm_store_ss(&distance_squared, _mm_add_ss(_mm_shuffle_ps(squares, squares, 0xaa), _mm_add_ss(_mm_shuffle_ps(squares, squares, 0x55), squares)));
	result->distance_squared = distance_squared;
	return flags;
}
