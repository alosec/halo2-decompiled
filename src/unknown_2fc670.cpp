// @flags /O2 /GL-
// Havok (third-party physics library): casting a segment against a set of
// spheres, keeping the nearest entry point.
#include <math.h>
#include <xmmintrin.h>

static __declspec(align(16)) const float g_ones_412420[4] = {1.0f, 1.0f, 1.0f, 1.0f};

// three-term dot product of two vectors
inline float dot3_2fc670(__m128 const &a, __m128 const &b)
{
	__m128 const product = _mm_mul_ps(a, b);
	float result;
	_mm_store_ss(&result, _mm_add_ss(_mm_shuffle_ps(product, product, 0xaa), _mm_add_ss(_mm_shuffle_ps(product, product, 0x55), product)));
	return result;
}

// the one-byte answer of a cast, returned through a hidden pointer
struct answer_2fc670
{
	char value;

	answer_2fc670(bool value_) : value((char)value_) {}
};

struct segment_2fc670
{
	__m128 start;
	__m128 end;
};

// where a cast hit: the surface normal, the sphere hit and the fraction of
// the segment travelled
struct hit_2fc670
{
	__m128 normal;
	long part;
	float fraction;
};

// a sphere: its centre, with the radius as the fourth term
struct sphere_2fc670
{
	float centre[3];
	float radius;
};

class spheres_2fc670
{
public:
	char unknown0[0xc];
	int sphere_count;
	sphere_2fc670 spheres[1];

	answer_2fc670 function_2fc670(segment_2fc670 const *segment, hit_2fc670 *hit) const;
};

// only a hit nearer than the one already in the hit record is kept
// @retail 0x2fc670
answer_2fc670 spheres_2fc670::function_2fc670(segment_2fc670 const *segment, hit_2fc670 *hit) const
{
	bool found = false;
	for (int i = 0; i < sphere_count; i++)
	{
		__m128 const centre = *(__m128 const *)&spheres[i];
		__m128 const end = _mm_sub_ps(segment->end, centre);
		__m128 const start = _mm_sub_ps(segment->start, centre);
		__m128 const direction = _mm_sub_ps(end, start);
		float const start_distance_squared = dot3_2fc670(start, start);
		float const b = dot3_2fc670(direction, start) * 2.0f;
		if (b >= 0.0f)
		{
			continue;
		}
		float const length_squared = dot3_2fc670(direction, direction);
		float const discriminant = b * b - (start_distance_squared - spheres[i].radius * spheres[i].radius) * length_squared * 4.0f;
		if (discriminant <= 0.0f)
		{
			continue;
		}
		float const entry = (-b - sqrtf(discriminant)) * 0.5f;
		if (entry >= length_squared)
		{
			continue;
		}
		if (entry < 0.0f)
		{
			continue;
		}
		float const fraction = entry / length_squared;
		if (fraction < hit->fraction)
		{
			hit->fraction = fraction;
			found = true;
			__m128 const scale = _mm_set_ps1(fraction);
			__m128 const point = _mm_add_ps(_mm_mul_ps(_mm_sub_ps(*(__m128 const *)g_ones_412420, scale), start), _mm_mul_ps(scale, end));
			hit->normal = point;
			__m128 const inverse_radius = _mm_set_ss(1.0f / spheres[i].radius);
			hit->part = i;
			hit->normal = _mm_mul_ps(_mm_shuffle_ps(inverse_radius, inverse_radius, 0), point);
		}
	}
	return answer_2fc670(found);
}
