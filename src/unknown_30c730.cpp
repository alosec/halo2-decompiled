// @flags /O2 /GL-
// Havok (third-party physics library): the deleting destructor of a
// reference-counted object that embeds another reference-counted object.
#include "reference_counted_2d88a0.h"

class holder_30c760 : public referenced_2d88a0
{
public:
	REFERENCED_NEW_2D88A0(4)

	referenced_2d88a0 part;

	// implicit in retail; compiled out of line at 0x30c760
	~holder_30c760();
};

// @retail 0x30c730 deleting holder_30c760
