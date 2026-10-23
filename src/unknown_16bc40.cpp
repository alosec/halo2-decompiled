// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_16BC40.CPP: the last used datum of a data array (it follows the
   data array code of unknown_16b570.cpp) and two object queries of the file
   after it (0x16bd10..0x16cfa0) */

#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"
#include "unknown_0259d0.h"

#include <string.h>

static inline long data_next_index_inlined(s_record_pool *data, long datum_index)
{
	long start;
	long index;
	long result;

	if (datum_index == NONE)
	{
		start = 0;
	}
	else
	{
		start = (datum_index & 0xffff) + 1;
	}

	index = data_next_absolute_index_inlined(data, start);
	result = NONE;
	if (index != NONE)
	{
		result = (*(short *)(data->data + data->size * index) << 16) | index;
	}

	return result;
}

// @retail 0x16bc40
long data_last_index(s_record_pool *data)
{
	long datum_index = NONE;
	long result;

	for (;;)
	{
		long next_index;

		result = datum_index;
		next_index = data_next_index_inlined(data, datum_index);
		if (next_index == NONE)
		{
			break;
		}
		datum_index = next_index;
	}

	return result;
}

struct s_object;
s_object *function_badc0(long object_index, dword type_mask);

/* the state at g_510c6c (unknown_1552e0.cpp), as read here */
struct s_16c2b0_state
{
	bool active;
	byte unknown01;
	short mode;
	byte unknown04[0x3c - 0x4];
	long object_index;
};

struct s_unknown_78;
extern s_unknown_78 *g_510c6c;

// @retail 0x16c2b0
long function_16c2b0(void)
{
	long result = NONE;
	s_16c2b0_state *state = (s_16c2b0_state *)g_510c6c;

	if (state->active && state->mode == 4 && function_badc0(state->object_index, 3))
		result = state->object_index;
	return result;
}

extern transform4x3f *g_4687d0;

/* an object, as read here: the offset of its matrix at +0x116 */
struct s_16c6f0_object
{
	byte unknown000[0x116];
	short matrix_offset;
};

struct s_16c6f0_object_header
{
	byte unknown00[8];
	s_16c6f0_object *object;
};

// @retail 0x16c6f0
void function_16c6f0(long object_index, transform4x3f *matrix)
{
	if (function_badc0(object_index, NONE))
	{
		s_16c6f0_object *object = ((s_16c6f0_object_header *)g_4e0300->data)[object_index & 0xffff].object;

		*matrix = *(transform4x3f *)((byte *)object + object->matrix_offset);
	}
	else
	{
		*matrix = *g_4687d0;
	}
}

/* the field of view interpolation of the state at g_510c6c */
struct s_16c740_state
{
	byte unknown00[4];
	real time;
	real duration;
	real current;
	real target;
};

struct s_16c740_flags
{
	byte unknown0[5];
	bool active;
};

struct s_unknown_13bf00;
extern s_unknown_13bf00 *g_510c50;
extern real g_54e854;
extern real g_54e858;
extern real g_54e85c;

#define PIN(x, lo, hi) ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x))

static inline bool real_is_finite(real value)
{
	return (*(long *)&value & 0x7f800000) != 0x7f800000;
}

__forceinline real field_of_view_current(s_16c740_state *state)
{
	real current = g_54e854;
	if (g_510c50 && ((s_16c740_flags *)g_510c50)->active && state->target != 0.0f)
	{
		if (state->time >= state->duration)
		{
			current = state->target;
		}
		else
		{
			real fraction = state->time / state->duration;

			fraction = PIN(fraction, 0.0f, 1.0f);
			real start = state->current;

			current = (state->target - start) * fraction + start;
		}
	}
	return current;
}

// @retail 0x16c740
void __stdcall function_16c740(real value, short ticks)
{
	s_16c740_state *state = (s_16c740_state *)g_510c6c;

	if (state->current == 0.0f)
	{
		state->current = g_54e854;
	}
	real current = field_of_view_current(state);
	real *transition = &state->time;
	state->current = current;
	state->duration = (real)ticks * (1.0f / 30.0f);
	state->target = value * 0.017453292f;
	state->time = 0.0f;
	if (!(real_is_finite(state->target) && state->target >= g_54e858 && state->target <= g_54e85c))
	{
		memset(transition, 0, 4 * sizeof(real));
	}
}
