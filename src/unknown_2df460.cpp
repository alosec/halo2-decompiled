// @flags /O2 /GL- /GX
// Havok (third-party physics library): constructing a description in place.
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

// @retail 0x2df460
void __cdecl function_2df460(void *memory)
{
	new (memory) class_2df1a0;
}
