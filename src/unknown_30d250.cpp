// @flags /O2 /GL-
// Havok (third-party physics library): destroys an object built on the
// reference object and two interfaces, and frees it when asked.
#include "reference_counted_2d88a0.h"

class class_30d280;

void __fastcall function_30d280(class_30d280 *object);

class deleting_30d250;

// the reference object, and the two interfaces at 0xc and 0x10; the object
// frees itself through any of the three
class part_a_30d250
{
public:
	virtual deleting_30d250 *function_30d250(unsigned int flags) = 0;

	unsigned short memory_size;
	unsigned short reference_count;
	long field_8;
};

class part_b_30d250
{
public:
	virtual deleting_30d250 *function_30d250(unsigned int flags) = 0;
};

class part_c_30d250
{
public:
	virtual deleting_30d250 *function_30d250(unsigned int flags) = 0;
};

class deleting_30d250 : public part_a_30d250, public part_b_30d250, public part_c_30d250
{
public:
	virtual deleting_30d250 *function_30d250(unsigned int flags);
};

// through the interfaces at 0xc and 0x10
// @retail 0x30d230 adjustor deleting_30d250::function_30d250 0xc
// @retail 0x30d240 adjustor deleting_30d250::function_30d250 0x10

// @retail 0x30d250
deleting_30d250 *deleting_30d250::function_30d250(unsigned int flags)
{
	function_30d280((class_30d280 *)this);
	if (flags & 1)
	{
		g_memory_2d88a0->release_block(this, memory_size, 0x22);
	}
	return this;
}
