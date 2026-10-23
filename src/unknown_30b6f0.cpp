// @flags /O2 /GL-
#include "listened_object_315950.h"
// Havok (third-party physics library): calls a game object's method as a
// free function.

struct s_1c3d00
{
	unsigned char field_0[0xc];
	void *field_c;
	long field_10;
	unsigned long field_14;

	void function_1c3d00();
};

// @retail 0x30b6f0
void __cdecl function_30b6f0(s_1c3d00 *object)
{
	object->function_1c3d00();
}

// an object over the listened object that adds nothing to destroy
class class_30b700 : public class_315950
{
};

// its destructor, which is the listened object's
// @retail 0x30b700 destructor class_30b700
