// @flags /O2 /Gr
/* UNKNOWN_1E1F20.CPP: the actor's weapon (lane M; called by the behaviors of
   0x1a8000..0x1affff) */

#include "unknown_11c920.h"
#include "ai_actor.h"
#include "unknown_2551c0.h"
#include "object_markers.h"
#include "object_queries.h"
#include "unknown_19ec40.h"
#include "unknown_1e46c0.h"
#include <string.h>

struct s_ai_weapon_definition
{
	byte unknown000[0x12f];
	byte flags12f_0 : 1;
};

static inline long unit_get_current_weapon(long unit_index)
{
	s_ai_object *unit = ai_object_get(unit_index);
	short slot = unit->current_weapon;
	long result = NONE;

	if (slot != NONE)
		result = unit->weapons[slot];
	return result;
}

// @retail 0x1e1f20
long function_1e1f20(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	long result = NONE;

	if (actor->unknown268 && *(long volatile *)&actor->unknown274 != NONE)
		result = unit_get_current_weapon(*(long volatile *)&actor->unknown274);
	if (result == NONE && actor->unknown018 != NONE)
		result = unit_get_current_weapon(actor->unknown018);
	return result;
}

PRIVATE inline long secondary_weapon_get(long unit_index)
{
	s_ai_object *unit = ai_object_get(unit_index);
	short slot = *((signed char *)unit + 0x213);
	long result = NONE;
	if (slot != NONE)
		result = unit->weapons[slot];
	return result;
}

// @retail 0x1e1fd0
long function_1e1fd0(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	long unit_index = actor->unknown018;
	long result = NONE;
	if (unit_index != NONE)
		result = secondary_weapon_get(unit_index);
	return result;
}

// @retail 0x1e2030
bool function_1e2030(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	long weapon_index = function_1e1f20(actor_index);
	bool result = false;

	if (weapon_index != NONE)
	{
		s_ai_weapon_definition *definition = (s_ai_weapon_definition *)g_4e3b44[ai_object_get(weapon_index)->definition_index & 0xffff].bytes;
		result = !TEST_FIELD_BIT(definition->flags12f_0);
		if (result && actor->unknown018 != NONE && (ai_object_get(actor->unknown018)->flags19 & 2))
			result = false;
	}
	return result;
}

void __stdcall function_cb500(long unit_index, short mode, const point3f *origin,
	const vector3f *forward, const real *offsets, point3f *point);

struct s_object_position_view
{
	byte field_0[0x30];
	point3f position;
	byte field_3c[0xaa - 0x3c];
	byte type;
};

// @retail 0x1e3b00
void function_1e3b00(long object_index, long mode, point3f const *reference,
	void const *unknown0, void const *unknown1, point3f *position)
{
	s_object_position_view *object = (s_object_position_view *)ai_object_get(object_index);
	const long *mode_reference = &mode;
	const point3f *const *origin_reference = &reference;
	const void *const *local_41f0cc = &unknown0;
	const void *const *offset_reference = &unknown1;
	if ((1 << object->type) & 3)
		function_cb500(object_index, (short)*mode_reference, *origin_reference,
			(const vector3f *)*local_41f0cc, (const real *)*offset_reference, position);
	else
		*position = object->position;
}

struct s_unit_state_c6ef0;
void function_c6ef0(s_unit_state_c6ef0 *state);
void function_c6de0(long object_index, void *control);
void function_118e80(long object_index, vector3f *forward);
void __stdcall function_cbf60(long unit_index, bool active);

/* the standard marker (docs/DECOMPILING.md): (1) with it this body matches
   byte for byte (`ret 4`); without it LTCG passes the actor in eax, and
   when 0x2902b0 and 0xb7b40 are built it passes it in ecx even with the
   actor's address taken. (2) retail holds no reference to 0x1e1250's
   address, and its one caller (0x1e0d50) is LTCG code that pushes the
   argument (`push edi; call 0x1e1250`). (3) tried: taking the argument's
   address (matches alone, lost once the object cycle is built). */
