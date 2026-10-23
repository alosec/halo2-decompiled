// @flags /O2 /GL- /GX
// Havok (third-party physics library): constructing, in place, an object
// holding an empty array.
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

// an array whose storage starts empty and unowned (top bit of the capacity)
struct array_2fc5b0
{
	void *data;
	long size;
	unsigned long capacity_and_flags;

	array_2fc5b0() : data(0), size(0), capacity_and_flags(0x80000000) {}
};

struct array_holder_2fc5b0 : class_2df1d0
{
	array_2fc5b0 items;
};

// @retail 0x2fc5b0
void __cdecl function_2fc5b0(void *memory)
{
	new (memory) array_holder_2fc5b0();
}
