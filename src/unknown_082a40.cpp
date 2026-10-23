// @flags /O2 /arch:SSE /Gr
// relevance of one object to the observers, and a value scaled from it
#include "unknown_11c920.h"
#include "unknown_08b110.h"
#include "unknown_0259d0.h"
#include "unknown_19ec40.h"
#include "unknown_0662e0.h"

struct s_relevance_observers;
real function_aa7c0(s_relevance_observers const *observers, point3f const *position, real maximum_distance);

/* the relevance thresholds live in the network configuration */

__declspec(noinline) void __stdcall function_82a40(long object_index, s_relevance_observers const *observers, real *relevance, long *value);

// @retail 0x82a40
void __stdcall function_82a40(long object_index, s_relevance_observers const *observers, real *relevance, long *value)
{
	point3f position;
	function_b9dd0(object_index, &position);
	real result = function_aa7c0(observers, &position, g_network_configuration.realcb8);
	*relevance = result;
	real upper = g_network_configuration.realcbc;
	long near_value = g_network_configuration.valuecc8;
	long far_value = g_network_configuration.valuecc4;
	real lower = g_network_configuration.realcc0;
	if (result >= upper)
	{
		*value = far_value;
		return;
	}
	long scaled;
	if (result <= lower)
		scaled = near_value;
	else
		scaled = (long)((real)near_value - (result - lower) / (upper - lower) * (real)(near_value - far_value));
	*value = scaled;
}
