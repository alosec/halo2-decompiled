// @flags /O2 /Gr /arch:SSE
#include "unknown_2551c0.h"
#include "globals.h"

struct s_28dab0
{
	byte field_0[4];
	long field_4;
	point3f field_8;
	short field_14;
	short field_16;
	long field_18;
	short field_1c;
	byte field_1e[2];
	long field_20;
	point3f field_24;
	bool field_30;
	bool field_31;
	byte field_32[2];
};

struct s_28dab1
{
	byte field_0[0x18];
	long field_18;
};

struct s_28dab2
{
	byte field_0[0x2e];
	short field_2e;
};

long function_1e4990(long arg_0);
long function_28dbd0(long arg_0, long arg_1, long arg_2, s_28dab2 const *arg_3, bool arg_4);
void __stdcall swarm_delete(long arg_0);

// score 53 (was 53): the tag pointer is in esi where retail keeps it in edi
// (the constant -1 is in the other one), and retail keeps the loop limit in
// ebx with a cmov where ours puts it in a stack slot with a branch. Nine
// source rewrites (the declaration order, the result split from the
// allocation, the comparison against the variable, the goto, the width of the
// limit) all scored 53 or worse.
// score 44 (was 44): retail's record stores run NONE pair, zero pair, first point,
// field_1c, second point, flags; that order 53. The record pointer before the
// tag-field read with the limit next gives retail's head (tag edi, NONE esi)
// but not its limit: ternary 79; `count = 10; if (field > 0) count = field;` 56
// (cmovg into ebp), 65 with an else tail. That form after the stores gives
// retail's cmovg ebx but moves NONE to ebx 81.
// Tag/sentinel register roles and spilled loop limit
// remain. Native requested count 74; replace nonpositive bound 97; explicit
// bound arms 44; sampled object flags 44; guarded tag-field reference 97;
// immutable record receiver 44; native completed count 51; immutable receiver
// and bound 44.
// tag/sentinel roles and the spilled loop bound remain.
// Existing point setters first/second/both 83/88/92; tag/placement references 44/44;
// plain while 44; equality termination 45; completed-count field reference 44.
// the fresh-record field stores now match retail's order, but
// the tag pointer is still in esi where retail keeps it in edi, and retail keeps
// the loop limit in ebx with a cmov where ours keeps it in a stack slot.
// @retail 0x28dab0
long __stdcall function_28dab0(long arg_0, s_28dab2 const *arg_1)
{
	long local_0 = NONE;
	s_28dab1 *local_1 = (s_28dab1 *)g_4e3b44[arg_0 & 0xffff].bytes;
	if ((*(byte *)function_1e4990(arg_0) & 1) && local_1->field_18 != NONE)
	{
		local_0 = record_pool_allocate(g_5044c8);
		if (local_0 != NONE)
		{
			long local_2 = local_1->field_18;
			s_28dab0 *local_3 = (s_28dab0 *)perception_get(local_0);
			local_3->field_30 = false;
			local_3->field_31 = true;
			local_3->field_14 = 0;
			local_3->field_16 = 0;
			local_3->field_1c = 0;
			local_3->field_4 = NONE;
			local_3->field_18 = NONE;
			local_3->field_8 = *g_468788;
			local_3->field_24 = *g_468788;
			short local_4 = arg_1->field_2e > 0 ? arg_1->field_2e : 10;
			for (short local_5 = 0; local_5 < local_4; local_5++)
				function_28dbd0(local_0, arg_0, local_2, arg_1, local_5 == 0);
			if (local_3->field_14 <= 0)
			{
				swarm_delete(local_0);
				local_0 = NONE;
				goto local_6;
			}
			local_3->field_16 = local_3->field_14;
		}
	}
local_6:
	return local_0;
}
