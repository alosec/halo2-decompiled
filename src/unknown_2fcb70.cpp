// @flags /O2 /GL-
// Havok (third-party physics library): a triangle's description out (the
// base's values, then its three corners), and its bounding box under a
// transform, grown by its radius plus a tolerance.
#include <xmmintrin.h>

struct s_2fc4e0
{
	long field_0;
	long field_4;
};

class class_2fc4e0
{
public:
	char field_0[0xc];
	// copied out as the description's second value; a radius here
	float radius;

	void function_2fc4e0(s_2fc4e0 *out) const;
};

struct s_2fcb70 : s_2fc4e0
{
	char field_8[8];
	__m128 corners[3];
};

struct c_transformed_point
{
	__m128 value;
	void transform(const void *matrix, const __m128 *point);
};

struct vector_2fcba0
{
	__m128 value;

	void add(__m128 const &other)
	{
		value = _mm_add_ps(value, other);
	}
};

struct box_2fcba0
{
	__m128 minimum;
	vector_2fcba0 maximum;
};

class triangle_2fcb70 : public class_2fc4e0
{
public:
	__m128 corners[3];

	void function_2fcb70(s_2fcb70 *out) const;
	void function_2fcba0(const void *transform, float tolerance, box_2fcba0 *out) const;
};

// @retail 0x2fcb70
void triangle_2fcb70::function_2fcb70(s_2fcb70 *out) const
{
	function_2fc4e0(out);
	out->corners[0] = corners[0];
	out->corners[1] = corners[1];
	out->corners[2] = corners[2];
}

// @retail 0x2fcba0
void triangle_2fcb70::function_2fcba0(const void *transform, float tolerance, box_2fcba0 *out) const
{
	c_transformed_point a;
	__m128 grow = _mm_set1_ps(tolerance + radius);
	c_transformed_point b;
	c_transformed_point c;
	a.transform(transform, &corners[0]);
	b.transform(transform, &corners[1]);
	c.transform(transform, &corners[2]);
	__m128 minimum = _mm_min_ps(_mm_min_ps(a.value, b.value), c.value);
	__m128 maximum = _mm_max_ps(_mm_max_ps(a.value, b.value), c.value);
	out->maximum.value = maximum;
	out->minimum = _mm_sub_ps(minimum, grow);
	out->maximum.add(grow);
}
