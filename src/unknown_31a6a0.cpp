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

// the base of 0x31a6b0's object, built from two vectors (not decompiled yet)
class class_3193c0 : public reference_base_318970
{
public:
	class_3193c0(void const *position, void const *orientation);
	// its vtable's slot 8, called directly from the constructor
	void function_319740(unsigned long value);

	char unknown8[0x10 - 8];
	long field_10;
	char unknown14[0x7c - 0x14];
	float field_7c;
	char unknown80[0xd0 - 0x80];
};

#if H2_WHOLE_IMAGE
extern "C" void h2_retail_319740(void *self, float value);
#endif

class class_31a6b0 : public class_3193c0
{
public:
	class_31a6b0(void const *position, void const *orientation);
};

// @retail 0x31a6b0
class_31a6b0::class_31a6b0(void const *position, void const *orientation)
	: class_3193c0(position, orientation)
{
	field_10 = 0;
#if H2_WHOLE_IMAGE
	/* the retail slot takes a float (port/recovered/havok_motion_3193c0.cpp) */
	h2_retail_319740(this, 0.0f);
#else
	function_319740(0);
#endif
	field_7c = 1.0f;
#if H2_WHOLE_IMAGE
	/* retail installs the class's own table last (0x31a6e7) */
	*(unsigned int *)this = 0x4151b0;
#endif
}
