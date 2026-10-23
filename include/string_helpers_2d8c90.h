#ifndef STRING_HELPERS_2D8C90_H
#define STRING_HELPERS_2D8C90_H
// Havok (third-party physics library): its small wrappers of the C library's
// string and memory routines.
#include <stdarg.h>

// formats into a buffer of `count` bytes, from an argument list (0x2d8c90)
// or from the arguments that follow (0x2d8ca0)
int __cdecl function_2d8c90(char *buffer, unsigned int count, char const *format, va_list arguments);
int __cdecl function_2d8ca0(char *buffer, unsigned int count, char const *format, ...);
// copies at most `count` characters, when there is room for any
void __cdecl function_2d8d60(char *destination, char const *source, unsigned int count);
// the string's length (the C++ library's, out of line; not decompiled)
unsigned int __cdecl string_length_2d8d80(char const *string);
// the last place the character appears in the string, or null
char *__cdecl function_2d8da0(char const *string, int character);
// copies `count` bytes, which may overlap the destination
void __cdecl function_2d8dd0(void *destination, void const *source, unsigned int count);

#endif
