// @flags /O2 /Ob1 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "unknown_075870.h"
#include "globals.h"

extern short g_485ac0;
extern s_connection_counter g_4e6398;
real g_4cf5d0;

bool network_observer_channel_has_host(s_network_observer *arg_0, long arg_1);
bool function_76520(long arg_0, bool arg_1, long *arg_2);
long __stdcall function_765c0(long arg_0, long arg_1, byte *arg_2);
long link_connection_get_reliable_window(s_link_connection *arg_0);
long link_connection_get_reliable_bytes(s_link_connection *arg_0);
long link_connection_get_reliable_pending(s_link_connection *arg_0);
long link_connection_owner_active(s_link_connection *arg_0);

struct s_75fc0
{
	byte field_0[0x518];
	long field_470;
	byte field_474[4];
	__int64 field_478;
	long field_480;
	real field_484;
	long field_488;
	bool field_48c;
	bool field_48d;
	bool field_48e;
	byte field_48f[5];
	long field_494;
	long field_498;
	real field_49c;
	bool field_4a0;
	bool field_4a1;
	bool field_4a2;
	byte field_4a3[0x4dc - 0x4a3];
	long field_4dc;
};

union s_75fc2
{
	s_connection_counter field_0;
	__int64 field_8;
};

static __forceinline long function_75fc1(real arg_0)
{
	long local_0;
	__asm
	{
		fld arg_0
		fistp local_0
	}
	return local_0;
}

#pragma optimize("g", off)
static __forceinline long function_75fc3(s_network_observer *arg_0, long arg_1)
{
    long local_0 = NONE;
    for (long local_1 = 0; local_1 < MAXIMUM_OBSERVER_CHANNELS; local_1++)
    {
        s_network_observer_channel *local_2 = &arg_0->channels[local_1];
        if (local_2->state && local_2->connection_index == arg_1)
        {
            local_0 = local_1;
            break;
        }
    }
    return local_0;
}
#pragma optimize("", on)

