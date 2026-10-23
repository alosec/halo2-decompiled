// @flags /O2 /GL-
// Havok (third-party physics library): a test of a ray against a plane-like
// record and two limits.
#include <xmmintrin.h>

struct answer_2f97e0
{
	char value;

	answer_2f97e0(bool value_) : value((char)value_) {}
};

// a start point and a direction
struct ray_2f97e0
{
	__m128 start;
	__m128 direction;
};

// a point whose fourth float is a weight, and a direction
struct surface_2f97e0
{
	__m128 point;
	__m128 direction;
};

struct limits_2f97e0
{
	char unknown0[0xc];
	float distance;
	float alignment;
};

class ray_test_2f97e0
{
public:
	answer_2f97e0 function_2f97e0(ray_2f97e0 const *ray, surface_2f97e0 const *surface, limits_2f97e0 const *limits);
};

// three-term dot product of two vectors
inline float dot3_2f97e0(__m128 const &a, __m128 const &b)
{
	__m128 const product = _mm_mul_ps(a, b);
	float result;
	_mm_store_ss(&result, _mm_add_ss(_mm_shuffle_ps(product, product, 0xaa), _mm_add_ss(_mm_shuffle_ps(product, product, 0x55), product)));
	return result;
}

// false only when the surface's weight is at least -0.1, the two directions
// align less than the limit, and the surface lies no further along the ray
// than the distance limit
// @retail 0x2f97e0
answer_2f97e0 ray_test_2f97e0::function_2f97e0(ray_2f97e0 const *ray, surface_2f97e0 const *surface, limits_2f97e0 const *limits)
{
	if (surface->point.m128_f32[3] < -0.1f)
	{
		return true;
	}
	__m128 const direction = ray->direction;
	if (dot3_2f97e0(direction, surface->direction) < limits->alignment)
	{
		if (dot3_2f97e0(_mm_sub_ps(surface->point, ray->start), direction) <= limits->distance)
		{
			return false;
		}
	}
	return true;
}
