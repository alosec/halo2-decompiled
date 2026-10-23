// @flags /O2 /Gr /arch:SSE
#include "unknown_11c920.h"
#include "globals.h"

struct s_swarm_spawn_slot;
extern s_swarm_spawn_slot *g_5044c4;

struct s_290250
{
	byte field_0[0x4c];
	short field_4c;
	byte field_4e[2];
};

void __stdcall function_2902b0(long arg_0, long arg_1, long arg_2, long arg_3,
	real arg_4, real arg_5, transform4x3f const *arg_6, short arg_7);

__forceinline short function_290251()
{
	short local_0;
	for (local_0 = 0; local_0 < 5; local_0++)
	{
		if (((s_290250 *)g_5044c4)[local_0].field_4c <= 0)
			return local_0;
	}
	return NONE;
}

// @retail 0x290250
void __stdcall function_290250(long arg_0, long arg_1, long arg_2, long arg_3,
	real arg_4, real arg_5, transform4x3f const *arg_6)
{
	short local_0 = function_290251();
	if (local_0 != NONE)
		function_2902b0(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6, local_0);
}
