// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_2605d0.h"
#include "unknown_2626b0.h"
#include "normalize3d.h"

/* slot type 0x6d */

short __stdcall function_1b2ff0(long actor_index);
bool __stdcall function_1b3360(long actor_index, s_slot *slot);
bool __stdcall function_1b3380(long actor_index, s_slot *slot);

/* iterates the squads of an encounter (function_204ec0 and function_205010) */
struct s_squad_iterator
{
	byte unknown00[0x14];
};

void function_204ec0(s_squad_iterator *iterator, short encounter_index, short a, short b);
short function_205010(s_squad_iterator *iterator);

// @retail 0x1b2ff0
short __stdcall function_1b2ff0(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = 0;
	short count = 0;
	s_squad_iterator iterator;

	function_204ec0(&iterator, (short)actor->unknown030, 5, actor->unknown26c != NONE);
	while (function_205010(&iterator) != NONE)
	{
		if (++count >= 2)
		{
			result = 1;
			break;
		}
	}
	return result;
}

/* the squads slot type 0x6d has tried (a bit per squad) and the one it
   chose */
struct s_squad_choice
{
	dword tried;
	short unknown4;
	short unknown6;
};

struct s_slot_6d
{
	s_slot_header header;
	s_squad_choice choice;
	byte unknown14[0x40 - 0x14];
};

bool __stdcall function_1b3070(long actor_index, s_squad_choice *choice);

// @retail 0x1b3360
bool __stdcall function_1b3360(long actor_index, s_slot *slot)
{
	s_squad_choice *choice = &((s_slot_6d *)slot)->choice;

	choice->tried = 0;
	return function_1b3070(actor_index, choice);
}

// @retail 0x1b3540
void __stdcall function_1b3540(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);

	actor->unknown4ae = true;
	actor->unknown484 = true;
	if (actor->unknown26c != NONE)
		actor->unknown4b4 = 0.6f;
}

