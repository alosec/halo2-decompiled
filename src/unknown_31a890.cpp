// @flags /O2 /GL-
// Havok (third-party physics library): a deleting destructor.
#include "reference_base_318970.h"

// an object, a reference-counted object whose destructor is the base's: its
// deleting destructor gives the block back with tag 0x29
class class_31a890 : public reference_base_318970
{
public:
	REFERENCED_NEW_2D88A0(0x29)
};

// @retail 0x31a890 deleting class_31a890
