// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_10ACA0.CPP: object values the script functions change
   (object_script_values.h): an object's interpolated values (g_5107f4, set in
   unknown_10aac0.cpp) cleared by name or all at once, and an object's fade
   toward a value over a time.

   Named object_scripting.cpp, this source links before unknown_058dd0.cpp
   together with unit_animation_access.cpp, and the full build then allocates
   0x73360's registers differently; it keeps its address name until that
   function is made robust to link order. */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_10a980.h"
#include "object_script_values.h"

inline s_scripted_object *scripted_object_get(long object_index)
{
	return SCRIPTED_OBJECT_GET(object_index);
}

/* forgets the object's interpolated value of a name */
// @retail 0x10aca0
void object_interpolated_value_clear(long object_index, long name)
{
	if (object_index != NONE && name && name != NONE)
	{
		s_interpolated_values *values = (s_interpolated_values *)g_5107f4;
		for (long index = 0; index < 32; index++)
		{
			if (values->object_indices[index] == object_index && values->entries[index].name == name)
				values->object_indices[index] = NONE;
		}
	}
}

/* forgets all the object's interpolated values */
// @retail 0x10ace0
void object_interpolated_values_clear_all(long object_index)
{
	if (object_index != NONE)
	{
		s_scripted_object *object = scripted_object_get(object_index);
		if (TEST_FIELD_BIT(object->has_interpolated_values))
		{
			s_interpolated_values *values = (s_interpolated_values *)g_5107f4;
			for (long index = 0; index < 32; index++)
			{
				if (values->object_indices[index] == object_index)
					values->object_indices[index] = NONE;
			}
			object->has_interpolated_values = false;
		}
	}
}

/* fades the object's value to a target over a time in seconds (both zero
   stops and clears the fade) */
// @retail 0x10ad90
void object_fade_start(long object_index, real target, real seconds)
{
	if (object_index != NONE)
	{
		s_scripted_object *object = scripted_object_get(object_index);
		if (target == 0.0f && seconds == 0.0f)
		{
			object->fading = false;
			object->fade_value = 0.0f;
			object->fade_rate = 0.0f;
			object->fade_ticks = NONE;
		}
		else
		{
			real ticks_real = (real)g_510c54->field_2_3 * seconds;
			long ticks;
			__asm
			{
				fld ticks_real
				fistp ticks
			}
			if (ticks > 0x7f)
				ticks = 0x7f;
			object->fade_ticks = (char)ticks;
			if (object->fade_ticks == 0)
			{
				real *value = &object->fade_value;
				*value = target;
				real *rate = &object->fade_rate;
				*(volatile real *)rate = 0.0f; /* keeps retail's store order; the plain form moves it */
			}
			else
			{
				real *rate = &object->fade_rate;
				*rate = (target - object->fade_value) / object->fade_ticks;
			}
			object->fading = true;
		}
	}
}