// @retail 0x1e1250 standard
void __stdcall function_1e1250(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	long unit_index = actor->unknown018;
	if (unit_index != NONE)
	{
		*(long *)((byte *)ai_object_get(unit_index) + 0x12c) = NONE;
		actor->unknown018 = NONE;
		function_cbf60(unit_index, false);
	}
}

void __stdcall function_1e1a00(long actor_index, long value);

// @retail 0x1e0d50
void function_1e0d50(long actor_index, long unit_index)
{
	long const *unit_reference = &unit_index;
	s_actor_view *actor = actor_get(actor_index);
	s_ai_object *unit = ai_object_get(*unit_reference);
	long old_actor = *(long *)((byte *)unit + 0x12c);
	if (old_actor != actor_index)
	{
		if (old_actor != NONE)
			function_1e1a00(old_actor, 0);
		if (actor->unknown018 != NONE)
			function_1e1250(actor_index);
		actor->unknown018 = *unit_reference;
		*(long *)((byte *)unit + 0x12c) = actor_index;
		function_cbf60(*unit_reference, true);
	}
}

struct s_actor_unit_state
{
	long name;
	short mode;
	byte field_6[0x28 - 6];
	vector3f forward;
	vector3f first;
	vector3f second;
	byte field_4c[0x7c - 0x4c];
};

struct s_actor_unit_vectors
{
	byte field_0[0x168];
	vector3f first;
	byte field_174[0x18c - 0x174];
	vector3f second;
};

// @retail 0x1e31b0
void function_1e31b0(long unit_index)
{
	s_actor_unit_state state;
	function_c6ef0((s_unit_state_c6ef0 *)&state);
	state.name = 0x6000085;
	state.mode = 1;
	function_118e80(unit_index, &state.forward);
	s_actor_unit_vectors *unit = (s_actor_unit_vectors *)ai_object_get(unit_index);
	state.first = unit->first;
	state.second = unit->second;
	function_c6de0(unit_index, &state);
	function_cbf60(unit_index, false);
}

struct s_actor_object_query
{
	long definition_index;
	dword flags;
	byte field_8[0x14 - 8];
	long parent_index;
	byte field_18[0x28 - 0x18];
	dword location[2];
	byte field_30[0x70 - 0x30];
	vector3f direction;
	byte field_7c[0xaa - 0x7c];
	byte type;
	byte field_ab[0x134 - 0xab];
	dword flags134;
	byte field_138[2];
	short link_offset;
};

PRIVATE inline s_actor_object_query *actor_query_object(long index)
{
	return (s_actor_object_query *)ai_object_get(index);
}

PRIVATE inline long actor_query_root(long index)
{
	long result = NONE;
	while (index != NONE)
	{
		result = index;
		index = actor_query_object(index)->parent_index;
	}
	return result;
}

struct s_actor_object_sample
{
	point3f point;
	point3f center;
	vector3f direction;
	dword location[2];
	vector3f velocity;
};

// @retail 0x1e3a00
void function_1e3a00(long object_index, s_actor_object_sample *sample)
{
	s_actor_object_sample *const *sample_reference = &sample;
	point3f *point_output = &(*sample_reference)->point;
	s_actor_object_query *object = actor_query_object(object_index);
	function_b9dd0(object_index, &(*sample_reference)->center);
	(*sample_reference)->direction = object->direction;
	if ((1 << object->type) & 3)
	{
		s_object_marker marker;
		function_b8d30(object_index, 0x4000095, &marker, 1, false);
		memcpy(point_output, &marker.matrix.position, sizeof(point3f));
	}
	else
		memcpy(point_output, &point_output[1], sizeof(point3f)); /* the sample's center follows its point */
	function_ba1d0(object_index, &(*sample_reference)->velocity, NULL);
	s_actor_object_query *root = actor_query_object(actor_query_root(object_index));
	struct location_pair { long first; long second; };
	*(location_pair *)(*sample_reference)->location = *(location_pair *)root->location;
}

