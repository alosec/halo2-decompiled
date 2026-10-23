// @flags /O2 /GL- /GX
// Havok (third-party physics library): creates an object of 0x260 bytes from
// its description, with a fixed size argument of 0x4f4c.
#include "reference_counted_2d88a0.h"

struct s_3111d0;

class c_3101c0
{
public:
	c_3101c0(s_3111d0 const &arg_0, long arg_1);

	REFERENCED_NEW_2D88A0(0x2a)

	char unknown0[0x260];
};

// @retail 0x311140
c_3101c0 *__cdecl function_311140(s_3111d0 const &description)
{
	return new c_3101c0(description, 0x4f4c);
}
