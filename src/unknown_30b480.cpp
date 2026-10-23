// @flags /O2 /GL-
// Havok (third-party physics library): the constructor of the axis-aligned
// box volume that contact queries add to the world: an empty collidable
// that owns itself, the box's corners, and an empty list of bodies found.
#include "contact_query_bounds_30b480.h"

// @retail 0x30b480
c_contact_query_bounds_volume::c_contact_query_bounds_volume(c_contact_query_bounds_info const *info)
{
	lower = info->lower;
	upper = info->upper;
	collidable.info.filter = info->filter;
}