struct s_actor_variant_entry
{
	long value;
	short variant;
	byte field_6[6];
};

struct s_actor_variant_definition
{
	byte field_0[0x2c];
	long count;
	s_actor_variant_entry *entries;
};

// @retail 0x1e06b0
long function_1e06b0(long tag_index)
{
	long result = 0;
	s_actor_variant_definition *definition = (s_actor_variant_definition *)g_4e3b44[tag_index & 0xffff].bytes;
	if (definition->count == 1)
	{
		result = definition->entries[0].value;
		goto done;
	}
	if (definition->count > 1)
	{
		signed char counts[64];
		signed char choices[64];
		memset(counts, 0, sizeof(counts));
		s_actor_iterator iterator;
		function_x66da2b(&iterator, false);
		s_actor_view *actor;
		while ((actor = (s_actor_view *)function_1e46c0(&iterator)) != NULL)
		{
			if (*(long *)((byte *)actor + 0x54) == tag_index && actor->unknown018 != NONE)
			{
				signed char variant = *((signed char *)ai_object_get(actor->unknown018) + 0xb1);
				if (variant >= 0 && variant < 64)
					counts[variant]++;
			}
		}
		short choice_count = 0;
		short minimum = 32767;
		for (short i = 0; i < definition->count; i++)
		{
			s_actor_variant_entry *entry = &definition->entries[i];
			if (entry->value && entry->variant >= 0 && entry->variant < 64)
			{
				short count = counts[entry->variant];
				if (count < minimum)
				{
					choices[0] = (signed char)i;
					choice_count = 1;
					minimum = count;
				}
				else if (count == minimum)
					choices[choice_count++] = (signed char)i;
			}
		}
		if (choice_count > 0)
		{
			dword seed = g_4e7408->unknown0 * 0x19660d + 0x3c6ef35f;
			g_4e7408->unknown0 = seed;
			short choice = (short)(((seed >> 16) * choice_count) >> 16);
			result = definition->entries[choices[choice]].value;
		}
	}
done:
	return result;
}

void swarm_set_team(long perception_index, short team);
void swarm_reset_orientations(long perception_index);

// @retail 0x1e1150
void function_1e1150(long actor_index, short team)
{
 s_actor_view *actor = actor_get(actor_index);
 actor->unknown024 = team;
 if (actor->unknown007)
  swarm_set_team(*(long *)actor->unknown01c, team);
 else if (actor->unknown018 != NONE)
  *(short *)((byte *)ai_object_get(actor->unknown018) + 0x138) = team;
}

// @retail 0x1e3240
void function_1e3240(void)
{
 s_actor_iterator iterator;
 function_x66da2b(&iterator, true);
 while (function_1e46c0(&iterator))
 {
  s_actor_view *actor = actor_get(iterator.actor_index);
  if (actor->unknown007 && *(long *)actor->unknown01c != NONE)
   swarm_reset_orientations(*(long *)actor->unknown01c);
  else
   function_1e31b0(actor->unknown018);
  actor->unknown008 = 1;
 }
}

void function_1f86a0(long actor_index);

// @retail 0x1e22d0
void function_1e22d0(long actor_index, bool conditional)
{
 s_actor_view *actor = actor_get(actor_index);
 function_2628f0(actor_index, g_470fa0);
 if (actor->unknown4ac == 4 || actor->unknown4ac == 5 || actor->unknown4ac == 6)
 {
  if (!conditional || (*(word *)((byte *)actor + 0x4ba) & 0x8000))
   function_1f86a0(actor_index);
 }
 byte *entry = (byte *)actor + 0x402;
 long count = 4;
 do
 {
  if (!conditional || (*(word *)(entry + 2) & 0x8000))
   *(s_reference *)entry = g_470fa0;
  entry += 6;
 } while (--count);
 for (short slot_index = 0; slot_index <= actor->current; ++slot_index)
 {
  short type = actor->slots[slot_index].type;
  s_slot_handler volatile const *handler = g_46eeb8[type];
  t_slot_notify callback = handler->notify34;
  if (callback)
   callback(actor_index, slot_index < 4 ? &actor->slots[slot_index] : NULL, conditional);
 }
}

