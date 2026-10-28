// @flags /O2 /GL-
// The out-of-line C++ string-length wrapper.
#include <string.h>
#include "string_helpers_2d8c90.h"

// @retail 0x2d8d80
unsigned int __cdecl string_length_2d8d80(char const *string)
{
	return strlen(string);
}
