// @flags /O2 /GL- /GX
// Havok (third-party physics library): returns its third argument offset
// by 0x80 bytes; the first two arguments (a step and the solver's float
// scale, as the other motions' slot 4 take them) are unused. And the constructor of
// the object whose vtable (0x4151b0) holds it.
#include "reference_base_318970.h"

struct class_31a6a0
{
    char *function_31a6a0(long arg_0, float arg_1, char *base);
};

// @retail 0x31a6a0
char *class_31a6a0::function_31a6a0(long arg_0, float arg_1, char *base)
{
    return base + 0x80;
}

#include "lane_a_motion_leaves.h"

// @retail 0x31a6b0
class_31a6b0::class_31a6b0(void const *position, void const *orientation)
	: class_3193c0(position, orientation)
{
	field_10 = 0;
	function_319740(0);
	field_70.m128_f32[3] = 1.0f;
}
