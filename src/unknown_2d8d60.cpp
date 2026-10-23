// @flags /O2 /GL-
// Havok (third-party physics library): copies at most a given number of
// characters, when there is room for any.
#include <string.h>
#include "string_helpers_2d8c90.h"

// @retail 0x2d8d60
void __cdecl function_2d8d60(char *destination, char const *source, unsigned int count)
{
	if (count)
	{
		strncpy(destination, source, count);
	}
}
