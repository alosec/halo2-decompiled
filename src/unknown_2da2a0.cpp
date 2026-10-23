// @flags /O2 /GL-
// Havok (third-party physics library): creates an object with no fields of
// its own.
#include "reference_counted_2d88a0.h"

class empty_2da2a0 : public referenced_2d88a0
{
public:
	REFERENCED_NEW_2D88A0(0x13)
};

// @retail 0x2da2a0
empty_2da2a0 *__cdecl function_2da2a0()
{
	return new empty_2da2a0();
}
