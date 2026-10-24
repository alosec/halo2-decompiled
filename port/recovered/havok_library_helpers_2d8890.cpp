// @flags /O2 /GL-
// Havok (third-party physics library) and C++ library helpers:
//   0x2d8890  rewinds the scratch area's cursor to its start (the timer marks
//             written there since are dropped)
//   0x2d8d80  std::char_traits<char>::length, out of line: the string's length
#include "scratch_2d88a0.h"
#include "string_helpers_2d8c90.h"

// @retail 0x2d8890
void __cdecl function_2d8890()
{
	g_480154 = g_480150;
}

// @retail 0x2d8d80
unsigned int __cdecl string_length_2d8d80(char const *string)
{
	char const *end = string;
	while (*end != 0)
	{
		end++;
	}
	return (unsigned int)(end - string);
}