void swarm_forget_prop(long perception_index, long index);

// @retail 0x1e2150
void function_1e2150(long actor_index, long object_index)
{
 s_actor_view *actor = actor_get(actor_index);
 if (*(long *)((byte *)actor + 0x338) == object_index)
  *(long *)((byte *)actor + 0x338) = NONE;
 if (*(long *)((byte *)actor + 0x32c) == object_index)
  *(long *)((byte *)actor + 0x32c) = NONE;
 if (actor->unknown344 == object_index)
  actor->unknown344 = NONE;
 if (actor->unknown368 == object_index)
 {
  actor->unknown368 = NONE;
  actor->unknown358 = 0;
 }
 if (*(short *)((byte *)actor + 0x722) == 1 && *(long *)((byte *)actor + 0x724) == object_index)
 {
  *(long *)((byte *)actor + 0x724) = NONE;
  *(short *)((byte *)actor + 0x722) = 0;
 }
 if (*(long *)((byte *)actor + 0x7e0) == object_index)
  *(long *)((byte *)actor + 0x7e0) = NONE;
 if (actor->unknown3b4 == object_index)
  actor->unknown3b4 = NONE;
 if (actor->unknown4ac == 7 && *(long *)((byte *)actor + 0x4b8) == object_index)
 {
  actor->unknown4ac = 0;
  actor->unknown4e4 = NONE;
 }
 if (*(short *)((byte *)actor + 0x688) == 1 && *(long *)((byte *)actor + 0x68c) == object_index)
  *(long *)((byte *)actor + 0x68c) = NONE;
 if (*(short *)((byte *)actor + 0x6a0) == 1 && *(long *)((byte *)actor + 0x6a4) == object_index)
  *(long *)((byte *)actor + 0x6a4) = NONE;
 if (*(short *)((byte *)actor + 0x6b0) == 1 && *(long *)((byte *)actor + 0x6b4) == object_index)
  *(long *)((byte *)actor + 0x6b4) = NONE;
 if (actor->unknown007 && *(long *)actor->unknown01c != NONE)
  swarm_forget_prop(*(long *)actor->unknown01c, object_index);
 for (short i = 0; i <= actor->current; ++i)
 {
  s_slot *slot = &actor->slots[i];
  t_slot_release callback = g_46eeb8[slot->type]->release28;
  if (callback) callback(actor_index, i < 4 ? &actor->slots[i] : NULL, object_index);
 }
}

void swarm_creatures_clear_index20(long perception_index);

/* empties a location: no leaf or cluster, in the current structure bsp */
PRIVATE inline void location_make_empty(s_location *location)
{
 location->leaf_index = NONE;
 location->cluster_index = NONE;
 location->bsp_index = g_4686c4;
}

/* the actor's next prop in its list, or NULL at the end */
PRIVATE inline s_prop_node_view *prop_list_take_next(long *prop_index)
{
 s_prop_node_view *prop = NULL;
 if (*prop_index != NONE)
 {
  prop = prop_node_get(*prop_index);
  *prop_index = prop->next_index;
 }
 return prop;
}

