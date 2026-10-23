// @flags /O2 /Gr
/* STRING_APPEND.CPP: appending to a 256-character string */

#include "unknown_11c920.h"
#include <string.h>

/* appends suffix to a string of at most 0x100 characters, keeping it
   terminated */
// @retail 0x122810
char *function_122810(char *string, const char *suffix)
{
	const char *cursor = string;
	dword length;

	for (length = 0; length < 0x100; length++)
	{
		if (!*cursor++)
		{
			break;
		}
	}
	strncpy(string + length, suffix, 0x100 - length);
	string[0xff] = 0;
	return string;
}
