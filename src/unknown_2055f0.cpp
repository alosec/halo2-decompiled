// @flags /O2 /Gr
/* UNKNOWN_2055F0.CPP: what creates a havok contact impact, and the impact
   list of a havok component (src/impacts.cpp) */

#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"
#include "unknown_1cec30.h"
#include "impacts.h"
#include "unknown_2055f0.h"

/* a component's impacts (g_51ec00, 0x40 bytes) */
struct s_havok_component_impacts
{
	short salt;
	short count;
	long impact_indices[15];
};

extern s_record_pool *g_51ec00;

// @retail 0x2055f0
inline void impact_data_set(
	s_impact_data *data,
	bool unknown00,
	long component_a,
	long unknown08,
	c_type_47f957 material_a,
	long component_b,
	long unknown14,
	c_type_47f957 material_b,
	point3f const *position,
	vector3f const *normal,
	long type,
	s_physics_model_shape_key const *shape)
{
	data->unknown00 = unknown00;
	data->component_a = component_a;
	data->unknown08 = unknown08;
	data->material_a = material_a;
	data->component_b = component_b;
	data->unknown14 = unknown14;
	data->material_b = material_b;
	data->position = *position;
	data->normal = *normal;
	data->type = type;
	data->unknown38 = false;
	if (!shape)
	{
		data->shape.type = NONE;
		data->shape.index = NONE;
	}
	else
	{
		data->shape = *shape;
	}
}

// @retail 0x205680
void havok_component_impact_add(
	s_havok_component *component,
	long impact_index)
{
	s_havok_component_impacts *impacts;

	if (component->unknown20 == NONE)
	{
		long impacts_index = record_pool_allocate(g_51ec00);

		((s_havok_component_impacts *)g_51ec00->data)[impacts_index & 0xffff].count = 0;
		component->unknown20 = impacts_index;
	}
	impacts = &((s_havok_component_impacts *)g_51ec00->data)[component->unknown20 & 0xffff];
	impacts->impact_indices[impacts->count] = impact_index;
	impacts->count++;
}

struct s_impact;
extern s_record_pool *g_51ebfc;
void impact_release(long impact_index, s_impact *impact);

struct s_havok_impact_contact;
long havok_component_impact_find(s_havok_component *component, s_havok_impact_contact const *contact, bool check_position);
real impact_distance_squared_to_nearest_player(point3f const *point, long type);
bool havok_component_impact_make_room(long rigid_body_index, s_havok_component *component, real strength);
long impact_new(s_impact_data const *data, long type);
void impact_set_contact(s_impact *impact, s_impact_data const *data, vector3f const *vector, real unknown44, bool unknownf);
void function_2079f0(long const *object_index, long type, bool preserve);

long function_2078f0(long const *object_index, s_impact_data const *data, vector3f const *vector, real scale, bool flag);

// @retail 0x2079f0
void function_2079f0(long const *object_index, long type, bool preserve)
{
	(void)&type;
	(void)&preserve;
	s_impact_object_view *object = ((s_impact_object_header *)g_4e0300->data)[*object_index & 0xffff].object;
	s_havok_component *component = havok_component_get(object->component_index);
	if (!preserve)
	{
		for (long i = 0; ; i++)
		{
			long index = component->unknown20;
			long count = index == NONE ? 0 : ((s_havok_component_impacts *)g_51ec00->data)[index & 0xffff].count;
			if (i >= count)
				break;
			long impact_index = ((s_havok_component_impacts *)g_51ec00->data)[index & 0xffff].impact_indices[i];
			s_component_impact_view *impact = &((s_component_impact_view *)g_51ebfc->data)[impact_index & 0xffff];
			if (impact->count > 0 && impact->type == type)
				impact_release(impact_index, (s_impact *)impact);
		}
	}
}

#if H2_NATIVE_PORT
/* native: emit the inline definition so the guest image can link it */
__attribute__((used)) static void *h2_emit_impact_data_set = (void *)&impact_data_set;
#endif
