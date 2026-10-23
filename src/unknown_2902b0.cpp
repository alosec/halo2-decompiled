// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_2902B0.CPP: placing one creature group at a slot of the table at
   g_5044c4: its placement is read from an object marker (0x2907d0), then
   either a group record (0x28dab0) or an object (0xb7b40) is created with
   its actor, the actor is set up, and the slot is filled. */
#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "unknown_20fe20.h"
#include "unknown_11cc90.h"
#include "unit_requests.h"
#include "squads.h"
#include "effects.h"
#include "slot_handler.h"
#include <math.h>

struct s_actor_position_state;

/* the placement 0x2907d0 fills (s_actor_position_state, 0x64 bytes), read here without its constructor */
struct s_2902b0_placement
{
	long flags;
	point3f position;
	short field10;
	byte unknown12[2];
	real field14;
	real field18;
	long field1c;
	short field20;
	short field22;
	short field24;
	byte unknown26[2];
	short field28;
	short field2a;
	short field2c;
	short field2e;
	long field30;
	long field34;
	real field38;
	short field3c;
	short field3e;
	byte field40;
	byte unknown41[0x60 - 0x41];
	short field60;
	byte unknown62[2];
};

/* a slot of the table at g_5044c4 (0x50 bytes) */
struct s_2902b0_slot
{
	long object_index;
	short marker_index;
	byte unknown06[2];
	transform4x3f transform;
	long definition_index;
	real field_40;
	real field_44;
	long record_index;
	short count;
	short ticks;
};

/* a record of g_5044c8 (0x34 bytes) */
struct s_2902b0_record
{
	byte unknown00[0x18];
	long field_18;
	byte unknown1c[0x31 - 0x1c];
	bool field_31;
	byte unknown32[2];
};

/* the object creation data 0xb7930 fills and 0xb7b40 reads (0xc4 bytes) */
struct s_2902b0_creation
{
	long tag_index;
	byte unknown04[0xc - 0x4];
	long field_0c;
	byte unknown10[0x1c - 0x10];
	point3f position;
	vector3f forward;
	vector3f up;
	byte unknown40[0xc4 - 0x40];
};

/* the actor fields read here */
struct s_2902b0_actor
{
	byte unknown000[0x18];
	long unit_index;
	long record_index;
	byte unknown020[0x54 - 0x20];
	long field_54;
	byte unknown058[0x86 - 0x58];
	short field_86;
	byte unknown088[0x22a - 0x88];
	bool field_22a;
};

/* the entries of the block at +0x98 of g_4e0350 (40 bytes) */
struct s_2902b0_palette_entry
{
	long unknown0;
	long tag_index;
	byte unknown8[0x28 - 0x8];
};

struct s_2902b0_scenario
{
	byte unknown00[0x98];
	long palette_count;
	s_2902b0_palette_entry *palette;
};

/* the facing request (type 0x22) */
struct s_2902b0_facing_request
{
	long type;
	point3f point;
	real facing_i;
	real facing_j;
	bool flag;
	byte unknown19[0x20 - 0x19];
};

struct s_2902b0_object_header
{
	byte unknown0[8];
	byte *object;
};

struct s_swarm_spawn_slot;
extern s_swarm_spawn_slot *g_5044c4;
extern s_record_pool *g_5044c8;

bool function_2907d0(long arg_0, short arg_1, transform4x3f const *arg_2, s_actor_position_state *arg_3);
long function_1e4990(long index);
long function_1e4a50(long index);
struct s_28dab2;
long __stdcall function_28dab0(long arg_0, s_28dab2 const *arg_1);
long __stdcall function_1e02f0(long squad_index, long entry_index, long unit_index, long perception_index, bool flag);
long __stdcall function_1e0160(long squad_index, long entry_index, long unit_index, bool flag);
void function_1e04a0(long primary_tag, long character_index, long unit_index, long secondary_tag, short grenade_type);
void function_b7930(void *data, long tag_index, long object_index, s_effect_owner const *owner);
long __stdcall function_b7b40(void *creation);
void __stdcall function_b8540(long object_index);
void __stdcall swarm_delete(long arg_0);
long function_1e06b0(long tag_index);
void function_11dfb0(vector2f const *rotation, vector3f *forward, vector3f *up);
bool function_1f34b0(long actor_index, point3f const *point, vector3f const *normal, real distance, short ticks);
void function_b9fc0(long object_index, vector3f *forward, vector3f *up);
real function_30bf0(vector3f *v);
point3f *function_b9dd0(long object_index, point3f *result);
long function_cbd50(long unit_index, short weapon_index);
void function_b9c60(long object_index, bool flag);
void function_a7b30(long index);
bool function_1e11b0(long actor_index, bool active);
void function_2909b0(short arg_0, long arg_1);

static __forceinline byte *object_data_get(long object_index)
{
	return ((s_2902b0_object_header *)g_4e0300->data)[object_index & 0xffff].object;
}

static __forceinline s_2902b0_actor *slot_actor_get(long actor_index)
{
	return (s_2902b0_actor *)actor_datum_get(actor_index);
}

/* a tag index from the palette at +0x98 of g_4e0350, NONE when the index is out of range */
static __forceinline long palette_tag_get(short index)
{
	s_2902b0_scenario *scenario = (s_2902b0_scenario *)g_4e0350;
	if (index != NONE && index >= 0 && index < scenario->palette_count)
		return scenario->palette[index].tag_index;
	return NONE;
}

