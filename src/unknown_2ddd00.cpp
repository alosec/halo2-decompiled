// @flags /O2 /GL-
// Havok (third-party physics library): setting an object from a
// description: its first value and two vectors, each vector's last word
// taken from the description's second value.
#include <xmmintrin.h>

struct class_2df1f0
{
	char unknown0[8];
	long field_8;

	void function_2df200(long const *value);
};

struct s_2ddd00
{
	long field_0;
	long field_4;
	char field_8[8];
	__m128 field_10;
	__m128 field_20;
};

class class_2ddd00
{
public:
	char field_0[0x10];
	__m128 field_10;
	__m128 field_20;

	void function_2ddd00(s_2ddd00 const *arg_0);
	void function_2ddd30(__m128 const *direction, __m128 *out) const;
};

// @retail 0x2ddd00
void class_2ddd00::function_2ddd00(s_2ddd00 const *arg_0)
{
	((class_2df1f0 *)this)->function_2df200(&arg_0->field_0);
	field_10 = arg_0->field_10;
	((long *)&field_10)[3] = arg_0->field_4;
	field_20 = arg_0->field_20;
	((long *)&field_20)[3] = arg_0->field_4;
}

// the end of the segment furthest along a direction, its last word naming
// which end it is
// @retail 0x2ddd30
void class_2ddd00::function_2ddd30(__m128 const *direction, __m128 *out) const
{
	__m128 const start = field_10;
	__m128 const end = field_20;
	__m128 product = _mm_mul_ps(_mm_sub_ps(end, start), *direction);
	float dot;
	_mm_store_ss(&dot, _mm_add_ss(_mm_shuffle_ps(product, product, 0xaa),
		_mm_add_ss(_mm_shuffle_ps(product, product, 0x55), product)));
	long id;
	if (dot < 0.0f)
	{
		*out = start;
		id = 0;
	}
	else
	{
		*out = end;
		id = 0x10;
	}
	((long *)out)[3] = id | 0x3f000000;
}
