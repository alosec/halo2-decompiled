// @flags /O2 /GL- /GX
// Havok (third-party physics library): constructing a description with two
// vectors, the second (1, 0, 0, 0), in place, and copying an object's
// description out as a free function.
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

struct description_2df380 : class_2df1a0
{
	__m128 field_10;
	__m128 field_20;

	description_2df380()
	{
		field_10.m128_f32[0] = 0.0f;
		field_10.m128_f32[1] = 0.0f;
		field_10.m128_f32[2] = 0.0f;
		field_10.m128_f32[3] = 0.0f;
		field_20.m128_f32[0] = 1.0f;
		field_20.m128_f32[1] = 0.0f;
		field_20.m128_f32[2] = 0.0f;
		field_20.m128_f32[3] = 0.0f;
	}
};

// @retail 0x2df380
void __cdecl function_2df380(void *memory)
{
	new (memory) description_2df380;
}

struct s_2fc4e0;

class class_2decc0
{
public:
	void function_2decc0(s_2fc4e0 *out) const;
};

// @retail 0x2df3e0
void __cdecl function_2df3e0(class_2decc0 const *object, s_2fc4e0 *out)
{
	object->function_2decc0(out);
}
