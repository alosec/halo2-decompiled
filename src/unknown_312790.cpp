// @flags /O2 /GL-
// Havok (third-party physics library): an object placed by a transform set
// from a description: the settings of its base, then the transform when it
// moved by more than a small tolerance.
#include "placed_shape_312800.h"
#include "unknown_1cec30.h"

// @retail 0x312790
void placed_shape_312800::function_312790(description_315ba0 const *description)
{
	((class_312430 *)this)->function_3124c0(description);
	if (!description->transform.equal_within(transform, 1e-4f).value)
	{
		((hkRigidBody *)this)->pose_write((hkTransform const &)description->transform);
	}
}
