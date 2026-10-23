// @flags /O2 /GL-
// Havok (third-party physics library): cleared matrices and copying a
// source's state.
#include <xmmintrin.h>

inline void set_zero_31a750(__m128 *rows, __m128 zero)
{
	rows[0] = zero;
	rows[1] = zero;
	rows[2] = zero;
}

struct s_31a770
{
	char kind;
	char unknown1[0xf];
	__m128 rows[7];
};

class class_31a750_source
{
public:
	long field_0;
	long field_4;
	long field_8;
	long field_c;
	long field_10[4];
	__m128 field_20;
	char unknown30[0x30];
	__m128 field_60;
	__m128 field_70;
};

class class_31a750
{
public:
	long field_0;
	long field_4;
	long field_8;
	long field_c;
	char unknown10[0x10];
	__m128 field_20;
	char unknown30[0x30];
	__m128 field_60;
	__m128 field_70;
	char unknown80[0x40];
	class_31a750_source *source;

	void function_31a750(__m128 *rows) const;
	char *function_31a770(long a, long b, s_31a770 *out) const;
	void set_source(class_31a750_source *new_source);
};

// @retail 0x31a750
void class_31a750::function_31a750(__m128 *rows) const
{
	__m128 zero = _mm_setzero_ps();
	_mm_store_ps((float *)&rows[0], zero);
	_mm_store_ps((float *)&rows[1], zero);
	_mm_store_ps((float *)&rows[2], zero);
}

// clears the state: the kind, then the seven rows of zeros, the third row
// first, as retail has them
// @retail 0x31a770
char *class_31a750::function_31a770(long a, long b, s_31a770 *out) const
{
	__m128 zero = _mm_setzero_ps();
	out->kind = 2;
	out->rows[2] = zero;
	out->rows[0] = zero;
	out->rows[1] = zero;
	set_zero_31a750(&out->rows[3], zero);
	out->rows[6] = zero;
	return (char *)out + 0x80;
}

// @retail 0x31a7b0
void class_31a750::set_source(class_31a750_source *new_source)
{
	source = new_source;
	if (new_source)
	{
		field_c = new_source->field_c;
		field_20 = new_source->field_20;
		field_20.m128_f32[3] = 0.0f;
		field_70 = new_source->field_70;
		field_60 = new_source->field_60;
	}
}
