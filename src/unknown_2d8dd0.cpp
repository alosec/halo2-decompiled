// @flags /O2 /GL-
// Havok (third-party physics library): copies a block of bytes that may
// overlap its destination, through the C library.
#include <string.h>
#include "string_helpers_2d8c90.h"

// @retail 0x2d8dd0
void __cdecl function_2d8dd0(void *destination, void const *source, unsigned int count)
{
	memmove(destination, source, count);
}
