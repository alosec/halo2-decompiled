// @flags /O2 /GL-
// Havok (third-party physics library): casting a segment against a sphere
// centred on the origin, reporting the first entry point.
#include <math.h>
#include <xmmintrin.h>

static __declspec(align(16)) const float g_ones_412420[4] = {1.0f, 1.0f, 1.0f, 1.0f};

// three-term dot product of two vectors
inline float dot3_2deda0(__m128 const &a, __m128 const &b)
{
	__m128 const product = _mm_mul_ps(a, b);
	float result;
	_mm_store_ss(&result, _mm_add_ss(_mm_shuffle_ps(product, product, 0xaa), _mm_add_ss(_mm_shuffle_ps(product, product, 0x55), product)));
	return result;
}

// the one-byte answer of a cast, returned through a hidden pointer
struct answer_2deda0
{
	char value;

	answer_2deda0(bool value_) : value((char)value_) {}
};

struct segment_2deda0
{
	__m128 start;
	__m128 end;
};

// where a cast hit: the surface normal, the part hit and the fraction of the
// segment travelled
struct hit_2deda0
{
	__m128 normal;
	long part;
	float fraction;
};

class sphere_2deda0
{
public:
	char unknown0[0xc];
	float radius;

	answer_2deda0 function_2deda0(segment_2deda0 const *segment, hit_2deda0 *hit) const;
};

// a long segment is first moved to the point nearest the centre, which keeps
// the quadratic well conditioned; the hit must lie within the segment
// @retail 0x2deda0
answer_2deda0 sphere_2deda0::function_2deda0(segment_2deda0 const *segment, hit_2deda0 *hit) const
{
	float const radius_squared = radius * radius;
	__m128 const direction = _mm_sub_ps(segment->end, segment->start);
	__m128 const approach = _mm_mul_ps(direction, segment->start);
	float shift;
	_mm_store_ss(&shift, _mm_add_ss(_mm_shuffle_ps(approach, approach, 0xaa), _mm_add_ss(_mm_shuffle_ps(approach, approach, 0x55), approach)));
	float half_b = shift;
	if (shift >= 0.0f)
	{
		return answer_2deda0(false);
	}
	float const length_squared = dot3_2deda0(direction, direction);
	__m128 base;
	if (shift * shift > length_squared * radius_squared * 100.0f)
	{
		if (length_squared < radius_squared)
		{
			return answer_2deda0(false);
		}
		shift = -shift;
		__m128 const scale = _mm_set_ps1(shift / length_squared);
		base = _mm_add_ps(_mm_mul_ps(_mm_sub_ps(*(__m128 const *)g_ones_412420, scale), segment->start), _mm_mul_ps(scale, segment->end));
		half_b = 0.0f;
	}
	else
	{
		shift = 0.0f;
		base = segment->start;
	}
	float const discriminant = half_b * half_b - (dot3_2deda0(base, base) - radius_squared) * length_squared;
	if (discriminant <= 0.0f)
	{
		return answer_2deda0(false);
	}
	float const entry = -half_b - sqrtf(discriminant) + shift;
	if (entry >= length_squared)
	{
		return answer_2deda0(false);
	}
	if (entry < 0.0f)
	{
		return answer_2deda0(false);
	}
	float const fraction = entry / length_squared;
	hit->fraction = fraction;
	__m128 const scale = _mm_set_ps1(fraction);
	__m128 const point = _mm_add_ps(_mm_mul_ps(_mm_sub_ps(*(__m128 const *)g_ones_412420, scale), segment->start), _mm_mul_ps(scale, segment->end));
	hit->normal = point;
	__m128 const inverse_radius = _mm_set_ss(1.0f / radius);
	hit->part = -1;
	hit->normal = _mm_mul_ps(_mm_shuffle_ps(inverse_radius, inverse_radius, 0), point);
	return answer_2deda0(true);
}
