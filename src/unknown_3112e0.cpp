// @flags /O2 /GL- /GX
// Havok (third-party physics library): creates an object of 0xa0 bytes from
// its description.
#include "reference_counted_2d88a0.h"

struct s_311340;

class c_311ba0
{
public:
	c_311ba0(const s_311340 *arg_0);

	REFERENCED_NEW_2D88A0(0x28)

	char unknown0[0xa0];
};

// @retail 0x3112e0
c_311ba0 *__cdecl function_3112e0(const s_311340 *description)
{
	return new c_311ba0(description);
}
