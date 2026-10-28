// @flags /O2 /GL- /Ob1
// Return the current C-library time without requesting a second output.
#include <time.h>

// @retail 0x3724f0
long __cdecl function_3724f0()
{
	return time(0);
}
