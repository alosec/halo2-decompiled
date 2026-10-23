// @flags /O2 /GL- /GX
// Havok (third-party physics library): creating the box volume of 0x30b480.
#include "contact_query_bounds_30b480.h"

// @retail 0x3122b0
c_contact_query_bounds_volume *__cdecl function_3122b0(c_contact_query_bounds_info const *info)
{
	return new c_contact_query_bounds_volume(info);
}