// @retail 0x1e23c0
void function_1e23c0(long actor_index)
{
 const long *index_reference = &actor_index;
 s_actor_view *actor = actor_get(*index_reference);
 location_make_empty((s_location *)((byte *)actor + 0x250));
 *(bool *)((byte *)actor + 0x278) = false;
 *(long *)((byte *)actor + 0x28c) = NONE;
 actor_reset_state(*index_reference);
 if (actor->unknown4ac == 2)
  *(long *)((byte *)actor + 0x4c8) = NONE;
 *(long *)((byte *)actor + 0x4fc) = NONE;
 if (actor->unknown007 && *(long *)actor->unknown01c != NONE)
  swarm_creatures_clear_index20(*(long *)actor->unknown01c);
 for (short i = 0; i <= actor->current; ++i)
 {
  s_slot *slot = &actor->slots[i];
  t_slot_proc callback = g_46eeb8[slot->type]->proc30;
  if (callback)
   callback(*index_reference, i < 4 ? &actor->slots[i] : NULL);
 }
 long prop_index = actor_get(*index_reference)->first_prop_index;
 s_prop_node_view *prop;
 while ((prop = prop_list_take_next(&prop_index)) != NULL)
 {
  /* retail calls 0x25d690 here rather than inlining it */
#pragma inline_depth(0)
  byte *prop_block = (byte *)function_25d690((s_prop_datum *)prop);
#pragma inline_depth()
  byte *view = NULL;
  if (prop->view_index != NONE)
  {
   byte *entry = g_502414->data + (prop->view_index & 0xffff) * 0x124;
   if (entry) view = entry + 0x70;
  }
  location_make_empty((s_location *)(prop_block + 0x28));
  *(long *)(prop_block + 0x44) = NONE;
  if (view && *(short *)(view + 0x70) == 1)
  {
   view[0x68] = false;
   view[0x69] = false;
   view[0x4c] = false;
   *(short *)(view + 0x70) = 0;
   view[0x6c] = true;
   view[0x6d] = true;
   view[0x88] = false;
   *(short *)(view + 0x90) = 0;
   *(short *)(view + 0x8a) = 0;
  }
 }
}

bool function_1e13f0(long actor_index);
void function_203d70(long actor_index, short squad_index, bool keep_team);
void function_203ed0(long actor_index, bool keep_count);
void function_203f60(long actor_index);
void function_203fb0(long actor_index);

// @retail 0x1e3400
void function_1e3400(long actor_index, long squad_index)
{
 const long *squad_reference = &squad_index;
 s_actor_view *actor = actor_get(actor_index);
 function_1e22d0(actor_index, false);
 if (*((byte *)actor + 0xa))
  function_203fb0(actor_index);
 else if (*(long *)((byte *)actor + 0x30) != NONE)
  function_203ed0(actor_index, false);
 if (function_1e13f0(actor_index))
 {
  if ((short)*squad_reference != NONE)
  {
   function_203d70(actor_index, (short)*squad_reference, true);
   return;
  }
 }
 else
  *(long *)((byte *)actor + 0x34) = (short)*squad_reference;
 function_203f60(actor_index);
}

void function_26def0(long actor_index, long owner_index);
void function_25aa10(long actor_index, long object_index);

// @retail 0x1e17d0
void function_1e17d0(long actor_index, long object_index)
{
 const long *object_reference = &object_index;
 s_actor_view *actor = actor_get(actor_index);
 if (actor->unknown26c == *object_reference)
 {
  actor->unknown26c = NONE;
  *((byte *)actor + 0x266) = 0;
  *((byte *)actor + 0x267) = 0;
  *(short *)((byte *)actor + 0x270) = 0;
 }
 if (actor->unknown274 == *object_reference)
 {
  actor->unknown274 = NONE;
  actor->unknown268 = 0;
 }
 if (*(long *)((byte *)actor + 0x5ac) == *object_reference)
 {
  *(long *)((byte *)actor + 0x5ac) = NONE;
  *(short *)((byte *)actor + 0x5b0) = NONE;
 }
 if (*(long *)((byte *)actor + 0x348) == *object_reference)
  *(long *)((byte *)actor + 0x348) = NONE;
 if (*(long *)((byte *)actor + 0x300) == *object_reference)
 {
  *(long *)((byte *)actor + 0x300) = NONE;
  *(short *)((byte *)actor + 0x304) = 0;
 }
 if (*(long *)((byte *)actor + 0x2e8) == *object_reference)
  *(long *)((byte *)actor + 0x2e8) = NONE;
 if (*(long *)((byte *)actor + 0x7e4) == *object_reference)
  *(long *)((byte *)actor + 0x7e4) = NONE;
 if (*(long *)((byte *)actor + 0x3f8) == *object_reference)
  function_26def0(actor_index, NONE);
 for (short i = 0; i <= actor->current; ++i)
 {
  s_slot *slot = &actor->slots[i];
  t_slot_release callback = g_46eeb8[slot->type]->release2c;
  if (callback) callback(actor_index, i < 4 ? &actor->slots[i] : NULL, *object_reference);
 }
 function_25aa10(actor_index, *object_reference);
}

