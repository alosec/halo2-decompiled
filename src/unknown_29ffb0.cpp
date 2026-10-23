#include "unknown_11c920.h"
#include "globals.h"
#include "slot_handler.h"
#include "object_default_placement.h"
#include "game_engine_events.h"
#include <math.h>
// @flags /O2 /Ob1 /arch:SSE /Gr

struct s_29ffd5
{
	byte field_0[0x24];
	point3f field_24;
	real field_30;
	real field_34;
};

struct s_29ffcd
{
	byte field_0[0x1e4];
	s_29ffd5 *field_1e4;
};

struct s_29ffb0
{
	byte field_0[0x14];
	long field_14;
	signed char field_18;
	byte field_19[0x116 - 0x19];
	short field_116;
	byte field_118[0x13c - 0x118];
	long field_13c;
	byte field_140[0x150 - 0x140];
	vector3f field_150;
	vector3f field_15c;
	byte field_168[0x180 - 0x168];
	vector3f field_180;
};

void function_b73b0(long arg_0);
void function_d4ff0(long arg_0);
struct s_object;
s_object *function_badc0(long arg_0, dword arg_1);
vector3f *function_11d090(vector3f const *arg_0, vector3f *arg_1);
void function_141590(transform4x3f const *arg_0, transform4x3f *arg_1);
bool function_14e970(long arg_0, long arg_2, point3f const *arg_1);
void function_1874b0(long arg_0, vector3f const *arg_1);

PRIVATE __forceinline real function_29ffb1(real arg_0, real arg_1, real arg_2, real arg_3, real arg_4, real arg_5)
{
	real local_0 = arg_0 * arg_1;
	local_0 += arg_2 * arg_3;
	local_0 += arg_4 * arg_5;
	return local_0;
}

// @retail 0x29ffb0
void __stdcall function_29ffb0(long arg_0, short arg_1, bool arg_2, bool arg_3)
{
	if (arg_0 != NONE)
	{
		s_29ffb0 *local_0 = (s_29ffb0 *)object_get(arg_0);
		s_29ffd5 *local_1 = &((s_29ffcd *)g_4e0350)->field_1e4[arg_1];
		if (arg_2)
		{
			if (local_0->field_14 != NONE)
				function_b9a50(arg_0);
			else
			{
				s_object_default_placement_view *local_2 = (s_object_default_placement_view *)local_0;
				if (TEST_FIELD_BIT(local_2->hidden))
				{
					s_object_default_placement_view *placed = object_default_placement_get(arg_0);
					placed->hidden = false;
					if (!TEST_FIELD_BIT(placed->flag8) && g_4de2f4 && *(byte *)g_4de2f4)
						function_b8600(arg_0, 0);
				}
			}
		}
		vector3f local_3;
		vector3f local_4;
		vector3f local_5;
		real local_6 = (real)cos(local_1->field_34);
		local_3.i = (real)cos(local_1->field_30) * local_6;
		local_3.j = (real)sin(local_1->field_30) * local_6;
		local_3.k = (real)sin(local_1->field_34);
		function_11d090(&local_3, &local_4);
		function_b73b0(arg_0);
		function_d4ff0(arg_0);
		s_29ffb0 *local_7 = (s_29ffb0 *)function_badc0(arg_0, 3);
		if (local_7)
		{
			long local_8 = NONE;
			s_29ffb0 *local_9 = (s_29ffb0 *)function_badc0(arg_0, 3);
			if (local_9)
				local_8 = local_9->field_13c;
			if (local_7->field_14 != NONE)
			{
				s_29ffb0 *local_10 = (s_29ffb0 *)object_get(local_7->field_14);
				transform4x3f local_11;
				function_141590((transform4x3f *)((byte *)local_10 + local_10->field_116) + local_7->field_18, &local_11);
				local_5.i = function_29ffb1(local_11.left.i, local_3.j, local_11.up.i, local_3.k, local_11.forward.i, local_3.i);
				local_5.j = function_29ffb1(local_11.left.j, local_3.j, local_11.up.j, local_3.k, local_11.forward.j, local_3.i);
				local_5.k = function_29ffb1(local_11.left.k, local_3.j, local_11.up.k, local_3.k, local_11.forward.k, local_3.i);
			}
			else
				local_5 = local_3;
			if (arg_3)
			{
				local_7->field_150 = local_3;
				local_7->field_15c = local_3;
				local_7->field_180 = local_3;
			}
			if (local_8 != NONE)
			{
				if (arg_2)
				{
					function_14e970(local_8, NONE, &local_1->field_24);
					arg_2 = false;
				}
				if (arg_3)
				{
					s_event_player *local_12 = event_player_get(local_8);
					if (local_12->local_index != NONE)
						function_1874b0(local_12->local_index, &local_5);
				}
			}
		}
		function_b75a0(arg_0, arg_2 ? &local_1->field_24 : NULL, arg_3 ? &local_3 : NULL, arg_3 ? &local_4 : NULL, NULL, false);
	}
}
