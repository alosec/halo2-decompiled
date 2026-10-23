// @flags /O2 /GL- /GX
// Havok (third-party physics library): building an object in place: its base
// (0x31bc70), a cleared word, and 128 bytes copied from a default table whose
// first word is -1.
#include <new>
#include "growable_array_2d91d0.h"

void *__fastcall function_31bc70(void *arg_0);

static const unsigned long g_412ff0[0x80 / 4] = {0xffffffff};

class class_2fbad0
{
public:
	long field_0;
	unsigned char field_4[0x80];

	class_2fbad0()
	{
		function_31bc70(this);
		field_0 = 0;
		function_2d8db0(field_4, g_412ff0, sizeof(field_4));
	}
};

// @retail 0x2fbad0
void __cdecl function_2fbad0(void *memory)
{
	new (memory) class_2fbad0;
}
