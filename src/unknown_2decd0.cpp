// @flags /O2 /GL-
// Havok (third-party physics library): a descriptor accessor, vectors
// cleared and a bounding box around a centre.
#include <xmmintrin.h>

struct descriptor_2decd0
{
	long kind;
	bool flag;
	long stride;

	descriptor_2decd0(long kind_, long stride_, bool flag_)
	{
		kind = kind_;
		stride = stride_;
		flag = flag_;
	}
};

struct s_2decd0_body
{
	char unknown0[0x30];
	__m128 centre;
};

class class_2decd0
{
public:
	long field_0;
	long field_4;
	long field_8;
	float radius;

	descriptor_2decd0 get_descriptor() const;
	void function_2decf0(long a, __m128 *out) const;
	void function_2ded10(long a, int count, __m128 *out) const;
	void function_2ded40(__m128 *out) const;
	void get_bounds(const s_2decd0_body *body, float tolerance, __m128 *out) const;
};

// @retail 0x2decd0
descriptor_2decd0 class_2decd0::get_descriptor() const
{
	return descriptor_2decd0(1, 0x10, true);
}

// @retail 0x2decf0
void class_2decd0::function_2decf0(long a, __m128 *out) const
{
	*out = _mm_setzero_ps();
}

// @retail 0x2ded10
void class_2decd0::function_2ded10(long a, int count, __m128 *out) const
{
	for (int i = count - 1; i >= 0; i--)
	{
		*out = _mm_setzero_ps();
		out++;
	}
}

// @retail 0x2ded40
void class_2decd0::function_2ded40(__m128 *out) const
{
	*out = _mm_setzero_ps();
}

// @retail 0x2ded60
void class_2decd0::get_bounds(const s_2decd0_body *body, float tolerance, __m128 *out) const
{
	__m128 extent = _mm_set_ps1(tolerance + radius);
	out[0] = _mm_sub_ps(body->centre, extent);
	out[1] = _mm_add_ps(body->centre, extent);
}
