// @flags /O2 /GL-
// Havok (third-party physics library): the deleting destructor of an object
// that adds nothing to destroy to the reference-counted base, giving its block
// back with tag 0x29. Retail calls the destructor out of line (0x31a740, the
// compiler's, in src/unknown_31a740.cpp) rather than inlining it, as it is
// only declared here.
#include "reference_base_318970.h"

class class_31a740 : public reference_base_318970
{
public:
	REFERENCED_NEW_2D88A0(0x29)

	virtual ~class_31a740();
};

// @retail 0x31a710 deleting class_31a740
