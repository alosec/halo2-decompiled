// @flags /O2 /GL- /GX
// Havok (third-party physics library): a description with bounds, zero when
// built, and one built in place in the memory it is given.
#include <new>
#include "placed_shape_312800.h"

// @retail 0x312310
c_contact_query_bounds_info::c_contact_query_bounds_info()
{
	bounds.minimum = _mm_setzero_ps();
	bounds.maximum = _mm_setzero_ps();
}

// @retail 0x312330
void __cdecl function_312330(void *memory)
{
	new (memory) c_contact_query_bounds_info;
}
