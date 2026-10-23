// @flags /O2 /Gr
#include "unknown_11c920.h"
#include "globals.h"

struct s_a0c4_object
{
	byte unknown00[0x2098];
	void *vtable2098;
	byte unknown209c[0xa0ac - 0x209c];
	void *vtable_a0ac;
};

struct s_flagged
{
	byte unknown00[0x29];
	bool field29;
};

byte g_5291a0[4];
byte g_450cb8[4];
byte g_450d60[4];
s_a0c4_object *g_4d87e8;
s_flagged *g_4d87f0;
s_flagged *g_4d87ec;
void *g_4d87d4;
void *g_4d87d8;
void *g_4d87dc;
void *g_4d87fc;
long g_4d87d0;
long g_4d87e0;
byte g_4d87e4;
long g_4d87c4;
long g_4d87c8;
long g_4d87cc;

void __stdcall function_78880(void *p);

// @retail 0x81f80
void function_81f80(void)
{
	function_78880(g_5291a0);

	if (g_4d87e8)
	{
		g_4d87e8->vtable_a0ac = g_450cb8;
		g_4d87e8->vtable2098 = g_450d60;
		g_4d87e8 = 0;
	}
	if (g_4d87f0)
	{
		g_4d87f0->field29 = false;
		g_4d87f0 = 0;
	}
	if (g_4d87ec)
	{
		g_4d87ec->field29 = false;
		g_4d87ec = 0;
	}
	if (g_4d87d4)
	{
		g_4d87d4 = 0;
	}
	if (g_4d87d8)
	{
		g_4d87d8 = 0;
	}
	if (g_4d87dc)
	{
		g_4d87dc = 0;
	}
	if (g_4d87f8)
	{
		delete g_4d87f8;
		g_4d87f8 = 0;
	}
	if (g_4d87fc)
	{
		g_4d87fc = 0;
	}
	g_4d87d0 = 0;
	g_4d87e0 = 0;
	g_4d87e4 = 0;
	g_4d87c4 = 0;
	g_4d87c8 = 0;
	g_4d87cc = 0;
}
#include "unknown_08b110.h"
#include <new>

struct s_081550_fields;
void function_081550(s_081550_fields *fields);

#pragma inline_depth(0)
static __forceinline void function_821c1(s_081550_fields *arg_0)
{
 function_081550(arg_0);
}
#pragma inline_depth(8)
// @retail 0x821c0
long function_821c0(void **arg_f0f1ad, c_replication_view_storage **out_storage)
{
	s_record_pool *views = (s_record_pool *)g_4d87ec;
	void *view = 0;
	c_replication_view_storage *storage = 0;
	long index = record_pool_allocate(views);
	if (index != NONE)
	{
		view = views->data + (index & 0xffff) * 0xb4;
		if (view)
			function_821c1((s_081550_fields *)view);
		if (g_4d87e4)
		{
			s_record_pool *data = (s_record_pool *)g_4d87f0;
			long storage_index = datum_new_at_index_with_salt(data, index);
			storage = (c_replication_view_storage *)(data->data + (storage_index & 0xffff) * 0xad30);
			if (storage)
				new (storage) c_replication_view_storage;
		}
	}
	*arg_f0f1ad = view;
	*out_storage = storage;
	return index;
}

/* the simulation definition registry's two tables (layout as in
   src/unknown_067eb0.cpp, whose 0x67e30 fills it through 0x82240) */
struct s_simulation_definition_registry
{
	long entity_count;
	void *entities[32];
	long event_count;
	void *events[32];
};

/* definition objects of the registry that have no type yet (placeholders) */
struct s_definition_object_4797b0 { void *vtable; };
s_definition_object_4797b0 g_4797b0;
s_definition_object_4797b0 g_4797b4;
s_definition_object_4797b0 g_4797b8;
s_definition_object_4797b0 g_4797bc;
s_definition_object_4797b0 g_4797c0;
s_definition_object_4797b0 g_4797c4;
s_definition_object_4797b0 g_4797d0;
s_definition_object_4797b0 g_4797e0;
s_definition_object_4797b0 g_4797e4;
s_definition_object_4797b0 g_4797ec;
s_definition_object_4797b0 g_4797f0;
s_definition_object_4797b0 g_4797f4;
s_definition_object_4797b0 g_4797f8;
s_definition_object_4797b0 g_479808;
s_definition_object_4797b0 g_479810;
s_definition_object_4797b0 g_479814;
s_definition_object_4797b0 g_479828;
s_definition_object_4797b0 g_479834;
s_definition_object_4797b0 g_479848;

