// @flags /O2 /GL-
// Havok (third-party physics library): formatting into a buffer of a given
// size, through the C library.
#include <stdio.h>
#include "string_helpers_2d8c90.h"

// @retail 0x2d8c90
int __cdecl function_2d8c90(char *buffer, unsigned int count, char const *format, va_list arguments)
{
	return _vsnprintf(buffer, count, format, arguments);
}

// @retail 0x2d8ca0
int __cdecl function_2d8ca0(char *buffer, unsigned int count, char const *format, ...)
{
	va_list arguments;
	va_start(arguments, format);
	return _vsnprintf(buffer, count, format, arguments);
}
