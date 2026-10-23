// @flags /O2 /GL-
// Havok (third-party physics library): giving a reference-counted object's
// block back to the allocator, with the size the object keeps at +4.
#include "physics_allocator.h"

struct s_2dae10
{
	void *vtable;
	unsigned short allocation_size;
	short reference_count;
};

// @retail 0x2dae10
void __cdecl function_2dae10(s_2dae10 *object)
{
	havok_allocator()->release(object, object->allocation_size, 0x1a);
}

class class_2dae30
{
public:
	void function_2dae30(long a, long b, long c);
};

// @retail 0x2dae30 standard
void class_2dae30::function_2dae30(long a, long b, long c)
{
}