// @retail 0x75fc0
bool s_network_observer::connection_v05(long arg_1,
	bool arg_2, bool arg_3, bool arg_4, bool *arg_5, bool *arg_6,
	long *arg_7, long *arg_8, long arg_9, byte *arg_10)
{
	s_network_observer *arg_0 = this;
	long local_0 = function_75fc3(arg_0, arg_1);
	bool local_2 = false;
	if (local_0 == NONE)
		return local_2;
	s_link_connection *local_3 = link_connection_get(arg_1);
	s_75fc0 *local_4 = (s_75fc0 *)((byte *)arg_0 + local_0 * sizeof(arg_0->channels[0]));
	long local_5 = NONE;
	long local_6 = NONE;
	bool local_7 = false;
	bool local_8 = network_observer_channel_has_host(arg_0, local_0);
	if (local_8)
	{
		arg_3 = true;
		arg_4 = true;
	}
	long local_9;
	bool local_10 = function_76520(local_0, false, &local_9);
	long local_11 = g_510548 ? g_51054c : GetTickCount();
	s_75fc2 local_12;
	local_12.field_0 = g_4e6398;
	long local_13 = NONE;
	bool local_14 = (local_3->callback || local_10) && !local_8;
	bool local_15 = local_4->field_48c;
	bool local_16 = false;
	real local_17 = -1.0f;
	long local_18 = NONE;
	if (local_15 && !local_8)
	{
		if (local_4->field_48d && local_4->field_4dc == 2)
		{
			local_16 = true;
			local_14 = true;
		}
		else if (local_4->field_48e)
			local_16 = true;
	}
	if (!local_8)
	{
		if (local_14)
		{
			if (local_15)
				local_17 = local_4->field_49c;
			else if (local_3->callback)
				local_17 = local_4->field_484;
			else
				local_17 = g_4cf5d0;
		}
		else if (!arg_2)
			local_17 = arg_3 ? 4.0f : 0.0f;
	}
	if (local_3->state == 5 && !local_8)
	{
		long local_19 = (local_3->flags & 8) ? 128 : 0;
		long local_20 = link_connection_get_reliable_window(local_3);
		real local_21 = (real)local_20 /
			((real)local_19 * *(real *)((byte *)arg_0->configuration + 0x90));
		if (local_21 < 1.0f)
		{
			real local_22 = *(real *)((byte *)arg_0->configuration + 0x94) +
				(1.0f - local_21) * (*(real *)((byte *)arg_0->configuration + 0x98) -
				*(real *)((byte *)arg_0->configuration + 0x94));
			if (local_17 <= 0.0f || 1.0f / local_22 <= local_17)
				local_17 = 1.0f / local_22;
		}
	}
	if (local_14)
	{
		local_18 = local_15 ? local_4->field_494 : local_4->field_480;
		local_5 = local_15 ? local_4->field_498 : local_4->field_488;
		arg_4 = true;
	}
	long local_23 = g_4e6398.low - (long)local_4->field_478;
	long local_24 = g_485ac0;
	if (local_24 <= 0)
		local_24 = 60;
	if (local_23 < 0)
	{
		s_75fc2 local_25;
		local_25.field_0 = g_4e6398;
		local_4->field_478 = local_25.field_8;
		local_4->field_470 = local_11;
		local_23 = 0;
	}
	if (local_17 > 0.0f)
	{
		local_13 = function_75fc1((real)local_24 / local_17);
		local_2 = local_23 >= local_13;
		if (!local_2)
			return local_2;
	}
	else if (local_17 == 0.0f)
		return local_2;
	long local_26 = NONE;
	if (local_18 >= 0)
	{
		local_26 = (local_23 * local_18 / local_24) / 8;
		local_6 = local_26 - 45;
		local_2 = local_6 >= 48;
		if (!local_2)
			return local_2;
	}
	if (local_13 >= 0)
	{
		local_4->field_470 = local_11;
		local_4->field_478 += local_13;
		if (local_4->field_478 + local_13 <= local_12.field_8)
			local_4->field_478 = local_12.field_8;
	}
	local_2 = true;
	if (local_16)
		local_7 = true;
	if (local_5 >= 0 && local_26 >= 0)
	{
		long local_27 = link_connection_get_reliable_bytes(local_3);
		long local_28 = link_connection_get_reliable_pending(local_3);
		if (local_28 > 0 && local_27 + local_26 > local_5)
		{
			long local_29 = local_5 - local_27;
			local_2 = local_29 >= local_27 / local_28 && local_29 - 45 >= 48;
			if (local_2)
				local_6 = local_29 - 45;
			if (local_4->field_48c)
				local_4->field_4a2 = true;
			if (!local_2)
				return local_2;
		}
	}
	long local_30;
	if (local_10)
	{
		real local_31 = 1.0f;
		if (arg_2)
		{
			if (local_3->callback)
				local_31 = *(real *)((byte *)arg_0->configuration +
					(link_connection_owner_active(local_3) ? 0xe4 : 0xe8));
			else
				local_31 = *(real *)((byte *)arg_0->configuration + 0xec);
		}
		long local_32 = local_6 >= 0 ? function_75fc1((real)local_6 * local_31) : 512;
		if (local_32 > arg_9)
			local_32 = arg_9;
		if (local_4->field_48c && local_32 < local_9)
			local_4->field_4a1 = true;
		local_30 = function_765c0(local_0, local_32, arg_10);
	}
	else
		local_30 = 0;
	long local_33 = local_6 >= 0 ? local_6 - local_30 : 0x600;
	if (local_6 >= 0 && local_33 <= 48)
		local_33 = 48;
	long local_34 = 0x516 - local_30;
	local_34 -= local_34 % 8;
	if (local_34 < 0)
		return false;
	if (local_33 > 8)
		local_33 -= local_33 % 8;
	if (local_33 > local_34)
		local_33 = local_34;
	*arg_7 = local_33;
	*arg_8 = local_30;
	*arg_5 = arg_4;
	*arg_6 = local_7;
	return local_2;
}
