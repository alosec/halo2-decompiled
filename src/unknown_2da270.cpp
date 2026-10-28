// @flags /O2 /GL-
// Havok (third-party physics library): a deleting destructor.
#include "reference_base_318970.h"

// an object, a reference-counted object whose destructor is the base's: its
// deleting destructor gives the block back with tag 0x13
class class_2da270 : public reference_base_318970
{
public:
	REFERENCED_NEW_2D88A0(0x13)
};

// @retail 0x2da270 deleting class_2da270


// @retail 0x2d7f40
class_2da270 *__cdecl function_2d7f40()
{
	return new class_2da270;
}
