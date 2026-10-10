#include "physical_memory.h"

#ifdef H2_NATIVE_PORT

/* Exact recovered sound physical-cache frame advance.  Storage initialization
 * and wrap-time rebasing remain explicit lifecycle dependencies. */

extern void *g_50210c;

// @retail 0x218810
void sound_cache_new_frame(void)
{
	physical_memory_new_frame(static_cast<s_physical_object *>(g_50210c));
}

#endif
