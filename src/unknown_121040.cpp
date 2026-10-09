#include "unknown_120d80.h"

#ifdef H2_NATIVE_PORT
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
