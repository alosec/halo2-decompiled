#include "unknown_120d80.h"
#include <wchar.h>

#ifdef H2_NATIVE_PORT
// @retail 0x120df0
void function_120df0(long index, wchar_t const *name)
{
	wchar_t *profile_name = global_preferences_globals.current.names[index];
	wcsncpy(profile_name, name, 31);
	profile_name[31] = 0;
	global_preferences_globals.dirty = true;
}

/* Native-port split of the recovered function in unknown_120d80.cpp.  This
 * pure preference update does not require the unrelated async/XCalcsig APIs. */
// @retail 0x121040
void function_121040(long value)
{
	if (value != NONE)
	{
		global_preferences_globals.current.unknown170 = value;
		global_preferences_globals.dirty = true;
	}
}
#endif
