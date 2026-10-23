// @flags /O2 /GL-
// Havok (third-party physics library): giving a reference-counted object's
// block back to the allocator, with the size the object keeps at +4.
#include "physics_allocator.h"

struct s_30b620
{
	void *vtable;
	unsigned short allocation_size;
	short reference_count;
};

// @retail 0x30b620
void __cdecl function_30b620(s_30b620 *object)
{
	havok_allocator()->release(object, object->allocation_size, 0x2c);
}