void function_26dc20(long actor_index);
void function_11bed0(s_location *location, point3f const *point);
struct s_bsp3d;
extern s_bsp3d *g_4e033c;
long function_14a280(s_bsp3d *bsp, long index, point3f *point);

struct s_actor_leaf_view
{
 short cluster;
 byte field_2[6];
};
struct s_actor_bsp_view
{
 byte field_0[0x30];
 s_actor_leaf_view *leaves;
};

bool actor_update_bsp_locations(long actor_index);

extern void *g_5047f4;
void function_258600(long actor_index);
void function_2694d0(long clump_index, long actor_index);
void record_pool_release(s_record_pool *pool, long index);
void function_1e1740(long actor_index, long object_index);
void function_26dc20(long actor_index);
void swarm_leader_detach(long actor_index);

// @retail 0x1e1a00
void __stdcall function_1e1a00(long actor_index, long value)
{
	long const *index_reference = &actor_index;
	long const *value_reference = &value;
	struct { s_actor_view *actor; s_actor_iterator iterator; } cleanup_state;
	s_actor_view *&actor = cleanup_state.actor;
	unsigned short actor_slot = (unsigned short)*index_reference;
	actor = actor_get(actor_slot);
	for (short slot_index = 0; slot_index <= actor->current; ++slot_index)
	{
		struct { t_slot_proc stop; s_slot *slot; } dispatch;
		dispatch.stop = g_46eeb8[actor->slots[slot_index].type]->stop;
		dispatch.slot = slot_index < 4 ? &actor->slots[slot_index] : NULL;
		if (dispatch.stop)
			dispatch.stop(*index_reference, dispatch.slot);
	}
	*(word *)&actor->current = 0xffff;
	function_258600(*index_reference);
	long memory_index = 0;
	do
	{
		s_slot_memory_entry *memory = &actor->memory[memory_index];
		if (memory->type != NONE)
		{
			byte *entry = g_502424->data + (memory->unknown4 & 0xffff) * 0xbc;
			long slot = *(short *)((byte *)memory + 2);
			*(long *)(entry + slot * 12 + 4) = NONE;
			*(short *)(entry + slot * 12 + 8) = 1;
			memory->type = NONE;
		}
	} while (++memory_index < 4);
	if (*((byte *)actor + 0xa))
		function_203fb0(*index_reference);
	else
		function_203ed0(*index_reference, (byte)*value_reference != 0);
	if (actor->unknown07c != NONE)
		function_2694d0(actor->unknown07c, *index_reference);
	long prop_index = actor_get(*index_reference)->first_prop_index;
	while (prop_index != NONE)
	{
		long current = prop_index;
		byte *prop = g_502418->data + (current & 0xffff) * 0x3c;
		prop_index = *(long *)(prop + 0x2c);
		function_1e2150(*index_reference, current);
		prop = g_502418->data + (current & 0xffff) * 0x3c;
		long view_index = *(long *)(prop + 0x14);
		if (view_index != NONE)
			record_pool_release(g_502414, view_index);
		record_pool_release(g_502418, current);
	}
	actor->first_prop_index = NONE;
	long index = NONE;
	while ((index = data_next_absolute_index_inlined(g_50241c, index + 1)) != NONE)
	{
		byte *prop = g_50241c->data + g_50241c->size * index;
		if (!prop)
			break;
		if (*(long *)(prop + 0x1c) == *index_reference)
			*(long *)(prop + 0x1c) = NONE;
	}
	s_actor_iterator &iterator = cleanup_state.iterator;
	function_x66da2b(&iterator, false);
	s_actor_view *other;
	while ((other = (s_actor_view *)function_1e46c0(&iterator)) != NULL)
	{
		if (other != actor)
			function_1e1740(iterator.actor_index, *index_reference);
	}
	function_2628f0(*index_reference, g_470fa0);
	if (*(long *)((byte *)actor + 0x3f4) != NONE)
		function_26dc20(*index_reference);
	if (actor->unknown007)
	{
		if (*(long *)((byte *)actor + 0x1c) != NONE)
			swarm_leader_detach(*index_reference);
	}
	else
	{
		if (actor->unknown018 != NONE)
		{
			byte *unit = (byte *)ai_object_get(actor->unknown018);
			if (*(byte *)(unit + 0xaa) == 0)
				*(long *)(unit + 0x3d8) = actor->unknown054;
		}
		function_1e1250(*index_reference);
	}
	long tag_index = actor_get(*index_reference)->unknown054;
	byte *definition = NULL;
	while (tag_index != NONE)
	{
		byte *tag = g_4e3b44[tag_index & 0xffff].bytes;
		if (*(long *)(tag + 0x34) > 0)
		{
			definition = *(byte **)(tag + 0x38);
			break;
		}
		tag_index = *(long *)(tag + 8);
	}
	if (definition && ((*(dword *)definition >> 3) & 1))
	{
		byte *counts = (byte *)g_5047f4;
		--*(short *)(counts + 6);
		if ((byte)*value_reference)
		{
			++*(short *)(counts + 8);
			*(long *)(counts + 0xc) = g_510c54->game_time;
		}
		long i = 0;
		do
		{
			if (*(long *)(counts + 0x18 + i * 4) == *index_reference)
			{
				*(long *)(counts + 0x18 + i * 4) = NONE;
				*(short *)(counts + 0x12 + i * 2) = 0;
			}
		} while (++i < 3);
	}
	actor = actor_get(*index_reference);
	if (actor->unknown009)
	{
		actor->unknown009 = false;
		*(long *)((byte *)actor + 0x10) = g_510c54->game_time;
		--*(short *)((byte *)g_4f55d0 + 0x36a);
	}
	record_pool_release(g_4f55f0, *index_reference);
}

