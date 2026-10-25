// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include <string.h>
#include <math.h>

struct short_rect
{
	short v0, v1, v2, v3;
};

struct short_rect_pair
{
	short_rect a, b;
};

struct s_223240
{
	long field_0, field_4, field_8;
	byte field_c[0x74];
	byte field_80[0x30];
	short_rect field_b0, field_b8;
	byte field_c0[0x58];
};

struct s_2f800_source;
struct s_2f800_size;
struct s_2f800_view;
struct s_223080;
struct s_223081;
struct s_unknown_13bf00;
struct s_223241
{
	byte field_0[0x358];
};
s_223241 g_4e9c8c[4];
extern s_unknown_13bf00 *g_510c50;
struct s_unknown_485a88
{
    byte flag;
    short_rect_pair rects;
};
extern s_unknown_485a88 g_unknown_485a88;
#define g_485a8a (g_unknown_485a88.rects)
extern long g_4e6470, g_4e64a0;
extern byte g_485ac2;
extern real g_5234dc;
s_223240 g_47f0f8[6];

void function_2f800(s_2f800_view *, s_2f800_source const *, s_2f800_size const *, s_2f800_size const *);
void __stdcall function_2c8a0(byte const *);
void function_2b5d0(long, long, long, long, s_223240 const *);
void function_223080(long, long, long, s_223080 *, long, long, s_223081 const *);
bool function_68290(void);
void main_time_wait_for_vblank(void);
void function_13f10(long, long);

PRIVATE __forceinline s_223240 *function_2232a1(s_223240 *arg_0)
{
	arg_0->field_4 = NONE;
	arg_0->field_8 = NONE;
	arg_0->field_b0 = g_485a8a.a;
	arg_0->field_b8 = g_485a8a.b;
	arg_0->field_0 = 1;
	function_2f800((s_2f800_view *)arg_0->field_80, 0, 0, 0);
	memcpy(arg_0->field_c, arg_0->field_80, 0x74);
	return arg_0;
}

// @retail 0x2232a0
void function_2232a0(void)
{
	function_2c8a0((byte const *)function_2232a1(&g_47f0f8[0]));
}

// score 151 (was 151): count-one paths still bypass the common count/layout join; tail call differs.
// Signed-byte count alone/with player value 169; signed-word layout alone/with normalized selection 174;
// native selected-player value, normalized selection and their combination 151; redundant scan test removed 154.
// count-one join, slot predicate and terminal/callee registers differ;
// byte count 169; layout enum 151; completed ordinal at terminal/dispatch/both 151/158/158;
// direct terminal initializer 151; layout refinement 151; world-layout result 151.
// count-one paths skip the common count/layout join;
// slot-predicate materialization and final initializer residences differ.
// Mode/options receivers, mode result, external count/slot references, typed selected
// record, terminal record reference and both record receivers each 151 (575 bytes).
// score 151 (was 151): the common count/layout join, slot predicate and terminal reset still differ.
// First/second/both conditional countdown expressions 169/161/167;
// signed-byte/unsigned-word layout 189/174; unsigned-word output ordinal 201;
// signed-byte selection 164; dispatch noinline declaration 151. Original body retained.
// One-dword frame agrees; count-one paths bypass the shared count/layout join, with slot and reset
// allocation gaps. unsigned selected slot 151; word selected slot 157; word view ordinal 201; byte
// view ordinal 167; exact scan bound 175; existing initializer inline 151; completed search result
// 180; word scan and ordinal 157; byte selected slot 164.
// @retail 0x2234e0
void function_2234e0(void)
{
	long local_1 = NONE;
	long local_2;
	long local_3;
	if (g_4e6470 > 0)
		--g_4e6470;
	if (g_4e64a0 > 0)
		--g_4e64a0;
	if (g_510c50 && ((byte const *)g_510c50)[5])
		local_2 = 1;
	else
	{
		short local_4 = *(short const *)((byte const *)g_4e8c20 + 8);
		local_2 = 1;
		if (local_4 >= 1)
		{
			local_2 = 4;
			if (local_4 <= 4)
				local_2 = local_4;
		}
	}
	if (local_2 > 1)
	{
		if (!(g_4e6948 && g_4e6948->flag && g_4e6948->index != NONE &&
			g_4e6948->state == 3) && g_485ac2)
			local_3 = 2;
		else
			local_3 = 1;
	}
	else
		local_3 = 0;
	long local_0 = 0;
	do
	{
		byte const *local_5 = 0;
		for (++local_1; local_1 < 4; ++local_1)
		{
			if (local_1 != NONE && g_4e8c20->entries[local_1] != NONE)
				break;
		}
		if (local_1 >= 4)
			local_1 = NONE;
		else if (g_4686c4 != NONE)
			local_5 = g_4e9c8c[local_1].field_0;
		function_223080(0, local_0, local_1, (s_223080 *)&g_47f0f8[local_0 + 1],
			local_2, local_3, (s_223081 const *)local_5);
		++local_0;
	} while (local_0 < local_2);
	s_223240 *local_6 = &g_47f0f8[local_2 + 1];
	function_2232a1(local_6);
	function_2b5d0(local_2 + 1, 3, local_2, local_3, &g_47f0f8[1]);
}

// @retail 0x223240
void function_223240(void)
{
	if (g_4e6948 && g_4e6948->flag1120 && g_4e6948->index != NONE && !function_68290())
		function_2234e0();
	else
		function_2232a0();
	bool local_0 = g_4e6470 <= 0 && g_4e64a0 <= 0;
	main_time_wait_for_vblank();
	if (local_0)
		function_13f10(0, 0);
}
