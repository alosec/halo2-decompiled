// @flags /O2 /GL-
// Havok (third-party physics library): the implicit destructor of a
// reference-counted object that embeds another reference-counted object.
#include "reference_counted_2d88a0.h"

class holder_30c760 : public referenced_2d88a0
{
public:
	referenced_2d88a0 part;
};

// @retail 0x30c760 destructor holder_30c760
