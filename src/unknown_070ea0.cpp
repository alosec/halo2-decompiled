// @flags /O2 /Ob1 /Gr
#include "unknown_11c920.h"
#include "unknown_058dd0.h"
#include "unknown_058ee0.h"
#include "unknown_0662e0.h"
#include "globals.h"
#include <xtl.h>
#include <string.h>
#include <time.h>

struct s_session_search;
struct s_909ff;
struct s_session_property_value
{
	long kind;
	long value04;
	long value08;
	long value0c;
	long value10;
	long value14;
	long value18;
};
struct s_search_session
{
	long kind;
	XNKID id;
	XNKEY key;
	XNADDR address;
	long value;
};

struct s_70ea0
{
	long field_0;
	long field_4;
	long field_8;
	long field_c;
	long field_10;
	long field_14;
	long field_18;
	long field_1c;
	dword field_20[3];
};

void function_70b70(c_session_state_matchmaking *arg_0);
long network_session_get_language(c_class_58d20 *arg_0);
s_session_id *network_session_get_id(c_class_58d20 *arg_0);
bool network_session_host_set_data49cc(c_class_58d20 *arg_0, const dword *arg_1);
bool function_90880(s_session_search *arg_0, long arg_1, long arg_2, long arg_3,
	long arg_4, long arg_5, long arg_6, long arg_7, const s_909ff *arg_8, long arg_9);
bool session_search_select(s_session_search *arg_0, s_search_session *arg_1);
void session_search_get_progress(s_session_search *arg_0, long *arg_1, long *arg_2, long *arg_3, long *arg_4);
void session_property_value_set_kind1(s_session_property_value *arg_0, long arg_1, long arg_2, long arg_3, long arg_4);
bool __stdcall function_59e50(c_class_58d20 *arg_0, long arg_1, long arg_2,
	const XNKID *arg_3, const XNKEY *arg_4, const s_session_member_identity *arg_5,
	long arg_6, const dword *arg_7, const long *arg_8, const long *arg_9,
	bool arg_10, long arg_11, const s_session_id *arg_12, const void *arg_13);

static __forceinline long function_70f23(c_class_58d20 *arg_0)
{
	return network_session_get_language(arg_0);
}

#pragma inline_depth(0)
// @retail 0x70ea0
bool function_70ea0(c_session_state_matchmaking *arg_0)
{
	c_class_58d20 *local_1 = arg_0->owner->session_a;
	c_class_58d20 *local_2 = arg_0->owner->session_b;
	bool local_0 = false;
	function_70b70(arg_0);
	long local_3 = local_1->state;
	if (local_3 == 5 || local_3 == 6 || local_3 == 7 || local_3 == 8)
	{
		if (local_1->type != 7)
			goto local_19;
		s_session_search *local_4 = (s_session_search *)&arg_0->flag97c;
		if (!arg_0->flag97c)
		{
			const s_909ff *local_5;
			if (*(bool *)((byte *)arg_0 + 0x93c))
				local_5 = (const s_909ff *)((byte *)arg_0 + 0x93d);
			else
				local_5 = NULL;
			function_90880(local_4, *(long *)((byte *)arg_0 + 0x18),
				*(long *)((byte *)arg_0 + 0x7b4), *(long *)((byte *)arg_0 + 0x960),
				*(long *)((byte *)arg_0 + 0x958), *(long *)((byte *)arg_0 + 0x95c),
				*(long *)((byte *)arg_0 + 0x94c), *(long *)((byte *)arg_0 + 0x950),
				local_5, function_70f23(local_1));
		}
		if (arg_0->flag97c && !local_2->state)
		{
			s_search_session *local_6 = (s_search_session *)((byte *)arg_0 + 0xa20);
			if (session_search_select(local_4, local_6))
			{
				s_session_snapshot local_7;
				local_7.unknown00 = local_6->kind;
				memcpy(local_7.unknown04, (byte *)arg_0 + 0xa24, sizeof(local_6->id));
				memcpy(local_7.unknown0c, (byte *)arg_0 + 0xa2c, sizeof(local_6->key));
				memcpy(local_7.words, (byte *)arg_0 + 0xa3c, sizeof(local_6->address));
				local_7.unknown40 = 2;
				if (network_session_host_set_data5ddc(local_1, (const s_parameters_part *)&local_7))
				{
					s_70ea0 local_8;
					memset(&local_8, 0, sizeof(local_8));
					local_8.field_14 = *(long *)((byte *)arg_0 + 0x960);
					local_8.field_18 = *(long *)((byte *)arg_0 + 0x958);
					local_8.field_1c = *(long *)((byte *)arg_0 + 0x95c);
					local_8.field_0 = 2;
					local_8.field_4 = *(long *)((byte *)arg_0 + 0x94c);
					local_8.field_8 = *(long *)((byte *)arg_0 + 0x950);
					local_8.field_10 = *(long *)((byte *)arg_0 + 0xa60);
					local_8.field_c = (long)time(NULL) - arg_0->unknown968;
					memcpy(local_8.field_20, (byte *)arg_0 + 0x93d, sizeof(local_8.field_20));
					s_session_id local_9 = *network_session_get_id(local_1);
					if (function_59e50(local_2, 2, local_7.unknown00,
						(const XNKID *)local_7.unknown04, (const XNKEY *)local_7.unknown0c,
						(const s_session_member_identity *)local_7.words,
						*(long *)((byte *)arg_0 + 0x7b4), (const dword *)((byte *)arg_0 + 0x7b8),
						(const long *)((byte *)arg_0 + 0x878), (const long *)((byte *)arg_0 + 0x8b8),
						true, NONE, &local_9, &local_8))
						network_session_set_mode(local_1, 8);
					else
						arg_0->mode = 9;
				}
				else
					arg_0->mode = 8;
				local_0 = true;
			}
			else if (*(long *)((byte *)arg_0 + 0x984) != 1)
			{
				if (*(long *)((byte *)arg_0 + 0x984) == 0)
					network_session_set_mode(local_1, 6);
				else
					arg_0->mode = 15;
				local_0 = true;
			}
		}
	}
	else
	{
		volatile long local_10 = local_3;
	}
	if (local_1->type == 7)
	{
		local_3 = local_1->state;
		if (local_3 == 5 || local_3 == 6 || local_3 == 7 || local_3 == 8)
		{
			long local_18 = arg_0->unknowna7c;
			long local_11 = g_510548 ? g_51054c : GetTickCount();
			if (local_11 - local_18 > *(long *)((byte *)arg_0 + 0x96c) + g_network_configuration.value198)
			{
				network_session_set_mode(local_1, 6);
				local_0 = true;
			}
			long local_12, local_13, local_14, local_15;
			session_search_get_progress((s_session_search *)&arg_0->flag97c,
				&local_12, &local_13, &local_14, &local_15);
			s_session_property_value local_16;
			session_property_value_set_kind1(&local_16, local_12, local_13, local_14, local_15);
			network_session_host_set_data49cc(local_1, (const dword *)&local_16);
		}
		else
		{
			volatile long local_17 = local_3;
		}
	}
local_19:
	return local_0;
}
#pragma inline_depth(255)
