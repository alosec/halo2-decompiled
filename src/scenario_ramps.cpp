// @flags /O2 /Ob1 /arch:SSE /Gr
/* SCENARIO_RAMPS.CPP: the scenario's named interpolators (the block at +0x3c0
   of g_4e0350) and their state in g_4e6740 (scenario_ramp_globals.cpp) */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include "scenario_ramps.h"

/* the head of a tag function's data, as in unknown_13b390.cpp: its type, its
   flags (the high nibble: the output kind) and its output bounds */
struct s_function_header
{
	byte type;
	byte flags;
	byte function_type[2];
	union
	{
		real_bounds bounds;
		dword colors[4];
	};
};

/* the function type of a periodic function (unknown_13b390.cpp) */
enum
{
	_function_periodic = 3
};

real function_134c50(real value); /* unknown_1321f0.cpp: a clamped cosine transition */
real game_time_get_seconds(void); /* unknown_146240.cpp */
real function_13b390(void const *function, real input, real range); /* unknown_13b390.cpp: a tag function at an input */
real function_13bb90(s_tag_data const *function, real input, real range); /* unknown_13b390.cpp: the same, mapped to its bounds */

/* an interpolator's function at a value plus its accumulated input, scaled by
   its scale interpolator's own output and mapped to the function's bounds */
// @retail 0x134fe0
real __stdcall scenario_ramp_evaluate(long index, real value)
{
	s_interpolator_globals *globals = g_4e6740;
	s_interpolator_state *state = &globals->states[index];
	real result = 0.0f;
	if (state)
	{
		s_scenario_interpolator *definition = &((s_scenario_interpolators_view *)g_4e0350)->interpolators[index];
		value += state->accumulated_input;
		short scale_index = definition->scale_index;
		long clamped = scale_index < 0 ? 0 : (scale_index > ((s_scenario_interpolators_view *)g_4e0350)->interpolator_count - 1 ? ((s_scenario_interpolators_view *)g_4e0350)->interpolator_count - 1 : scale_index);
		if (clamped == scale_index)
		{
			if (((s_scenario_interpolators_view *)g_4e0350)->interpolators[scale_index].scale_index == NONE)
			{
				s_tag_data *function_data = &definition->function;
				real output = function_13b390(function_data, value, 0.0f);
				s_interpolator_state *scale_state = &globals->states[definition->scale_index];
				real scale = 0.0f;
				if (scale_state)
					scale = scenario_ramp_evaluate(definition->scale_index, scale_state->value);
				result = scale * output;
				s_function_header const *header = (s_function_header const *)function_data->address;
				if (!(header->flags & 0xf0))
				{
					real lo = header->bounds.lo;
					real hi = header->bounds.hi;
					result = result < 0.0f ? 0.0f : (result > 1.0f ? 1.0f : result);
					result = (hi - lo) * result + lo;
				}
			}
		}
		else
			result = function_13bb90(&definition->function, value, 0.0f);
	}
	return result;
}

/* an interpolator's function at its current value */
// @retail 0x134fc0
inline real scenario_ramp_evaluate_current(long index)
{
	s_interpolator_state *state = &g_4e6740->states[index];
	real result = 0.0f;
	if (state)
		result = scenario_ramp_evaluate(index, state->value);
	return result;
}

/* an interpolator's current value, or its function's output there */
// @retail 0x1352e0
real scenario_ramp_current_value(long name, bool evaluate)
{
	long index = NONE;
	s_interpolator_state *state = interpolator_get(name, &index);
	real result = 0.0f;
	if (state)
	{
		real value = state->value;
		if (evaluate)
			result = scenario_ramp_evaluate_current(index);
		else
			result = value;
	}
	return result;
}

__forceinline real interpolator_sample(s_interpolator_state *state, real value)
{
	if (state->smooth)
		value = function_134c50(value);
	else
		value = value < 0.0f ? 0.0f : (value > 1.0f ? 1.0f : value);
	return (1.0f - value) * state->start_value + state->target_value * value;
}

/* the value a fraction of the way from the start to the target value */
// @retail 0x135530
real scenario_ramp_value_at_fraction(long name, real value, bool evaluate)
{
	long index = NONE;
	s_interpolator_state *state = interpolator_get(name, &index);
	real result = 0.0f;
	if (state)
	{
		result = interpolator_sample(state, value);
		value = result;
		if (evaluate)
			result = scenario_ramp_evaluate(index, value);
	}
	return result;
}

/* the value at a game time */
// @retail 0x1355b0
real scenario_ramp_value_at_time(long name, real value, bool evaluate)
{
	long index = NONE;
	s_interpolator_state *state = interpolator_get(name, &index);
	real result = 0.0f;
	if (state && state->end_time > state->start_time)
	{
		value = 0.0f > (value - state->start_time) / (state->end_time - state->start_time) ?
			0.0f : (value - state->start_time) / (state->end_time - state->start_time);
		result = interpolator_sample(state, value);
		value = result;
		if (evaluate)
			result = scenario_ramp_evaluate(index, value);
	}
	return result;
}