class c_game_engine_player_entity_definition;
class c_game_engine_statborg_entity_definition;
class c_breakable_surface_group_entity_definition;
class c_damage_section_response_event_definition;
class c_breakable_surface_damage_event_definition;
class c_unit_exit_vehicle_event_definition;
class c_unit_melee_damage_event_definition;
class c_vehicle_flip_event_definition;
class c_vehicle_trick_event_definition;
class c_item_type;
class c_projectile_type;
class c_projectile_attached_event;
class c_projectile_effect_event;
class c_projectile_impact_effect_event;
class c_weapon_type;
class c_weapon_fire_event;
class c_weapon_pickup_event;
class c_weapon_put_away_event;
class c_weapon_drop_event;
class c_weapon_effect_event;
class c_device_type;
class c_device_touch_event;
class c_game_engine_event;
class c_game_engine_request_boot_player_event;
extern c_game_engine_player_entity_definition g_4797c8;
extern c_game_engine_statborg_entity_definition g_4797cc;
extern c_breakable_surface_group_entity_definition g_4797dc;
extern c_damage_section_response_event_definition g_4797d4;
extern c_breakable_surface_damage_event_definition g_4797d8;
extern c_unit_exit_vehicle_event_definition g_4797e8;
extern c_unit_melee_damage_event_definition g_4797fc;
extern c_vehicle_flip_event_definition g_479800;
extern c_vehicle_trick_event_definition g_479804;
extern c_item_type g_47980c;
extern c_projectile_type g_479818;
extern c_projectile_attached_event g_47981c;
extern c_projectile_effect_event g_479820;
extern c_projectile_impact_effect_event g_479824;
extern c_weapon_type g_47982c;
extern c_weapon_fire_event g_479830;
extern c_weapon_pickup_event g_479838;
extern c_weapon_put_away_event g_47983c;
extern c_weapon_drop_event g_479840;
extern c_weapon_effect_event g_479844;
extern c_device_type g_47984c;
extern c_device_touch_event g_479850;
extern c_game_engine_event g_479854;
extern c_game_engine_request_boot_player_event g_479858;

// @retail 0x82240
void __stdcall function_82240(s_simulation_definition_registry *registry,
	long *entity_count, long *event_count)
{
	void **entities = registry->entities;
	entities[0] = &g_4797b0;
	entities[1] = &g_4797b4;
	entities[2] = &g_4797b8;
	entities[3] = &g_4797bc;
	entities[4] = &g_4797c0;
	entities[5] = &g_4797c4;
	entities[6] = &g_4797c8;
	entities[7] = &g_4797cc;
	entities[8] = &g_4797dc;
	entities[9] = &g_4797e0;
	entities[10] = &g_47980c;
	entities[11] = &g_479810;
	entities[12] = &g_479814;
	entities[13] = &g_479818;
	entities[14] = &g_47982c;
	entities[15] = &g_479848;
	entities[16] = &g_47984c;
	void **events = registry->events;
	events[0] = &g_4797d0;
	events[1] = &g_4797d4;
	events[3] = &g_47981c;
	events[4] = &g_479820;
	events[5] = &g_479824;
	events[6] = &g_479828;
	events[7] = &g_479854;
	events[8] = &g_4797ec;
	events[9] = &g_479808;
	events[2] = &g_4797d8;
	events[10] = &g_479844;
	events[11] = &g_4797e8;
	events[12] = &g_479830;
	events[13] = &g_479834;
	events[14] = &g_4797f0;
	events[15] = &g_4797f8;
	events[16] = &g_479838;
	events[17] = &g_47983c;
	events[18] = &g_479840;
	events[19] = &g_479800;
	events[20] = &g_479804;
	events[21] = &g_479850;
	events[22] = &g_4797f4;
	events[23] = &g_4797fc;
	events[24] = &g_4797e4;
	events[25] = &g_479858;
	*entity_count = 17;
	*event_count = 26;
}
