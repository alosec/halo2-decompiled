/* UNKNOWN_080E10.H: function_12d2f0 retried until it succeeds or a time
   limit passes, as 0x80e10, 0x862e0 and 0x8c790 expand it inline (lane D) */

#ifndef UNKNOWN_080E10_H
#define UNKNOWN_080E10_H

#include "physical_memory.h"
#include <xtl.h>
#include <d3d8.h>

extern s_physical_object *g_4e6464;
long __stdcall function_12d2f0(long size, long user_data, long update, long release);
void function_12c600(void);
double timing_ticks_to_seconds(__int64 ticks);

/* the time stamp counter */
static __forceinline __int64 function_80e13(void)
{
#if H2_NATIVE_PORT
	return h2_native_time_stamp();
#else
	volatile __int64 local_0 = 0;
	__asm rdtsc
#endif
}

/* at most maximum_pumps calls of function_12c600 between the attempts, then
   until timeout seconds have passed since the first */
static __forceinline long function_80e14(long size, long user_data, long update, long release,
	long maximum_pumps, real timeout)
{
	__int64 start = function_80e13();
	long result = 0;
	if (size > 0)
	{
		long page_count = g_4e6464->page_count;
		if (page_count > 0)
		{
			long pumps = 0;
			do
			{
				result = function_12d2f0(size, user_data, update, release);
				if (result == 0)
				{
					if (pumps < maximum_pumps)
					{
						pumps++;
						function_12c600();
					}
					else
					{
						__int64 elapsed = function_80e13() - start;
						if (elapsed < 0)
							elapsed = 0;
						if (!((real)timing_ticks_to_seconds(elapsed) < timeout))
							break;
						D3DDevice_KickPushBuffer();
						D3DDevice_IsBusy();
						SwitchToThread();
					}
				}
			} while (result == 0);
		}
	}
	return result;
}

#endif
