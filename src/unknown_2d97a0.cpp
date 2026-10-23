// @flags /O2 /GL- /GX
// Havok (third-party physics library): the deleting destructor of an object
// whose destructor is its base's; its block goes back with tag 0x19.
#include "unknown_0a9f20.h"
#include "reference_counted_2d88a0.h"

class class_2d97a0 : public c_reference_a9f20
{
public:
	REFERENCED_NEW_2D88A0(0x19)
};

// @retail 0x2d97a0 deleting class_2d97a0
