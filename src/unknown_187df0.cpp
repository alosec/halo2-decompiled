// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "globals.h"
#include <string.h>

struct s_player_profile;
struct s_187df0
{
	byte field_0[0x24];
	long field_24;
	byte field_28[0x21c - 0x28];
};

struct __declspec(align(8)) s_187df1
{
	byte field_0[0xfc];
	dword field_fc;
	byte field_100[0x1e0 - 0x100];
};

struct s_187df2
{
	dword field_0;
	byte field_4[0x18 - 4];
	byte field_18[0x1e0];
	long field_1f8;
	byte field_1fc[0xc70 - 0x1fc];
};

void __stdcall player_slot_change_profile(long arg_0, s_player_profile *arg_1, long arg_2);
void function_12b980(void);

#pragma inline_depth(0)
// Retail takes its argument on the stack and returns with ret 4, though
// nothing takes this function's address.
// @retail 0x187df0 standard
void __stdcall function_187df0(bool arg_0)
{
	(void)&arg_0;
	__declspec(align(8)) s_187df1 local_0;
	long local_1 = NONE;
	for (long local_2 = 0; local_2 < 4; local_2++)
	{
		if (g_4e8c20->entries[local_2] != NONE)
		{
			local_1 = local_2;
			break;
		}
	}
	if (local_1 != NONE)
	{
		long local_3 = g_4e8c20->entries[local_1];
		long local_4 = ((s_187df0 *)g_4e8c24->data)[local_3 & 0xffff].field_24;
		if (local_4 != NONE)
		{
			s_187df2 *local_5 = (s_187df2 *)g_54e8e0 + local_4;
			long local_6;
			if (local_5 && (local_5->field_0 & 0x10))
			{
				memcpy(&local_0, local_5->field_18, 0x1e0);
				local_6 = local_5->field_1f8;
			}
			else
			{
				memset(&local_0, 0, 0x1e0);
				local_6 = NONE;
			}
			if (arg_0)
				local_0.field_fc |= 1;
			else
				local_0.field_fc &= ~1;
			player_slot_change_profile(local_4, (s_player_profile *)&local_0, local_6);
			function_12b980();
		}
	}
}
#pragma inline_depth(255)
