// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_0259a0.h"

/* slot type 0x23 */

struct s_slot_23
{
	s_slot_header header;
	bool unknown0c;
	bool unknown0d;
	short unknown0e;
	byte unknown10[0x1b - 0x10];
	bool unknown1b;
	bool unknown1c;
	char unknown1d;
	byte unknown1e[2];
	s_type_c3b527 point;
	vector3f facing;
	byte unknown3c[0x40 - 0x3c];
};

struct s_slot_23_data
{
    bool unknown00;
    bool unknown01;
    short unknown02;
    real unknown04;
    byte unknown08[0x0f - 0x08];
    bool unknown0f;
    bool unknown10;
    char unknown11;
    byte unknown12[2];
    s_type_c3b527 point;
    vector3f facing;
    byte unknown30[0x34 - 0x30];
};

short __stdcall function_1b84a0(long actor_index);
bool __stdcall function_1b85a0(long actor_index, s_slot *slot);
void __stdcall function_1b89d0(long actor_index, s_slot *slot);
void __stdcall function_1b8ae0(long actor_index, s_slot *slot);
bool function_25da00(s_prop_node_view *node);

// @retail 0x1b84a0
short __stdcall function_1b84a0(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = 0;

	if (actor->prop_index != NONE && !actor->unknown223)
	{
		s_prop_node_view *node = prop_node_get(actor->prop_index);
		s_prop_view_fields *view = prop_node_view(node);

		if (view && node->unknown24 >= 3 && (view->unknown70 == 1 || !function_25da00(node)))
			result = 3;
	}
	return result;
}

// @retail 0x1b8540
bool __stdcall function_1b8540(long actor_index, s_slot *slot)
{
	bool result = false;
	s_actor_view *actor = actor_get(actor_index);

	if (actor->prop_index != NONE && prop_node_get(actor->prop_index)->unknown27 < 2)
	{
		s_slot_23 *state = (s_slot_23 *)slot;

		state->unknown0c = false;
		state->unknown0d = false;
		state->unknown0e = 0;
		result = true;
	}
	return result;
}

real function_1f8940(long actor_index);
void *function_1e4db0(long actor_index);

/* the block of the actor's character tag function_1e4db0 returns */
struct s_character_db0_flags
{
	byte flags;
	byte unknown01[0xc - 1];
	real unknown0c;
	real unknown10;
};

bool function_1f86f0(long index);
bool function_1f4f40(long actor_index, vector3f const *facing, short unknown, s_type_c3b527 const *point, bool face_prop);
void function_262800(long actor_index, s_reference reference, bool unknown);

/* slot type 0x23, update: once the actor stands at its prop's position, it
   turns to face it; then it lets the prop go */
// score 47 (was 47): the actor index, actor, view and state registers differ (retail EBP/EDI/EBX/ESI,
// ours EDI/EBX/EBP/ESI), and retail saves EBX late, giving the NONE path its own epilogue.
// Round 198: typed slot data block/current/entry 61/62; on that block actor once/at use 66/85; long/word/byte readiness 76/78/77; stored readiness 47 (exact-match-only, restored). Opening body restored.
// Round 238: actual input/required/done byte fields, all seven subsets, 61 each;
// completion byte field 61. Original Boolean field types and body restored.
// Round 89: the readiness stored in the field and tested from it, kept (retail's mov eax, 1) 47; on
// it, one actor lookup at the top 52 (index EBP and view EBX as retail, actor/state ESI/EDI swapped),
// plus the state declared first 52, a prop index local 52 or the typed slot data block 52; the
// NONE case as an early store and return 91 (actor EDI as retail, more pushes sink).
// Round 270: native success/else-return gates for active, facing and release phases,
// all seven subsets 47; full gates with the slot data member 47. Original restored.
// Round 252: native node reference with/without the data block 47/47; actor reference
// after the view, alone/with node 47/47; entry actor reference alone/with node/with
// node and data block/with data block 52/52/52/52. Original complete body restored.
// @retail 0x1b89d0
void __stdcall function_1b89d0(long actor_index, s_slot *slot)
{
	long prop_index = actor_get(actor_index)->prop_index;
	s_slot_23 *state = (s_slot_23 *)slot;

	if (prop_index != NONE)
	{
		s_prop_node_view *node = prop_node_get(prop_index);
		s_prop_view_fields *view = prop_node_view(node);

		if (view)
		{
			s_actor_view *actor = actor_get(actor_index);
			if (state->unknown0c)
				return;

			state->unknown0c = state->unknown0d && function_1f86f0(actor_index);
			if (state->unknown0c)
			{
				if (!state->unknown1b || !state->unknown1d || state->unknown1c)
					return;

				function_1f4f40(actor_index, &state->facing, state->unknown1d, &state->point, true);
				state->unknown1c = true;
				state->unknown0c = false;
			}

			if (view->unknown70 != 0 || !function_25da00(node))
				return;

			function_262800(actor_index, actor->unknown418, false);
		}
	}

	state->unknown0c = true;
}