long __stdcall function_1dfb90(long definition_index);
void function_1e3b60(long actor_index);

/* the initial team of each actor type */
extern short const g_44aee8[20];

// @retail 0x1e0160
long __stdcall function_1e0160(long squad_index, long entry_index, long unit_index, bool flag)
{
	long actor_index = NONE;
	long resolved_squad = squad_index;
	long const *unit_reference = &unit_index;
	bool const *flag_reference = &flag;
	byte *squad = NULL;
	if (squad_index != NONE)
		squad = g_51e9d8->data + (squad_index & 0xffff) * 0x98;
	if (*unit_reference != NONE && entry_index != NONE)
	{
		actor_index = function_1dfb90(entry_index);
		if (actor_index != NONE)
		{
			s_actor_view *actor = actor_get(actor_index);
			function_1e0d50(actor_index, *unit_reference);
			if (squad_index == NONE)
				function_203f60(actor_index);
			else
			{
				if (!(squad_index & 0xffff0000))
					resolved_squad = (*(short *)squad << 16) | (squad_index & 0xffff);
				function_203d70(actor_index, (short)resolved_squad, false);
			}
			byte *unit = (byte *)ai_object_get(*unit_reference);
			short entry_team = 0;
			if (resolved_squad != NONE)
			{
				byte *entry = *(byte **)((byte *)g_4e0350 + 0x164) + (resolved_squad & 0xffff) * 0x74;
				if (entry)
					entry_team = *(short *)(entry + 0x24);
			}
			short unit_team;
			if (entry_team != 0)
				function_1e1150(actor_index, entry_team);
			else if (unit && (unit_team = *(short *)(unit + 0x138)) != NONE && unit_team != 0)
				function_1e1150(actor_index, unit_team);
			else
				function_1e1150(actor_index, g_44aee8[actor->unknown004]);
			if (squad && !squad[0x76] && actor->unknown024 != 0)
				squad[0x76] = (byte)actor->unknown024;
			actor->unknown084 = *flag_reference ? 0 : 3;
			function_1e3b60(actor_index);
			if (*(short *)((byte *)actor + 0x254) == NONE)
			{
				if (*(long *)((byte *)actor + 0x1c) != NONE)
					swarm_leader_detach(actor_index);
				function_1e1a00(actor_index, 0);
				actor_index = NONE;
			}
		}
	}
	return actor_index;
}