// Disabled: enabling it alone costs 16 matched functions (b6df0, b7290, b8600, b8890, d6bc0, 10a660, 10cd50, 11a320, 11b520, 1c35f0, 1c3770, 1c39c0, 1ca290, 1d1b60, 1e0d50, 1e1250); with 0xb7b40 (and 0xb7430) enabled too it costs 3: d6bc0, 11a320 (0xd6800 takes the object in ecx; retail pushes all four, ret 0x10) and 1ca290 (ecx/edx; retail esi and the stack) (build/codex-report.md, rounds 27 and 28).
#if 0
/* a real drawn between lower and upper from a seed */
static __forceinline real random_real_range(dword *seed, real lower, real upper)
{
	real random = function_x82e52f(seed, NULL, 0);
	return lower + (upper - lower) * random;
}

// @retail 0x2902b0 disabled
void __stdcall function_2902b0(long arg_0, long arg_1, long arg_2, long arg_3,
	real arg_4, real arg_5, transform4x3f const *arg_6, short arg_7)
{
	s_2902b0_creation creation;
	s_unit_request request;
	point3f point;
	s_2902b0_placement placement;
	s_2902b0_facing_request facing_request;
	vector3f direction;
	s_2902b0_slot *slot = &((s_2902b0_slot *)g_5044c4)[arg_7];
	bool placed = function_2907d0(arg_2, (short)arg_3, arg_6, (s_actor_position_state *)&placement);
	if ((short)arg_1 <= 1)
	{
		placement.field1c = 2;
		placement.field2e = 3;
	}
	if (!placed)
		return;

	byte *definition = g_4e3b44[arg_0 & 0xffff].bytes;
	long object_index = NONE;
	long record_index = NONE;
	long actor_index;
	long info = function_1e4990(arg_0);
	if (!info)
		return;
	if (*(byte *)info & 1)
	{
		record_index = function_28dab0(arg_0, (s_28dab2 const *)&placement);
		if (record_index == NONE)
			return;
		actor_index = function_1e02f0(NONE, arg_0, NONE, record_index, false);
	}
	else
	{
		function_b7930(&creation, *(long *)(definition + 0x10), NONE, NULL);
		function_210850((s_type_c3b527 const *)&placement.position, &creation.position);
		creation.field_0c = placement.field30 ? placement.field30 : function_1e06b0(arg_0);
		function_11dfb0((vector2f const *)&placement.field14, &creation.forward, &creation.up);
		object_index = function_b7b40(&creation);
		if (object_index == NONE)
			return;
		long primary_tag = palette_tag_get(placement.field22);
		long secondary_tag = palette_tag_get(placement.field24);
		short grenade_type = NONE;
		if (placement.field2c - 1 != NONE)
			grenade_type = placement.field2c - 1;
		function_1e04a0(primary_tag, arg_0, object_index, secondary_tag, grenade_type);
		actor_index = function_1e0160(NONE, arg_0, object_index, false);
	}
	if (actor_index != NONE)
	{
		s_2902b0_actor *actor = slot_actor_get(actor_index);
		if (placement.field1c & 1)
			actor->field_86 = 0;
		if (placement.field38 > 0.0f)
		{
			function_210850((s_type_c3b527 const *)&placement.position, &point);
			direction.i = (real)(cos(placement.field14) * cos(placement.field18));
			direction.j = (real)(sin(placement.field14) * cos(placement.field18));
			direction.k = (real)sin(placement.field18);
			function_1f34b0(actor_index, &point, &direction, placement.field38, NONE);
		}
		if (actor->unit_index != NONE)
		{
			switch (placement.field3e)
			{
			case 1:
			{
				function_b9fc0(actor->unit_index, &direction, NULL);
				direction.k = 0.0f;
				if (function_30bf0(&direction) > 0.0f)
				{
					facing_request.type = 0x22;
					function_b9dd0(actor->unit_index, &facing_request.point);
					facing_request.facing_i = direction.i * -1.0f;
					facing_request.facing_j = direction.j * -1.0f;
					facing_request.flag = true;
					function_e6900(actor->unit_index, (s_unit_request *)&facing_request);
				}
				break;
			}
			case 2:
			{
				long weapon = function_1e4a50(actor->field_54);
				if (weapon && (*(byte *)weapon & 0x20))
				{
					request.type = 0x2c;
					request.arguments[0] = true;
					function_e6900(actor->unit_index, &request);
				}
				break;
			}
			}
		}
		long unit_index = actor->unit_index;
		if (actor->unit_index != NONE)
		{
			long weapon_index = function_cbd50(unit_index, (char)object_data_get(unit_index)[0x212]);
			if (weapon_index != NONE)
			{
				byte *weapon_tag = g_4e3b44[*(long *)object_data_get(weapon_index) & 0xffff].bytes;
				if (*(long *)(weapon_tag + 0x2f4) != NONE)
					actor->field_22a = true;
			}
		}
		if (unit_index != NONE && (placement.field1c & 0x10))
			function_b9c60(unit_index, true);
		function_a7b30(object_index);
		actor = slot_actor_get(actor_index);
		function_1e11b0(actor_index, true);
		long record = actor->record_index;
		if (record != NONE)
		{
			s_2902b0_record *group = &((s_2902b0_record *)g_5044c8->data)[record & 0xffff];
			group->field_31 = true;
			slot->transform = *arg_6;
			slot->marker_index = (short)arg_3;
			slot->object_index = arg_2;
			slot->record_index = record;
			slot->definition_index = arg_0;
			slot->field_40 = arg_4;
			slot->count = (short)arg_1;
			slot->field_44 = arg_5;
			real delay = random_real_range(&g_4e7408->unknown0, 0.1f, 0.5f) * g_510c54->field_2_3;
			long ticks;
			__asm
			{
				fld delay
				fistp ticks
			}
			slot->ticks = (short)ticks;
			function_2909b0(arg_7, group->field_18);
		}
	}
	else if (object_index != NONE)
		function_b8540(object_index);
	else if (record_index != NONE)
		swarm_delete(record_index);
}

#endif