/* the value some seconds after the interpolator's current time */
// @retail 0x135680
real scenario_ramp_value_after(long name, real value, bool evaluate)
{
	long index = NONE;
	s_interpolator_state *state = interpolator_get(name, &index);
	real result = 0.0f;
	if (state && state->end_time > state->start_time)
	{
		value += state->current_time;
		value = 0.0f > (value - state->start_time) / (state->end_time - state->start_time) ?
			0.0f : (value - state->start_time) / (state->end_time - state->start_time);
		result = interpolator_sample(state, value);
		value = result;
		if (evaluate)
			result = scenario_ramp_evaluate(index, value);
	}
	return result;
}


/* moves each active interpolator to the game time, then adds each input
   interpolator's value over the tick to the accumulated inputs */
// @retail 0x134e00
void scenario_ramps_update(real delta)
{
	s_interpolator_globals *globals = g_4e6740;
	if (globals)
	{
		s_scenario_interpolators_view *scenario = (s_scenario_interpolators_view *)g_4e0350;
		if (scenario && scenario->interpolator_count > 0)
		{
			real time = g_510c54 && g_510c54->active ? g_510c54->game_time * g_510c54->rate : 0.0f;
			for (long i = 0; i < scenario->interpolator_count; i++)
			{
				s_interpolator_state *state = &globals->states[i];
				s_scenario_interpolator *definition = &scenario->interpolators[i];
				if (state->active)
				{
					real value = 0.0f;
					if (state->end_time > state->start_time)
					{
						real fraction = (time - state->start_time) / (state->end_time - state->start_time);
						if (fraction < 0.0f)
							fraction = 0.0f;
						if (state->smooth)
							fraction = function_134c50(fraction);
						else
							fraction = fraction < 0.0f ? 0.0f : (fraction > 1.0f ? 1.0f : fraction);
						value = (1.0f - fraction) * state->start_value + state->target_value * fraction;
					}
					state->current_time = time;
					state->value = value;
				}
				if (((s_function_header const *)definition->function.address)->type == _function_periodic)
					state->periodic = true;
				else
					state->periodic = false;
			}
			for (long j = 0; j < scenario->interpolator_count; j++)
			{
				s_interpolator_state *state = &g_4e6740->states[j];
				s_scenario_interpolator *definition = &scenario->interpolators[j];
				if (state->active)
				{
					short input = scenario->interpolators[j].input_index;
					long clamped = input < 0 ? 0 : (input > scenario->interpolator_count - 1 ? scenario->interpolator_count - 1 : input);
					if (clamped == input)
					{
						s_scenario_interpolator *source = &scenario->interpolators[definition->input_index];
						if (source->input_index == NONE && source->scale_index == NONE)
							state->accumulated_input += g_4e6740->states[definition->input_index].value * delta;
					}
				}
			}
		}
	}
}

/* the state of the interpolator with a name, and its index */
// @retail 0x134d20
s_interpolator_state *interpolator_get(long name, long *index_out)
{
	s_interpolator_state *result = NULL;
	long index = NONE;
	if (g_4e6740 && g_4e0350 && name)
	{
		s_scenario_interpolators_view *scenario = (s_scenario_interpolators_view *)g_4e0350;
		for (long i = 0; i < scenario->interpolator_count; i++)
		{
			long interpolator_name = scenario->interpolators[i].name;
			if (interpolator_name && interpolator_name == name)
			{
				g_4e6740->last_name = name;
				index = i;
				break;
			}
		}
	}
	if (index != NONE)
	{
		if (index_out)
			*index_out = index;
		result = &g_4e6740->states[index];
	}
	else if (index_out)
		*index_out = NONE;
	return result;
}
/* moves an interpolator from its value to a target over some seconds */
// @retail 0x135110
long interpolator_start(long name, real target, real seconds)
{
	long index = NONE;
	s_interpolator_state *state = interpolator_get(name, &index);
	if (state)
	{
		real time = game_time_get_seconds();
		state->smooth = false;
		state->active = true;
		state->current_time = time;
		state->start_time = time;
		state->start_value = state->value;
		state->target_value = target;
		state->end_time = time + seconds;
		state->accumulated_input = 0.0f;
	}
	return index;
}

/* restarts a stopped interpolator where it stopped */
// @retail 0x135210
long interpolator_resume(long name)
{
	long index = NONE;
	s_interpolator_state *state = interpolator_get(name, &index);
	if (state && !TEST_FIELD_BIT(state->active))
	{
		real time = game_time_get_seconds();
		real elapsed = time - state->current_time;
		state->start_time += elapsed;
		state->end_time += elapsed;
		state->current_time = time;
		state->active = true;
	}
	return index;
}