void __stdcall swarm_leader_attach(long actor_index, long perception_index);

// @retail 0x1e02f0
long __stdcall function_1e02f0(long squad_index, long entry_index, long unit_index, long perception_index, bool flag)
{
	long actor_index = NONE;
	long const *unit_reference = &unit_index;
	long const *perception_reference = &perception_index;
	bool const *flag_reference = &flag;
	byte *squad = NULL;
	if (squad_index != NONE)
		squad = g_51e9d8->data + (squad_index & 0xffff) * 0x98;
	if ((*unit_reference != NONE || *perception_reference != NONE) && entry_index != NONE)
	{
		actor_index = function_1dfb90(entry_index);
		if (actor_index != NONE)
		{
			s_actor_view *actor = actor_get(actor_index);
			if (*unit_reference != NONE)
				function_1e0d50(actor_index, *unit_reference);
			else if (*perception_reference != NONE)
				swarm_leader_attach(actor_index, *perception_reference);
			if (squad_index == NONE)
				function_203f60(actor_index);
			else
			{
				if (!(squad_index & 0xffff0000))
					squad_index = (*(short *)squad << 16) | (squad_index & 0xffff);
				function_203d70(actor_index, (short)squad_index, false);
			}
			byte *unit = *unit_reference != NONE ? (byte *)ai_object_get(*unit_reference) : NULL;
			short entry_team = 0;
			if (squad_index != NONE)
			{
				byte *entry = *(byte **)((byte *)g_4e0350 + 0x164) + (squad_index & 0xffff) * 0x74;
				if (entry)
					entry_team = *(short *)(entry + 0x24);
			}
			short unit_team;
			if (entry_team != 0)
				function_1e1150(actor_index, entry_team);
			else if (unit && (unit_team = *(short *)(unit + 0x138)) != NONE && unit_team != 0)
				function_1e1150(actor_index, unit_team);
			else
				function_1e1150(actor_index, g_44aee8[actor->unknown004]);
			if (squad && !squad[0x76] && actor->unknown024 != 0)
				squad[0x76] = (byte)actor->unknown024;
			actor->unknown084 = *flag_reference ? 0 : 3;
			function_1e3b60(actor_index);
			if (*(short *)((byte *)actor + 0x254) == NONE)
			{
				if (*(long *)((byte *)actor + 0x1c) != NONE)
					swarm_leader_detach(actor_index);
				function_1e1a00(actor_index, 0);
				actor_index = NONE;
			}
		}
	}
	return actor_index;
}

#include "unit_requests.h"

bool g_4f55e8;
void function_caf00(long unit_index);
long function_1e4a10(long tag_index);
void function_d5c90(long object_index, real *body_vitality, real *shield_vitality);
bool function_1df5d0(short first, short second);
bool function_1e11b0(long actor_index, bool active);
bool function_114b60(short entry_index, short fallback_index, long unit_index, long priority, void const *extra);

extern bool g_4f55e5;
bool g_4f55e0;
real function_259a0(dword *seed);
long function_1469f0(real seconds);
void *character_grenade_settings_get(long character_index, short key);
long function_cbd50(long unit_index, short weapon_index);
void __stdcall function_c6f80(long unit_index, long ticks, long flags);
void function_101a10(long weapon_index, real fraction);
void function_1018b0(long weapon_index, short const *rounds);

