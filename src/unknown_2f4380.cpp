// @flags /O2 /GL-
// Havok (third-party physics library): a deleting destructor.
#include "reference_base_318970.h"

// an object of the pair-agent kind, a reference-counted object whose
// destructor is the base's: its deleting destructor gives the block back with
// tag 0x1a (in a file of its own: under /GX, which its neighbour needs, the
// compiler calls the class's own destructor instead)
class class_2f4380 : public reference_base_318970
{
public:
	REFERENCED_NEW_2D88A0(0x1a)
};

// @retail 0x2f4380 deleting class_2f4380