/* looks an interpolator up; the result is always false */
// @retail 0x135290
bool interpolator_exists(long name)
{
	s_interpolator_globals *globals = g_4e6740;
	if (globals && g_4e0350 && name)
	{
		s_scenario_interpolators_view *scenario = (s_scenario_interpolators_view *)g_4e0350;
		for (long index = 0; index < scenario->interpolator_count; index++)
		{
			long interpolator_name = scenario->interpolators[index].name;
			if (interpolator_name && interpolator_name == name)
			{
				globals->last_name = name;
				break;
			}
		}
	}
	return false;
}

/* the state of an interpolator by name (no retail function: always inlined) */
__forceinline s_interpolator_state *interpolator_find(long name)
{
	s_interpolator_state *result = NULL;
	long index = NONE;
	if (g_4e6740 && g_4e0350 && name)
	{
		s_scenario_interpolators_view *scenario = (s_scenario_interpolators_view *)g_4e0350;
		for (long i = 0; i < scenario->interpolator_count; i++)
		{
			long interpolator_name = scenario->interpolators[i].name;
			if (interpolator_name && interpolator_name == name)
			{
				g_4e6740->last_name = name;
				index = i;
				break;
			}
		}
	}
	if (index != NONE)
		result = &g_4e6740->states[index];
	return result;
}

/* starts an interpolator that eases along a cosine curve */
// @retail 0x135180
long scenario_ramp_start_eased(long name, real target, real seconds)
{
	long index = interpolator_start(name, target, seconds);
	if (index != NONE)
		interpolator_find(name)->smooth = true;
	return index;
}

/* an interpolator's accumulated input */
// @retail 0x135330
real scenario_ramp_accumulated_input(long name)
{
	s_interpolator_state *state = interpolator_find(name);
	real result = 0.0f;
	if (state)
		result = state->accumulated_input;
	return result;
}

/* the game time an interpolator started */
// @retail 0x135450
real scenario_ramp_start_time(long name)
{
	s_interpolator_state *state = interpolator_find(name);
	real result = 0.0f;
	if (state)
		result = state->start_time;
	return result;
}

/* the game time an interpolator reaches its target */
// @retail 0x1354c0
real interpolator_get_end_time(long name)
{
	s_interpolator_state *state = interpolator_find(name);
	real result = 0.0f;
	if (state)
		result = state->end_time;
	return result;
}

/* stops every interpolator */
// @retail 0x135750
void scenario_ramps_stop_all(void)
{
	s_interpolator_globals *globals = g_4e6740;
	if (globals && g_4e0350)
	{
		s_scenario_interpolators_view *scenario = (s_scenario_interpolators_view *)g_4e0350;
		for (long i = 0; i < scenario->interpolator_count; i++)
			globals->states[i].active = false;
	}
}

/* restarts every stopped interpolator where it stopped */
// @retail 0x135790
void scenario_ramps_resume_all(void)
{
	s_interpolator_globals *globals = g_4e6740;
	if (globals)
	{
		s_scenario_interpolators_view *scenario = (s_scenario_interpolators_view *)g_4e0350;
		if (scenario)
		{
			real time = game_time_get_seconds();
			for (long i = 0; i < scenario->interpolator_count; i++)
			{
				s_interpolator_state *state = &globals->states[i];
				if (!state->active)
				{
					real shifted = state->start_time;
					real elapsed = time - state->current_time;
					shifted += elapsed;
					state->start_time = shifted;
					shifted = state->end_time;
					shifted += elapsed;
					state->end_time = shifted;
					state->current_time = time;
					state->active = true;
				}
			}
		}
	}
}

static __forceinline real scenario_ramp_state_value(s_interpolator_state const *state)
{
	real result = 0.0f;
	if (state)
		result = state->value;
	return result;
}

/* 1 for a value below one half, else 0 */
static __forceinline real scenario_ramp_toggle_target(real value)
{
	real result = 0.0f;
	if (value < 0.5f)
		result = 1.0f;
	return result;
}

/* moves the interpolator last looked up to 1 if it is below one half, else
   to 0, over two seconds */
// @retail 0x135820
void scenario_ramps_toggle_last_used(void)
{
	s_interpolator_globals *globals = g_4e6740;
	if (globals && g_4e0350)
	{
		long name = globals->last_name;
		if (name)
		{
			long index;
			s_interpolator_state *state = interpolator_get(name, &index);
			real value = scenario_ramp_state_value(state);
			real target = scenario_ramp_toggle_target(value);
			interpolator_start(name, target, 2.0f);
		}
	}
}

/* how far an interpolator is from its start to its end time, from 0 */
// @retail 0x1353a0
real scenario_ramp_progress(long name)
{
	s_interpolator_state *state = interpolator_find(name);
	real result = 0.0f;
	if (state && state->end_time > state->start_time)
	{
		result = 0.0f > (state->current_time - state->start_time) / (state->end_time - state->start_time) ?
			0.0f : (state->current_time - state->start_time) / (state->end_time - state->start_time);
		if (state->smooth)
			result = function_134c50(result);
	}
	return result;
}