// @retail 0x1b8ae0
void __stdcall function_1b8ae0(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);

	if (actor->prop_index != NONE)
	{
		s_prop_view_fields *view = prop_node_view(prop_node_get(actor->prop_index));
		s_slot_23_data *state = (s_slot_23_data *)slot->unknownc;

		actor->unknown4a2 = true;
		if (view->unknown06 != 2)
			state->unknown02 = g_510c54->field_2_3;
		else if (state->unknown02 > 0)
			state->unknown02--;

		s_actor_view *current = actor_get(actor_index);
		if (current->unknown50c && current->unknown504 == 1 &&
			!(function_1f8940(actor_index) < 2.0f) && state->unknown02 <= 0)
		{
			actor->unknown41c = 3;
			actor->unknown420 = 0;
		}
		else if (view->unknown70 == 0)
		{
			actor->unknown420 = 2;
			actor->unknown41c = 4;
		}
		else if (view->unknown70 == 1)
		{
			point3f point;

			function_210850(&view->unknown78, &point);
			actor->unknown420 = 3;
			actor->unknown424.point = point;
			actor->unknown41c = 4;
		}

		if (actor->unknown229)
			actor->unknown482 = true;

		s_character_db0_flags *character = (s_character_db0_flags *)function_1e4db0(actor_index);

		if (character && (character->flags & 2) && actor->unknown086 <= 3)
			actor->unknown483 = true;
	}
}

// @retail 0x1b8c60
short __stdcall function_1b8c60(long actor_index, s_slot *slot, bool active)
{
	s_slot_23 *state = (s_slot_23 *)slot;
	short result = g_46fbe8;

	if (state->unknown0c)
		result = g_46fbe4;
	return result;
}

