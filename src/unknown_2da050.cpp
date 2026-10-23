// @flags /O2 /GL-
// Havok (third-party physics library): giving a reference-counted object's
// block back to the allocator, with the size the object keeps at +4.
#include "physics_allocator.h"

struct s_2da050
{
	void *vtable;
	unsigned short allocation_size;
	short reference_count;
};

// @retail 0x2da050
void __cdecl function_2da050(s_2da050 *object)
{
	havok_allocator()->release(object, object->allocation_size, 0x19);
}
