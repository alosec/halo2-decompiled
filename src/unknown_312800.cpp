// @flags /O2 /GL- /GX
// Havok (third-party physics library): an object placed by a transform, built
// from a description: its shape, its bounds not yet set, and the transform.
#include "placed_shape_312800.h"
#include "unknown_1cec30.h"

// @retail 0x312800
placed_shape_312800::placed_shape_312800(description_315ba0 const *description)
	: overlapped_312800((referenced_2d88a0 *)description->field_4, 2)
{
	bounds.clear_312800();
	list.field_0 = description->field_0;
	field_14 = (long)&bounds;
	field_54 = ((shape_312800 *)object)->slot_5();
	((hkTransform &)transform).set((hkTransform const &)description->transform);
}
