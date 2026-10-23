// @flags /O2 /GL-
// Havok (third-party physics library): places a child object by the transform
// in a description.
#include "placed_child_2dec70.h"

// @retail 0x2dec70
placed_child_2dec70::placed_child_2dec70(child_info_2dec70 const &info) : shape_2de750(info)
{
	transform[0] = info.transform[0];
	transform[1] = info.transform[1];
	transform[2] = info.transform[2];
	transform[3] = info.transform[3];
	child = info.child;
	child->reference_count++;
}
