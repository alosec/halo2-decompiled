// @flags /O2 /GL-
// Havok (third-party physics library): returns a 16-bit zero and ignores
// its four arguments.
#include "reference_base_318970.h"

struct class_30c3a0
{
    short function_30c3a0(long arg_0, long arg_1, long arg_2, long arg_3);
};

// @retail 0x30c3a0
short class_30c3a0::function_30c3a0(long arg_0, long arg_1, long arg_2, long arg_3)
{
    return 0;
}

// an object, a reference-counted object whose destructor is the base's: its
// deleting destructor gives the block back with tag 0x1d
class class_30c3b0 : public reference_base_318970
{
public:
	REFERENCED_NEW_2D88A0(0x1d)
};

// @retail 0x30c3b0 deleting class_30c3b0
