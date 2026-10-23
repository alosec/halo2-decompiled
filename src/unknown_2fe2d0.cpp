// @flags /O2 /GL- /GX
// Havok (third-party physics library): constructing a description with a
// vector of (1, 1, 1, 0) in place.
#include <new>
#include <xmmintrin.h>

struct class_2df1e0
{
	long field_0;

	class_2df1e0();
};

struct class_2df1d0 : class_2df1e0
{
	class_2df1d0();
};

struct class_2df1a0 : class_2df1d0
{
	float field_4;

	class_2df1a0();
};

struct description_2fe2d0 : class_2df1a0
{
	__m128 field_10;

	description_2fe2d0()
	{
		field_10.m128_f32[0] = 1.0f;
		field_10.m128_f32[1] = 1.0f;
		field_10.m128_f32[2] = 1.0f;
		field_10.m128_f32[3] = 0.0f;
	}
};

// @retail 0x2fe2d0
void __cdecl function_2fe2d0(void *memory)
{
	new (memory) description_2fe2d0;
}
