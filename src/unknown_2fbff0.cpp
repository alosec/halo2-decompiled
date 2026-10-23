// @flags /O2 /GL- /GX
// Havok (third-party physics library): builds an object with a cleared
// second field in place.
#include <new>

struct class_2df1e0
{
	long field_0;

	class_2df1e0();
};

struct cleared_2fbff0 : class_2df1e0
{
	long field_4;

	cleared_2fbff0() : field_4(0) {}
};

// @retail 0x2fbff0
void __cdecl function_2fbff0(void *memory)
{
	new (memory) cleared_2fbff0();
}
