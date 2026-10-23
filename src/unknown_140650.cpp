// @flags /O2 /Gr /arch:SSE
/* UNKNOWN_140650.CPP: UTF-16 to UTF-8 string conversion. Kept apart from
   unknown_13fd90.cpp because retail's clamp is a cmov, which needs /arch:SSE. */

#include "unknown_11c920.h"
#include "unknown_13fd90.h"

long utf8_encode_character(dword value, byte *buffer, long buffer_size);

// @retail 0x140650
void utf16_string_to_utf8_string(const word *source, char *destination, long destination_size)
{
	long destination_index = 0;

	for (long i = 0; source[i]; i++)
	{
		dword character = source[i];
		long remaining = destination_size - destination_index;
		long length = utf8_encode_character(character, (byte *)destination + destination_index, remaining);

		destination_index += length <= remaining ? length : remaining;
	}

	if (destination_index < destination_size)
		destination[destination_index] = 0;
	else if (destination_index > 0)
		destination[destination_index - 1] = 0;
}
