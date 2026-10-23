// @flags /O2 /GL-
// Havok (third-party physics library): the last place a character appears in
// a string, through the C library.
#include <string.h>
#include "string_helpers_2d8c90.h"

// @retail 0x2d8da0
char *__cdecl function_2d8da0(char const *string, int character)
{
	return strrchr(string, character);
}
