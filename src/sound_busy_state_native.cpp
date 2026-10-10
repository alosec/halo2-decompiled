#include "main_globals.h"

#ifdef H2_NATIVE_PORT

/* Exact recovered busy-state query beneath the sound master-fade policy.  It
 * reads only genuine main-loop, async texture, and geometry-cache state. */

extern bool g_4ed39d;
extern long g_4e6470;
extern long g_4e64a0;

// @retail 0x12be90
bool function_12be90(void)
{
	bool result = false;

	if (g_4ed39d)
		result = true;
	if (main_globals.switch_structure_bsp || main_globals.reset_map ||
		main_globals.unknown6f || main_globals.unknown72 ||
		main_globals.save_map || main_globals.quit_game ||
		main_globals.unknown76 || g_4e6470 > 0 || g_4e64a0 > 0)
	{
		result = true;
	}
	return result;
}

#endif
