// @flags /O2 /Gr /arch:SSE
#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"
#include "unknown_1cec30.h"
#include "impacts.h"
#include "unknown_2055f0.h"

struct s_impact;
extern s_record_pool *g_51ebfc;
void havok_component_impact_add(s_havok_component *component, long impact_index);

struct s_havok_impact_contact;
long havok_component_impact_find(s_havok_component *component, s_havok_impact_contact const *contact, bool check_position);
real impact_distance_squared_to_nearest_player(point3f const *point, long type);
bool havok_component_impact_make_room(long rigid_body_index, s_havok_component *component, real strength);
long impact_new(s_impact_data const *data, long type);
void impact_set_contact(s_impact *impact, s_impact_data const *data, vector3f const *vector, real unknown44, bool unknownf);
void function_2079f0(long const *object_index, long type, bool preserve);

// @retail 0x2078f0
long function_2078f0(long const *object_index, s_impact_data const *data, vector3f const *vector, real scale, bool flag)
{
	(void)&object_index;
	(void)&data;
	(void)&vector;
	(void)&scale;
	(void)&flag;
	s_impact_object_view *object = ((s_impact_object_header *)g_4e0300->data)[*object_index & 0xffff].object;
	s_havok_component *component = havok_component_get(object->component_index);
	long index = havok_component_impact_find(component, (s_havok_impact_contact const *)data, false);
	if (index == NONE)
	{
		real strength = impact_distance_squared_to_nearest_player(&data->position, 1);
		function_2079f0(object_index, data->type, data->unknown38);
		if (!havok_component_impact_make_room(NONE, component, strength))
			goto local_0;
		index = impact_new(data, 1);
		s_component_impact_view *impact = &((s_component_impact_view *)g_51ebfc->data)[index & 0xffff];
		havok_component_impact_add(component, index);
		impact->count++;
	}
	if (index != NONE && !data->unknown38)
	{
		s_component_impact_view *impact = &((s_component_impact_view *)g_51ebfc->data)[index & 0xffff];
		impact_set_contact((s_impact *)impact, data, vector, scale, flag);
	}
local_0:
	return index;
}