s_slot_handler_2 g_47e908 =
{
	{
		0x23, 2, 0, -2, 0,
		function_1b84a0, function_1b8c60, function_1b8540, slot_proc_nothing, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	(t_slot_proc)function_1b85a0, function_1b89d0, function_1b8ae0
};

void *function_1e4be0(long actor_index);
bool __stdcall function_1ac610(long actor_index, point3f const *point,
    s_object_marker *markers, short *indices);
short __stdcall function_272af0(s_match_globals *structure, point3f const *point);
struct s_1f4a20_entry;
struct s_1f4a20_source;
bool function_1f4a20(s_1f4a20_entry const *entries, long actor_index, s_reference reference, s_1f4a20_source const *source,
    bool alternate, s_type_c3b527 *point, vector3f *facing,
    signed char *type, bool *available);

// score 212 (was 257): frame 0x1634 versus 0x162c; scaled actor offset and
// slot/scratch homes remain, with a 14-byte length deficit.
// Round 198: payload/native input/read-only input/input scope 323 each; clamp 322;
// typed preparation/both scopes 257/257; scopes plus clamp 259; early payload receiver 257;
// ordinary result/receiver with ordinary result 325/325; search outputs first 257.
// Typed payload/native input and preparation scope kept at 257 plain; all follow-ups restored.
// Round 256: native geometry/bounds/marker-reference request fields, all seven subsets,
// and complete fields with movement scope 257 each; request header and body restored.
// Round 171: native search request at current/entry/guard/array declaration boundaries 257 each;
// complete zero aggregates at those boundaries 312/314/312/312. Original request, initialization
// and executable source restored; aligned frame and actor/slot/scratch homes remain.
// Round 213: character/payload/scratch home names and named scalar outputs [257, 257, 257,
// 257]; shared guard/node prop index with original/renamed actor and entry/later result [257,
// 257, 257, 257]. Frame and actor/slot/scratch allocation remain.
// Round 216: late marker default 323; result initialized after actor acquisition 257; shared
// scratch cleanup 257; late marker/result, marker/cleanup, result/cleanup and all 323/323/257/323;
// marker readiness directly from tests 315. Original early marker default and every exit restored;
// frame 0x1634/0x162c and actor/slot/scratch homes remain.
// Round 395: actor at use 303; invalid-search guard 212 (plain kept); complete
// success test 305; guard/actor 300, actor/test 296, guard/test 240 (retail length);
// all three and search-entry availability 305/305. Original actor and success flow kept.
// @retail 0x1b85a0
bool __stdcall function_1b85a0(long actor_index, s_slot *slot)
{
    volatile bool result = true;
    s_actor_view *actor = actor_get(actor_index);
    if (actor->prop_index == NONE)
        return false;
    if (!*((bool *)actor + 0x40))
        return result;

    byte *character;
    s_slot_23_data *state;
    bool has_markers = false;
    s_object_marker markers[32];
    short marker_indices[32];
    s_2605d0_request request;
    real distance;
    {
        s_prop_node_view *node = prop_node_get(actor->prop_index);
        s_prop_view_fields *view = prop_node_view(node);
        character = (byte *)function_1e4be0(actor_index);
        state = (s_slot_23_data *)slot->unknownc;
        state->unknown0f = false;
        state->unknown10 = false;
        state->unknown11 = 0;
        if (!*((bool *)actor + 0x3c) && actor->unknown227)
            has_markers = function_1ac610(actor_index, &prop_node_state(node)->position,
                markers, marker_indices);

        memset(&request, 0, sizeof(request));
        request.type = 3;
        if (view && view->unknown70 == 1)
        {
            function_210850(&view->unknown78, (point3f *)((byte *)&request + 0x24));
            short sector = *(short *)((byte *)view + 0x84) == NONE ?
                *(short *)((byte *)view + 0x72) :
                function_272af0(g_4e0348, (point3f *)((byte *)&request + 0x24));
            *((bool *)&request + 0x20) = true;
            *(s_type_c3b527 *)((byte *)&request + 0x30) = view->unknown78;
            *(long *)((byte *)&request + 0x40) = *(long *)((byte *)view + 0x74);
            *(short *)((byte *)&request + 0x44) = sector;
            distance = function_210ac0(&view->unknown78, &actor->position);
        }
        else
        {
            *((bool *)&request + 0x52) = false;
            distance = node->unknown28;
        }
    }
    s_character_db0_flags const *movement = (s_character_db0_flags const *)function_1e4db0(actor_index);
    long minimum;
    if (movement)
    {
        real limit = movement->unknown0c;
        minimum = (long)(limit > distance ? distance : limit);
        *(long *)((byte *)&request + 0x684) = minimum;
        *(long *)((byte *)&request + 0x688) = (long)movement->unknown10;
    }
    else
    {
        minimum = 0;
        *(long *)((byte *)&request + 0x684) = 0;
        *(long *)((byte *)&request + 0x688) = 100;
    }
    *(real *)((byte *)&request + 0x48) = 2.0f;
    *((bool *)&request + 0x46) = true;
    *(real *)((byte *)&request + 0x4c) = (real)minimum;
    *((bool *)&request + 0x50) = true;
    *((bool *)&request + 0x59) = character && (*character & 1);
    *(s_object_marker **)((byte *)&request + 0x64) = has_markers ? markers : NULL;
    if (state->unknown04 != 0.0f)
        *(real *)((byte *)&request + 0x1c) = state->unknown04;
    if (actor->unknown50c && actor_get(actor_index)->unknown504 != 2 &&
        !REFERENCE_EQUAL(actor->unknown418, g_470fa0))
    {
        *((bool *)&request + 0x692) = true;
        *(s_reference *)((byte *)&request + 0x694) = actor->unknown418;
    }

    byte *scratch = ai_scratch_buffer_get();
    s_261d20_entry entry;
    long other_actor;
    bool unknown;
    s_reference reference = function_261280((s_prop_search *)&request, actor_index,
        &entry, &other_actor, scratch, &unknown);
    if (REFERENCE_EQUAL(reference, g_470fa0) || *(short *)((byte *)&entry + 8) > 1)
    {
        state->unknown00 = true;
        g_46eeb8[0x23]->unknown8 = g_46f348;
        ai_scratch_buffer_release(scratch);
        return result;
    }
    s_262b40_result *location = function_262b40(reference);
    bool available = false;
    bool success = false;
    if (*(short *)((byte *)&entry + 0x5c) != 0 &&
        *(long *)((byte *)location + 0x14) != NONE &&
        *(long *)((byte *)location + 0x14) != 0xffff)
    {
        success = function_1f4a20((s_1f4a20_entry const *)&entry, actor_index, reference,
            (s_1f4a20_source const *)scratch, true, &state->point, &state->facing,
            (signed char *)&state->unknown11, &available);
        if (success)
        {
            state->unknown0f = true;
            state->unknown10 = false;
        }
        else if (available)
            reference = g_470fa0;
    }
    if (!available && !success)
        reference = function_2626b0(actor_index, reference, other_actor, scratch, unknown, true);
    if (!REFERENCE_EQUAL(reference, g_470fa0))
    {
        state->unknown01 = true;
        actor->unknown4ae = true;
        ai_scratch_buffer_release(scratch);
        return result;
    }
    state->unknown00 = true;
    g_46eeb8[0x23]->unknown8 = g_46f348;
    ai_scratch_buffer_release(scratch);
    return result;
}
