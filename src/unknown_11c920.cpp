// @flags /O2 /Gr
/* UNKNOWN_11C920.CPP: case-insensitive string comparison.
   See docs/unknown_11c920.md for the symbol mapping. Existing bounded-length and
   formatting helpers remain in their upstream translation units. */

#include "unknown_11c920.h"
#include <wctype.h>

// @retail 0x11c920
int function_11c920(char const *s1, char const *s2)
{
	int c1 = towlower(*s1);
	int c2 = towlower(*s2);
	if (c1)
	{
		dword local_0 = (dword)s1 - (dword)s2;
		do
		{
			if (!c2)
				return c1 != 0;
			if (c1 != c2)
				return c1 > c2 ? 1 : -1;
			++s2;
			c1 = towlower(*(char const *)((dword)s2 + local_0));
			c2 = towlower(*s2);
		} while (c1);
	}
	return c2 ? -1 : 0;
}
