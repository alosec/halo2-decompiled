// @flags /O2 /GL- /GX
// Havok (third-party physics library): constructing a description (a
// distance of 20 and a second distance of 5) in place.
#include <new>

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

struct description_2dc0e0 : class_2df1a0
{
	long field_8;
	float field_c;
	float field_10;

	description_2dc0e0()
	{
		field_8 = 0;
		field_c = 20.0f;
		field_10 = 5.0f;
	}
};

// @retail 0x2dc0e0
void __cdecl function_2dc0e0(void *memory)
{
	new (memory) description_2dc0e0;
}
