// @flags /O2 /GL- /GX
// Havok (third-party physics library): a description with a transform, and
// one built in place in the memory it is given.
#include <new>
#include "placed_shape_312800.h"

// @retail 0x30b6a0
c_contact_query_transform_info::c_contact_query_transform_info()
{
}

// @retail 0x30b6b0
void __cdecl function_30b6b0(void *memory)
{
	new (memory) c_contact_query_transform_info;
}
