// @flags /O2 /GL-
// Havok (third-party physics library): giving a reference-counted object's
// block back to the allocator, with the size the object keeps at +4.
#include "physics_allocator.h"

struct s_3114b0
{
	void *vtable;
	unsigned short allocation_size;
	short reference_count;
};

// @retail 0x3114b0
void __cdecl function_3114b0(s_3114b0 *object)
{
	havok_allocator()->release(object, object->allocation_size, 0x29);
}
