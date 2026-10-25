// @flags /O2 /Ob1 /Gr
#include "unknown_11c920.h"
#include "unknown_058dd0.h"
#include <string.h>

struct s_session_search;
void function_72330(c_session_state_matchmaking *arg_0);
void function_72540(c_session_state_matchmaking *arg_0);
void function_72260(c_session_state_matchmaking *arg_0);
void function_72170(c_session_state_matchmaking *arg_0);
void function_90840(s_session_search *arg_0);
long function_66050(c_class_58d20 *arg_0, long arg_1);
long network_session_find_member(c_class_58d20 *arg_0, const s_session_member_identity *arg_1);
bool function_70d60(c_session_state_matchmaking *arg_0);
bool function_70ea0(c_session_state_matchmaking *arg_0);
bool function_71210(c_session_state_matchmaking *arg_0);
bool function_712d0(c_session_state_matchmaking *arg_0);
bool __stdcall function_71390(c_session_state_matchmaking *arg_0);
bool function_71480(c_session_state_matchmaking *arg_0);
bool __stdcall function_71730(c_session_state_matchmaking *arg_0);
bool __stdcall function_71d70(c_session_state_matchmaking *arg_0);
bool network_session_host_set_value49f8(c_class_58d20 *arg_0, long arg_1);

// score 182 (was 243): frame and length agree; primary-session EBP versus ESI, mode joins
// and final Boolean normalization differ; byte return result 182; word return result 275;
// long return result 258; unsigned return result 258; independent mode gates 243;
// mode flag else 243; member gates with retained frame 243; terminal gates with retained frame 243.
// score 182 (was 182): primary-session join carrier and return normalization remain;
// secondary consumer scope / state phases / byte result after state / after sessions 182/182/185/185f;
// scoped dispatch / member receiver / both lifetimes / with dispatch 183/182/182/183f.
// @retail 0x706f0
bool c_session_state_matchmaking::update()
{
	byte local_3 = false;
	s_session_owner *local_0 = owner;
	c_class_58d20 *local_1 = local_0->session_a;
	c_class_58d20 *local_2 = local_0->session_b;
	long local_4 = local_1->state;
	if (!local_4 || function_058d90(local_1))
	{
		function_06df60(local_0, 0, 0, 0);
		mode = 1;
		goto local_done;
	}
	if (local_4 == 5 || local_4 == 6 || local_4 == 7 || local_4 == 8)
	{
		if (mode == 3)
		{
			if (!flag10)
				mode = 19;
			if (mode == 3)
			{
				if (!*(bool *)((byte *)this + 0x14))
					function_72330(this);
				if (mode == 3)
				{
					function_72540(this);
					if (mode == 3)
					{
						if (local_2->state > 2 && local_2->state <= 8)
						{
							for (long local_5 = 0; local_5 < local_2->member_count; local_5++)
							{
								if (local_2->members[local_5].unknown88 == 1 &&
									network_session_find_member(local_1,
										(const s_session_member_identity *)&local_2->members[local_5]) != NONE)
									mode = 5;
							}
						}
						if (mode == 3)
						{
							long local_6 = NONE;
							if (local_1->state > 2 && local_1->state <= 8)
								local_6 = local_1->value49c8;
							if (function_66050(local_1, local_6))
								mode = 7;
						}
					}
				}
			}
		}
		if (mode == 3)
			function_72260(this);
	}
	else
	{
		volatile long local_7 = local_4;
	}
	if (flag97c)
		function_90840((s_session_search *)&flag97c);
	function_72170(this);
	if (mode == 3)
	{
		switch (local_1->type)
		{
		case 6: local_3 = function_70d60(this); break;
		case 7: local_3 = function_70ea0(this); break;
		case 8: local_3 = function_71210(this); break;
		case 9: local_3 = function_712d0(this); break;
		case 10: local_3 = function_71480(this); break;
		case 11: local_3 = function_71730(this); break;
		case 12: local_3 = function_71d70(this); break;
		case 13: local_3 = function_71390(this); break;
		default: mode = 1; break;
		}
	}
	local_4 = local_1->type;
	if (local_4 != 7 && local_4 != 8 && local_4 != 9 && local_4 != 13 && flag97c)
		function_090c80(&flag97c);
	if (mode != 3 && mode != 2)
	{
		if (local_1->function_058d20())
			network_session_set_mode(local_1, 1);
		function_06df60(local_0, 1, 0, 0);
		local_3 = true;
	}
	local_4 = local_1->state;
	if (local_4 == 5 || local_4 == 6 || local_4 == 7 || local_4 == 8)
		network_session_host_set_value49f8(local_1, mode);
	else
	{
		volatile long local_8 = local_4;
	}
local_done:
	return local_3;
}
