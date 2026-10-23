// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1D5460.CPP: the impacts of a havok component: each component may
   own a list (g_51ec00) of up to 15 impacts (g_51ebfc, at most 0x20) */

#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"
#include "unknown_1cec30.h"

#define MAXIMUM_IMPACTS 0x20
#define MAXIMUM_IMPACTS_PER_COMPONENT 15

extern s_record_pool *g_51ebfc;
extern s_record_pool *g_51ec00;
extern long g_502138; /* impacts.cpp */

/* an impact (g_51ebfc, 0xa0 bytes) */
struct s_havok_impact
{
	short identifier;
	byte unknown02;
	char unknown03;
	real unknown04;
	byte unknown08[0xa0 - 0x8];
};

/* a component's impacts (g_51ec00, 0x40 bytes) */
struct s_havok_impact_list
{
	short identifier;
	short count;
	long impact_indices[MAXIMUM_IMPACTS_PER_COMPONENT];
};

struct s_havok_impact_contact;

/* lane K's impacts (impacts.cpp) */
struct s_impact;
struct s_impact_data;
bool impact_matches_data(s_impact *impact, s_impact_data const *data, bool check_position);
void function_2266a0(long impact_index);
long impacts_last_sorted(void);

static inline s_havok_impact *havok_impact_get(long impact_index)
{
	return &((s_havok_impact *)g_51ebfc->data)[impact_index & 0xffff];
}

static inline s_havok_impact_list *havok_impact_list_get(long list_index)
{
	return &((s_havok_impact_list *)g_51ec00->data)[list_index & 0xffff];
}

static inline long havok_component_impact_count(s_havok_component const *component)
{
	return component->unknown20 == NONE ? 0 : havok_impact_list_get(component->unknown20)->count;
}

// @retail 0x1d5460
long havok_component_impact_find(s_havok_component *component, s_havok_impact_contact const *contact, bool check_position)
{
	long list_index = component->unknown20;
	long i;

	for (i = 0; i < (list_index == NONE ? 0 : havok_impact_list_get(list_index)->count); i++)
	{
		long impact_index = havok_impact_list_get(list_index)->impact_indices[i];

		if (impact_matches_data((s_impact *)havok_impact_get(impact_index), (s_impact_data const *)contact, check_position))
		{
			return impact_index;
		}
	}
	return NONE;
}

// @retail 0x1d5500
bool havok_component_impact_make_room(long rigid_body_index, s_havok_component *component, real strength)
{
	long weakest = NONE;
	bool result = false;

	if (rigid_body_index == NONE || (component->rigid_bodies.data[rigid_body_index].flags45 & 1))
	{
		if (havok_component_impact_count(component) == MAXIMUM_IMPACTS_PER_COMPONENT)
		{
			long i;

			for (i = 0; i < havok_component_impact_count(component); i++)
			{
				if (havok_impact_get(havok_impact_list_get(component->unknown20)->impact_indices[i])->unknown04 > strength)
				{
					weakest = i;
				}
			}
		}
		if (havok_component_impact_count(component) >= MAXIMUM_IMPACTS_PER_COMPONENT && weakest == NONE)
		{
			goto done;
		}
		if (weakest != NONE)
		{
			function_2266a0(havok_impact_list_get(component->unknown20)->impact_indices[weakest]);
		}
		else if (g_51ebfc->actual_count == MAXIMUM_IMPACTS)
		{
			long impact_index = impacts_last_sorted();

			if (impact_index != NONE && havok_impact_get(impact_index)->unknown04 > strength)
			{
				function_2266a0(impact_index);
				if (g_502138 > 0)
				{
					g_502138--;
				}
				else
				{
					g_502138 = NONE;
				}
			}
		}
		result = g_51ebfc->actual_count < MAXIMUM_IMPACTS &&
			havok_component_impact_count(component) < MAXIMUM_IMPACTS_PER_COMPONENT;
	}
done:
	return result;
}
