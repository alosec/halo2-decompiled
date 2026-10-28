#include "unknown_11c920.h"
#include "globals.h"
#include "slot_handler.h"
#include "object_markers.h"
#include "unknown_0259d0.h"
#include "unknown_11cc90.h"
#include "unknown_2551c0.h"
#include "normalize3d.h"
#include <math.h>
#include <stddef.h>

// @flags /O2 /Gr /arch:SSE

/* the placement a spawned creature starts from (filled by its constructor, 0x200e60) */
struct s_actor_position_state
{
	s_actor_position_state();
	long flags;
	point3f position;
	short field10;
	byte unknown12[2];
	real facing;
	real field18;
	long field1c;
	byte unknown20[0xe];
	short field2e;
	byte unknown30[0x64 - 0x30];
};
typedef char assert_256bd0_placement_size[(sizeof(s_actor_position_state) == 0x64) ? 1 : -1];
typedef char assert_256bd0_placement_facing[(offsetof(s_actor_position_state, facing) == 0x14) ? 1 : -1];
typedef char assert_256bd0_placement_field2e[(offsetof(s_actor_position_state, field2e) == 0x2e) ? 1 : -1];
struct s_290c50_iterator
{
	long unknown00;
	s_record_pool_iterator records;
	long object_index;
};
struct s_28dab2;
struct s_203780;
extern short g_470c40, g_470c44;
extern point2f *g_468778;
real function_30bf0(vector3f *vector);
real function_11cc90(vector2f const *a, vector2f const *b);
long function_1e4990(long index);
void swarm_iterator_new(s_290c50_iterator *iterator);
long function_28dbd0(long character_index, long perception_index, long definition_index, s_28dab2 const *placement, bool flag);
long __stdcall function_1e0850(long character_index, long squad_index, s_203780 const *placement, bool substitute);
bool function_1e11b0(long actor_index, bool active);
void __stdcall function_b77d0(long object_index, vector3f const *linear, vector3f const *angular);
void swarm_creatures_set_object_flag(long perception_index, bool flag);
void function_b7360(long object_index);

static inline byte *perception_iterator_next_256bd0(s_290c50_iterator *iterator)
{
	byte *entry = NULL;
	if (g_4f55d0->active && data_datum_iterator_next((s_data_datum_iterator *)iterator))
		entry = (byte *)iterator->unknown00;
	iterator->object_index = iterator->records.datum_index;
	return entry;
}

