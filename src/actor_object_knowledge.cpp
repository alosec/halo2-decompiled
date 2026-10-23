// @flags /O2 /arch:SSE /Gr
/* ACTOR_OBJECT_KNOWLEDGE.CPP: bringing an actor's prop of an object up to date
   without perceiving it (docs/unknown_25d020.md) */

#include "unknown_11c920.h"
#include "globals.h"
#include "props.h"
#include "unknown_0259d0.h"
#include "unknown_25d020.h"

struct s_object_motion_view;

long function_25d810(long object_index, long actor_index, bool create);
bool __stdcall function_25c230(long actor_index, long prop_ref_index, short priority);
bool function_25da40(s_prop_datum *datum);
void function_25c860(long prop_ref_index);
void function_25c4e0(long prop_ref_index);
void function_25b620(long prop_ref_index, long actor_index, bool noticed);
void function_2640c0(long object_index, s_object_motion_view *result);
real function_30bf0(vector3f *v);

/* the view of a prop_ref's tracking, if it has one */
PRIVATE __forceinline s_type_f95cd3 *prop_ref_tracking_view(s_prop_datum *datum)
{
	s_type_f95cd3 *result = NULL;
	if (datum->tracking_index != NONE)
	{
		s_type_e5ff81 *tracking = tracking_get(datum->tracking_index);
		if (tracking)
		{
			result = &tracking->view;
		}
	}
	return result;
}

PRIVATE __forceinline long prop_ticks_round(real ticks_real)
{
	long ticks;

	__asm
	{
		fld ticks_real
		fistp ticks
	}
	return ticks;
}

/* the squared distance between two points */
PRIVATE __forceinline real point_distance_squared(point3f const *a, point3f const *b)
{
	vector3f offset;
	vector3d_from_points3d(a, b, &offset);
	return offset.i * offset.i + offset.j * offset.j + offset.k * offset.k;
}

// @retail 0x25d020
bool __stdcall function_25d020(long actor_index, long object_index, long type, long prop_ref_argument,
	long reference_actor_index, long reference_index)
{
	/* retail reads the prop_ref from its argument slot at each use */
	long volatile &prop_ref_index = prop_ref_argument;
	bool result = false;
	s_prop_datum *reference = NULL;
	short priority = 2;
	bool noticed = false;
	if (reference_index != NONE)
	{
		reference = prop_ref_get(reference_index);
		priority = reference->unknown1a;
	}
	if (prop_ref_index == NONE)
	{
		long found = function_25d810(object_index, actor_index, true);
		prop_ref_index = found;
		if (found == NONE)
			goto not_found;
	}
	s_prop_datum *datum = prop_ref_get(prop_ref_argument);
	s_type_5cfb45 *state = function_25d690(datum);
	s_type_f95cd3 *view = prop_ref_tracking_view(datum);
	if (!(datum->state >= 1 && datum->state <= 2))
	{
		if (datum->state < 1)
		{
			function_25c230(actor_index, prop_ref_index, priority);
			view = prop_ref_tracking_view(datum);
			state = function_25d690(datum);
			noticed = true;
		}
		if (function_25da40(datum))
		{
			if (noticed)
				function_25c860(prop_ref_index);
			if (reference_index != NONE)
			{
				s_type_5cfb45 *reference_state = function_25d690(reference);
				if (state != reference_state && reference_state->unknown00 > state->unknown00)
				{
					*state = *reference_state;
					if (view)
					{
						s_type_f95cd3 *reference_view = function_25d740((s_prop_node *)reference);
						if (reference_view)
							view->unknown94 = reference_view->unknown94;
					}
					s_2641c0 sample;
					if (function_2641c0(actor_index, &sample, &state->position))
						datum->unknown28 = distance3d(&sample.field_c, &state->position);
				}
			}
			else if (state->unknown00 < g_510c54->game_time)
			{
				s_2640c0 motion;
				s_2641c0 sample;
				function_2640c0(datum->object_index, (s_object_motion_view *)&motion);
				if (function_2641c0(actor_index, &sample, &function_25d690(datum)->position))
					function_264330(actor_index, prop_ref_index, &sample, &motion, true);
			}
			if (view)
			{
				view->unknown8e = (short)prop_ticks_round(g_510c54->field_2_3 * 30.f);
				real radius;
				if (datum->unknown28 < 10.f)
					radius = 1.5f;
				else
					radius = (datum->unknown28 - 10.f) * 0.5 + 1.5;
				if (!noticed)
				{
					if (!view->unknowna2 || point_distance_squared(&state->position, &view->unknowna4) > radius * radius)
					{
						function_25c4e0(prop_ref_index);
						if (view->unknowna2)
						{
							vector3d_from_points3d(&view->unknowna4, &state->position, &view->unknown94);
							function_30bf0(&view->unknown94);
						}
						else
							view->unknown94 = *g_4687a4;
						view->unknowna4 = state->position;
						view->unknowna2 = true;
					}
				}
			}
		}
		switch ((short)type)
		{
		case 2:
		{
			long prop = prop_ref_index;
			function_25b620(prop, actor_index, noticed);
			break;
		}
		}
	}
	if (view)
		*(real *)((byte *)view + 0x3c) = function_265d30(actor_index, prop_ref_index);
	result = true;
not_found:
	return result;
}
