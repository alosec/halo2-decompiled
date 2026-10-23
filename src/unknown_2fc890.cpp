// @flags /O2 /GL-
// Havok (third-party physics library): the bounding box of a set of spheres
// (centre in the first three floats, radius in the fourth) placed by a
// transform.
#include <xmmintrin.h>

struct transform_2fc890
{
	__m128 axes[3];
	__m128 position;
};

struct bounds_2fc890
{
	__m128 lower;
	__m128 upper;
};

class spheres_2fc890
{
public:
	long field_0;
	long field_4;
	long field_8;
	long count;
	__m128 spheres[8];

	void function_2fc890(transform_2fc890 const *transform, float tolerance, bounds_2fc890 *out) const;
};

// every centre placed by the transform, the box around the spheres there,
// grown by the tolerance
// @retail 0x2fc890
void spheres_2fc890::function_2fc890(transform_2fc890 const *transform, float tolerance, bounds_2fc890 *out) const
{
	__m128 centres[8];
	__m128 const *sphere = spheres;
	__m128 *centre = centres;
	for (int i = count - 1; i >= 0; i--)
	{
		__m128 const p = *sphere;
		*centre = _mm_add_ps(_mm_add_ps(
			_mm_mul_ps(transform->axes[0], _mm_shuffle_ps(p, p, 0x00)),
			_mm_mul_ps(transform->axes[1], _mm_shuffle_ps(p, p, 0x55))),
			_mm_add_ps(_mm_mul_ps(transform->axes[2], _mm_shuffle_ps(p, p, 0xaa)), transform->position));
		sphere++;
		centre++;
	}
	__m128 lower = _mm_set1_ps(3.40282347e+38f);
	__m128 upper = _mm_set1_ps(-3.40282347e+38f);
	for (int j = 0; j < count; j++)
	{
		__m128 const radius = _mm_shuffle_ps(spheres[j], spheres[j], 0xff);
		lower = _mm_min_ps(lower, _mm_sub_ps(centres[j], radius));
		upper = _mm_max_ps(upper, _mm_add_ps(centres[j], radius));
	}
	__m128 const grow = _mm_set1_ps(tolerance);
	out->lower = _mm_sub_ps(lower, grow);
	out->upper = _mm_add_ps(upper, grow);
}
