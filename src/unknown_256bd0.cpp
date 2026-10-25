#include "unknown_11c920.h"
#include "globals.h"
#include "slot_handler.h"
#include "object_markers.h"
#include "unknown_0259d0.h"
#include "unknown_2551c0.h"
#include <math.h>

// @flags /O2 /Gr /arch:SSE

struct s_actor_position_state
{
	s_actor_position_state();
	long field_0[0x19];
};
typedef char assert_256bd0_placement_size[(sizeof(s_actor_position_state) == 0x64) ? 1 : -1];
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

// score 211 (was 211): frame 0x114 versus 0x110 and hoisted active byte remain;
// native placement fields 213; actor getter 212.
// slot reference 211; native fields with getter 214.
// const entry result 211; condition-local entry 211.
// candidate skip 211; placement scope 211.
// frame 0x114 versus 0x110, activity caching and saved homes remain;
// marker position workspace 235; placement position reads 253; in-place velocity 212;
// marker/placement 225; marker/velocity 235; placement/velocity 253; all three 225;
// angle field before sign adjustment 230.
// Helper state pointer in two orders 211/211; state reference 211/211;
// activity value 211/211; external activity reference 211/211. Cached byte/frame remain.
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
		byte *local_7b726b = (byte *)&placement;
		*(point3f *)(local_7b726b + 4) = position;
		*(short *)(local_7b726b + 0x10) = NONE;
		real dot = g_468778->y * direction.j + g_468778->x * direction.i;
		if (-1.0f > dot)
			dot = -1.0f;
		else if (dot > 1.0f)
			dot = 1.0f;
		real clamped = -1.0f > dot ? -1.0f : dot > 1.0f ? 1.0f : dot;
		real angle = (real)acos((double)clamped);
		real cross = g_468778->x * direction.j - g_468778->y * direction.i;
		*(real *)(local_7b726b + 0x14) = 0.0f > cross ? 0.0f - angle : angle;
		*(real *)(local_7b726b + 0x18) = 0.0f;
		*(long *)(local_7b726b + 0x1c) = 4;
		*(short *)(local_7b726b + 0x2e) = 1;
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
					velocity.i = direction.i * 0.03f;
					velocity.j = direction.j * 0.03f;
					velocity.k = direction.k * 0.03f;
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
