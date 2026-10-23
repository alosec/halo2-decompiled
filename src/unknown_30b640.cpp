// @flags /O2 /GL- /GX
// Havok (third-party physics library): creates a transform contact-query
// volume from its description.
#include "contact_volume_30bb50.h"

// @retail 0x30b640
c_contact_query_transform_volume *__cdecl function_30b640(c_contact_query_transform_info const *info)
{
	return new c_contact_query_transform_volume(info);
}