/* the flags word at +0x10a of an object */
struct s_object_flags_256bd0
{
	word unknown0 : 6;
	word bit6 : 1;
};
// score 205 (was 206): frame 0x114/0x110 from the hoisted ai-active byte (cause below), then
// scratch-register rotation, the direction copy store order and the angle helper operand pairing.
// Frame 0x114 versus 0x110: our build hoists the ai globals' active byte out
// of the iterator loop into a stack byte, where retail re-reads it after every 0x6b380 call (its
// compiler treats that call as possibly writing it; src/unknown_2003a0.cpp reaches the same reload
// only through a volatile read, which is no longer allowed); every stack offset then differs by 4.
// Also: the 0x28dbd0 call passes the character in EAX where retail passes the variant (BLOCKED-BY
// 0x28dbd0); retail selects the entry with cmovne; actor/point/selection register homes.
// Tried: native placement fields 213; actor getter 212; slot reference 211; native fields with getter
// 214; const entry result 211; condition-local entry 211; candidate skip 211; placement scope 211;
// before sign adjustment 230; helper state pointer in two orders 211/211; state reference 211/211;
// activity value 211/211; external activity reference 211/211; actual datum cursor with separate
// index output (pointer, void result, read-only result, reference output, each in both orders) 211;
// shared native datum result/position value/read-only result/read-only position value
// 211/260/211/260; caller-only native result/position value 211/260; complete native/read-only datum
// values 257/257; the signed angle as a call to the existing 0x11cc90 206 (kept), with the placement
// written through named fields; entry selected by a ternary 263; the iterator step as a plain
// function in swarm.cpp 206 (still hoisted); CRT position/direction/both copies 211/211/211;
// placement copy alone/with position/with direction/all copies 211 each; all copies with reused
// actor offset 254; native selected/object/variant/character handles 211 each, the pairs 211, all
// four 211, all with double minimum comparison 306; native actor/spawned-actor/unit metadata 206
// each, all pairings 206, complete native and read-only native records 206/206. Round 95: the step's
// two tests nested 206; the velocity through the existing vector3f_scale 206 (identical code, kept);
// the step selecting the entry with a ternary inside the active test 263 (frame 0x110 through a
// branchless select, the active byte still spilled); the next result in a named bool 206; the loop
// distance through vector3d_from_points3d and length_sq3f 208.
// Round 180: Shared point setters for source/placement/both 310/292/318; native direction
// initializer 251; with source/placement/both setters 275/310/288; selected point value 279.
// Round 344: unsigned-word status 206; per-arm object result 206; per-arm status 272;
// word status with object/status/both arms 206/272/272; both arms with signed status 272;
// full unsigned status with both arms 275. All creation-call bindings preserved.
// Round 153: Character/variant/both native admission exits 206/206/206; variant/character/both
// fields at every use 205/202/273; both exits with variant/both fields 205/273. On the fresh
// character-field graph, nested step tests/native entry select/shared displacement 202/274/202.
// Four-point 202 rejected; body/helper restored.
// Round 187: placement angle first 253; angle after position 249; angle last 251;
// constants first 250; descending fields 243; angle then descending fields 269;
// short stores first 215; position then descending constants 251. Original order restored.
// Displacement producer orders xyz/xzy/yzx/zxy/zyx 206 each; all three named squares
// after the deltas/interleaved with each delta 205/205; successive z-y-x additions 208.
// One-count forms rejected; original arithmetic and helper restored.
// Round 63: object/selected handles declared the other way 205; result read after the timer pointer 205;
// minimum declared before the flags query 210; entry position through a named point pointer 205; the
// variant creation and the spawn as if/else after the flags block 208.
// Round 102: the cause of the hoist: our link proves the iterator's address never escapes, so the
// iterator stores cannot reach g_4f55d0->active; retail reloads it each pass. A diagnostic
// `s_290c50_iterator *const *` read of the step helper's parameter (stores &iterator) gives
// retail's frame 0x110 and loop: 187 fast (aligned 112 -> 67), not kept. The object index copied
// only while active (retail skips the copy when inactive) 209; with the diagnostic 187; the step
// helper forceinline 205; both 209.
// @retail 0x256bd0
short __stdcall function_256bd0(long actor_index, s_slot *slot, bool active)
{
	byte *actor = g_4f55f0->data + (actor_index & 0xffff) * 0x888;
	short result = g_470c44;
	short *timer = (short *)((byte *)slot + 0xc);
	--*timer;
	if (*timer <= 0)
	{
		s_object_marker marker;
		point3f position;
		if (function_b8d30(*(long *)(actor + 0x18), 0xf0005b4, &marker, 1, false) > 0)
			position = marker.matrix.position;
		else
			position = *(point3f *)(actor + 0x22c);
		vector3f direction = *(vector3f *)(actor + 0x290);
		if (direction.k != 0.0f)
		{
			direction.k = 0.0f;
			if (function_30bf0(&direction) == g_45dbd8)
				direction = *g_4687a8;
		}
		s_actor_position_state placement;
		placement.position = position;
		placement.field10 = NONE;
		placement.facing = function_11cc90((vector2f const *)g_468778, (vector2f const *)&direction);
		placement.field18 = 0.0f;
		placement.field1c = 4;
		placement.field2e = 1;
		byte *unit = *(byte **)(g_4e0300->data + (*(long *)(actor + 0x18) & 0xffff) * 12 + 8);
		byte *definition = g_4e3b44[*(long *)unit & 0xffff].bytes;
		long character_index = *(long *)(definition + 0x30c);
		if (character_index != NONE)
		{
			long variant = *(long *)(g_4e3b44[character_index & 0xffff].bytes + 0x18);
			if (variant != NONE)
			{
				long object_index = NONE;
				long selected = NONE;
				byte *flags = (byte *)function_1e4990(character_index);
				if (flags && (flags[0] & 1))
				{
					real minimum = 25.0f;
					s_290c50_iterator iterator;
					swarm_iterator_new(&iterator);
					byte *entry;
					while ((entry = perception_iterator_next_256bd0(&iterator)) != NULL)
					{
						real y = position.y - *(real *)(entry + 0xc);
						real x = position.x - *(real *)(entry + 8);
						real z = position.z - *(real *)(entry + 0x10);
						real distance = z * z + y * y + x * x;
						if (minimum > distance)
						{
							minimum = distance;
							selected = iterator.object_index;
						}
					}
					if (selected != NONE)
						object_index = function_28dbd0(variant, selected, character_index, (s_28dab2 const *)&placement, false);
				}
				if (selected == NONE)
				{
					long spawned = function_1e0850(character_index, *(long *)(actor + 0x30), (s_203780 const *)&placement, false);
					if (spawned != NONE)
					{
						byte *other = g_4f55f0->data + (spawned & 0xffff) * 0x888;
						if (*(long *)(actor + 0x30) == NONE)
							function_1e11b0(spawned, true);
						long perception = *(long *)(other + 0x1c);
						if (perception != NONE)
							object_index = *(long *)(g_5044c8->data + (perception & 0xffff) * 0x34 + 0x18);
					}
				}
				if (object_index != NONE)
				{
					vector3f velocity;
					vector3f_scale(&direction, 0.03f, &velocity);
					function_b77d0(object_index, &velocity, 0);
				}
			}
		}
		actor = g_4f55f0->data + (actor_index & 0xffff) * 0x888;
		if (actor[7])
		{
			long perception = *(long *)(actor + 0x1c);
			if (perception != NONE)
				swarm_creatures_set_object_flag(perception, true);
		}
		else
		{
			long unit_index = *(long *)(actor + 0x18);
			s_object_flags_256bd0 *flags = (s_object_flags_256bd0 *)(*(byte **)(g_4e0300->data + (unit_index & 0xffff) * 12 + 8) + 0x10a);
			flags->bit6 = true;
			function_b7360(*(long *)(actor + 0x18));
		}
		result = g_470c40;
	}
	return result;
}