s_slot_handler_2 g_47e1c0 =
{
	{
		0x6d, 2, 0xfff, -2, 0,
		function_1b2ff0, function_1bced0, function_1b3360, 0, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	(t_slot_proc)function_1b3380, 0, function_1b3540
};

// @retail 0x1b3070
bool __stdcall function_1b3070(long actor_index, s_squad_choice *choice)
{
	s_actor_view *actor = actor_get(actor_index);
	short current;
	real best_score;
	short best;
	short best_encounter;
	short best_squad;
	short previous;
	previous = NONE;
	current = 0;
	best_squad = NONE;
	best_encounter = NONE;
	best = NONE;
	best_score = -3.402823466e38f;
	s_squad_iterator iterator;
	function_204ec0(&iterator, (short)actor->unknown030, 5, actor->unknown26c != NONE);
	short squad = function_205010(&iterator);
	if (squad == NONE)
		return false;
	do
	{
		if (current >= 32)
			break;
		if (!(choice->tried & (1 << current)))
		{
			word encounter = *(word *)(iterator.unknown00 + 6);
			byte *encounters = *(byte **)((byte *)g_4e0350 + 0x16c);
			byte *squads = *(byte **)(encounters + encounter * 0x38 + 0x34);
			s_type_c3b527 *target = (s_type_c3b527 *)(squads + squad * 0x88 + 0x24);
			point3f point;
			vector3f direction;
			if (target->output_index == NONE)
				vector3d_from_points3d(&actor->position, &target->point, &direction);
			else
			{
				if (!function_2104b0(target->output_index, &target->point, &point))
					point = target->point;
				vector3d_from_points3d(&actor->position, &point, &direction);
			}
			/* the normalization is evaluated first, so the dot product is of the unit direction */
			real score = (real)(dot3f(&direction, &actor->unknown290) - normalize3d(&direction) * 0.1);
			if (score > best_score)
			{
				best = current;
				best_squad = *(short *)(iterator.unknown00 + 2);
				best_encounter = *(short *)(iterator.unknown00 + 6);
				best_score = score;
			}
		}
		else if (squad == choice->unknown6 && *(short *)(iterator.unknown00 + 6) == choice->unknown4)
			previous = current;
		current++;
		squad = function_205010(&iterator);
	} while (squad != NONE);
	if (current < 2)
		return false;
	if (best != NONE)
	{
		choice->unknown4 = best_encounter;
		choice->unknown6 = best_squad;
		choice->tried |= 1 << best;
		return true;
	}
	choice->tried = 1 << previous;
	actor = actor_get(actor_index);
	actor->unknown3fe = 3;
	long i = 0;
	do
	{
		actor->unknown400[i].reference = g_470fa0;
		i++;
	} while (i < 4);
	function_1b3070(actor_index, choice);
	return true;
}

real function_1f8940(long actor_index);

// score 27 (was 27): frame and active-getter prefix agree; selected reference enters EBX before validation.
// Earlier forms and observations:
// actor-data load and early selected-reference transfer still differ.
// frame and prologue agree; selected-reference transfer and follow setup remain.
// Earlier forms:
// the selected reference still enters EBX before its validity comparison.
// Earlier: actor-data load and early selected-reference transfer still differ.
// late selected value 44; late value/native result 97; native/short/byte result
// 97/94/134; sentinel operands 44; active-test result 122; native request fields 44.
// the active test reads through the getter, as the other actor
// updates do, which gives retail's actor-data load (EDX, LEA ESI) and request
// schedule; the found reference still enters EBX right after the selection call,
// where retail keeps it in EAX until just before the follow call. Calling the
// state-2 getter 120 (stays a call); actor declared inside the active block 50;
// reference declared then assigned 27; a selected copy in the found branch 27;
// reference as a scalar 27 (diagnostic); direct release and exit on the not-found
// path 51; reference declared at function scope 27.
// Earlier actor/reference forms:
// Actor object reference/const view 44/44; candidate/flag/index/joint names 44/44/44/44;
// result from reference validity 116; successful-selection arm first 71.
// actor-data load and selected-reference transfer still differ.
// Const reference/early reference/output-first declarations/const scratch 44;
// choice failure from test 123; nested reference gate 44; inclusive cancel limits 48;
// aggregate-zero request 89 (446 bytes).
// This round:
// Member equality low/high first 92/92; combined follow/cancel 27; native follow result 27;
// nested count domain 33; owner/path/both output defaults 113/63/120.
// Earlier request/reference forms:
// Selection/follow inline states 27/27/27; noinline states 27/27/27;
// mixed inline/noinline states 27/27. Original declarations restored.
// Earlier: actor-data load and selected-reference transfer still differ.
// failed result first 34; failed result chain 27; shared cancellation count 27;
// native cancellation limit 89; const reference and count 27; result chain and count 27;
// cancel completion test 27; named cancel result 27.
// Round 85: reference assigned inside the sentinel test 27; result/active stores swapped 34;
// other index and flag declared before the scratch 27; follow-and-cancel as a forceinline
// helper 27 (plain inline stays out of line); actor pointer for the active store 130.
// Request clear before scratch 71; all constants before scratch 76; full request before scratch 75;
// kind/numerics/flags before scratch 68/68/77; choice fields before constants 30;
// constant prefix with range before choice 77. Scratch/request staging restored.
// Round 14 (exact LTCG harness): a copy of the reference for the follow call, the cancel call or both 27;
// the follow result in a local 27; the found arm first 54; the sentinel test as a long compare 27, as
// field compares 72 (130 instructions); the reference declared at function scope 27; the outputs declared
// at the top, before the scratch or swapped, the scratch after the request 27; the request at the top 50.
// @retail 0x1b3380
bool __stdcall function_1b3380(long actor_index, s_slot *slot)
{
    bool result = true;
    s_actor_view *actor = actor_get(actor_index);
    if (actor_get(actor_index)->unknown040)
    {
        if (!REFERENCE_EQUAL(actor->unknown418, g_470fa0) &&
            (actor->unknown504 == 2 || function_1f8940(actor_index) < 0.6f))
        {
            function_262800(actor_index, actor->unknown418, false);
            result = function_1b3070(actor_index, &((s_slot_6d *)slot)->choice);
            if (!result)
                goto done;
        }
        byte *scratch = ai_scratch_buffer_get();
        s_2605d0_request request;
        memset(&request, 0, sizeof(request));
        request.type = 4;
        request.unknown05b = true;
        *((byte *)&request + 0x68c) = true;
        *(short *)((byte *)&request + 0x68e) = ((s_slot_6d *)slot)->choice.unknown4;
        *(short *)((byte *)&request + 0x690) = ((s_slot_6d *)slot)->choice.unknown6;
        *(real *)((byte *)&request + 0x1c) = 50.0f;
        request.unknown015 = true;
        *((byte *)&request + 0x59) = true;
        request.unknown69c = actor->unknown328 <= 2;
        long other_index;
        bool unknown;
        s_reference reference = function_2605d0(actor_index, &request, 0, (long)&other_index, scratch, &unknown);
        if (REFERENCE_EQUAL(reference, g_470fa0))
        {
            actor_get(actor_index)->unknown040 = false;
            result = false;
        }
        else if (REFERENCE_EQUAL(function_2626b0(actor_index, reference, other_index, scratch, unknown, true), g_470fa0))
        {
            if (actor->unknown5b4 > 4 || (actor->unknown26c != NONE && actor->unknown5b4 > 2))
                function_262800(actor_index, reference, false);
        }
        ai_scratch_buffer_release(scratch);
    }
done:
    return result;
}
