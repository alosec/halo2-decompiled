// @flags /O2 /Gr
/* SCENARIO_RAMP_GLOBALS.CPP: the interpolators' state (g_4e6740) through the
   game's lifecycle: allocated once, cleared for each new map */

#include "unknown_11c920.h"
#include "unknown_123b30.h"
#include "scenario_ramps.h"
#include <string.h>

s_interpolator_globals *g_4e6740;

/* allocates the interpolators' state */
// @retail 0x134d90
void scenario_ramps_initialize(void)
{
	g_4e6740 = (s_interpolator_globals *)function_123d40("unknown", "unknown", sizeof(s_interpolator_globals));
}

// @retail 0x134dd0
void scenario_ramps_dispose(void)
{
	g_4e6740 = 0;
}

/* clears every interpolator's state */
// @retail 0x134de0
void scenario_ramps_reset_for_map(void)
{
	if (g_4e6740)
	{
		memset(g_4e6740, 0, sizeof(*g_4e6740));
	}
}
