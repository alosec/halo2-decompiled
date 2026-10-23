// @flags /O2 /GL- /GX
// Havok (third-party physics library): constructing a description in place,
// with two words of 0x80000000 (-0 as floats) among its zeros.
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

struct description_2fd110 : class_2df1a0
{
	unsigned long field_8;
	unsigned long field_c;
	unsigned long field_10;
	unsigned long field_14;
	unsigned long field_18;
	unsigned long field_1c;

	description_2fd110()
	{
		field_8 = 0;
		field_c = 0;
		field_10 = 0x80000000;
		field_14 = 0;
		field_18 = 0;
		field_1c = 0x80000000;
	}
};

// @retail 0x2fd110
void __cdecl function_2fd110(void *memory)
{
	new (memory) description_2fd110;
}
