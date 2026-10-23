/* SCENARIO_RAMPS.H: the scenario's named interpolators (src/interpolators.cpp,
   src/scenario_ramp_globals.cpp). Each scenario interpolator names a tag
   function, the interpolator whose value feeds its accumulated input and the
   one that scales its output; its state moves its value from a start value to
   a target value between a start and an end time of the game clock. */

#ifndef SCENARIO_RAMPS_H
#define SCENARIO_RAMPS_H

#include "unknown_11c920.h"
#include "unknown_246cd0.h" /* s_tag_data */
#include <stddef.h>

enum
{
	k_maximum_interpolators = 16
};

/* one entry of the scenario's interpolator block, 0x18 bytes */
struct s_scenario_interpolator
{
	long name; /* string id; 0 for none */
	byte unknown04[8];
	s_tag_data function;
	short input_index; /* the interpolator whose value accumulates, or NONE */
	short scale_index; /* the interpolator that scales the output, or NONE */
};
typedef char assert_scenario_interpolator_size[(sizeof(s_scenario_interpolator) == 0x18) ? 1 : -1];
typedef char assert_scenario_interpolator_function[(offsetof(s_scenario_interpolator, function) == 0xc) ? 1 : -1];
typedef char assert_scenario_interpolator_scale_index[(offsetof(s_scenario_interpolator, scale_index) == 0x16) ? 1 : -1];

/* the scenario (g_4e0350) as these functions see it: its interpolator block */
struct s_scenario_interpolators_view
{
	byte unknown000[0x3c0];
	long interpolator_count;
	s_scenario_interpolator *interpolators;
};
typedef char assert_scenario_interpolators_view_count[(offsetof(s_scenario_interpolators_view, interpolator_count) == 0x3c0) ? 1 : -1];

/* the state of an interpolator, 0x20 bytes. Times are game seconds. */
struct s_interpolator_state
{
	real value;
	real start_value;
	real target_value;
	real current_time; /* the time of the last update; it stays put while stopped */
	real start_time;
	real end_time;
	real accumulated_input; /* the input interpolator's value, integrated over time */
	byte active : 1;
	byte smooth : 1; /* eases along a cosine curve instead of a straight line */
	byte periodic : 1; /* its function is a periodic one */
	byte : 5;
	byte unknown1d[3];
};
typedef char assert_interpolator_state_size[(sizeof(s_interpolator_state) == 0x20) ? 1 : -1];
typedef char assert_interpolator_state_accumulated_input[(offsetof(s_interpolator_state, accumulated_input) == 0x18) ? 1 : -1];

/* g_4e6740: the name last looked up, then the state of each interpolator */
struct s_interpolator_globals
{
	long last_name;
	s_interpolator_state states[k_maximum_interpolators];
};
typedef char assert_interpolator_globals_size[(sizeof(s_interpolator_globals) == 0x204) ? 1 : -1];

extern s_interpolator_globals *g_4e6740;

void scenario_ramps_initialize(void);
void scenario_ramps_dispose(void);
void scenario_ramps_reset_for_map(void);
void scenario_ramps_update(real delta);
void scenario_ramps_stop_all(void);
void scenario_ramps_resume_all(void);
void scenario_ramps_toggle_last_used(void);

s_interpolator_state *interpolator_get(long name, long *index_out);
real __stdcall scenario_ramp_evaluate(long index, real value);
real scenario_ramp_evaluate_current(long index);
long interpolator_start(long name, real target, real seconds);
long scenario_ramp_start_eased(long name, real target, real seconds);
long interpolator_resume(long name);
bool interpolator_exists(long name);
real scenario_ramp_current_value(long name, bool evaluate);
real scenario_ramp_accumulated_input(long name);
real scenario_ramp_progress(long name);
real scenario_ramp_start_time(long name);
real interpolator_get_end_time(long name);
real scenario_ramp_value_at_fraction(long name, real fraction, bool evaluate);
real scenario_ramp_value_at_time(long name, real time, bool evaluate);
real scenario_ramp_value_after(long name, real seconds, bool evaluate);

#endif
