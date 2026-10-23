/* OBJECT_SCRIPT_VALUES.H: object values the script functions change: an object's
   interpolated values (g_5107f4, set in unknown_10aac0.cpp) and its fade
   (src/unknown_10aca0.cpp). The scripts' attachment helpers are in
   object_attachments.cpp. */

#ifndef OBJECT_SCRIPT_VALUES_H
#define OBJECT_SCRIPT_VALUES_H

#include "unknown_11c920.h"
#include <stddef.h>

#ifndef NUMBEROF
#define NUMBEROF(array) (sizeof(array) / sizeof((array)[0]))
#endif

/* g_5107f4 seen as its interpolated values: each entry blends one named value
   of one object (unknown_10aac0.cpp) */
struct s_interpolated_value
{
	long name;
	real initial_value;
	real final_value;
	long start_time;
	long duration;
};

struct s_interpolated_values
{
	s_interpolated_value entries[32];
	long object_indices[32];
};
typedef char assert_interpolated_values_size[(sizeof(s_interpolated_values) == 0x300) ? 1 : -1];

/* the object (a view of the object data) */
struct s_scripted_object
{
	byte unknown000[0xc0];
	word : 9;
	word has_interpolated_values : 1; /* entries of g_5107f4 name the object */
	word : 6;
	byte unknown0c2[0xf4 - 0xc2];
	real fade_value;
	byte unknown0f8[4];
	real fade_rate; /* added each tick while fading */
	byte unknown100[8];
	char fade_ticks;
	byte unknown109;
	word : 11;
	word fading : 1;
	word : 4;
};
typedef char assert_scripted_object_fade_value[(offsetof(s_scripted_object, fade_value) == 0xf4) ? 1 : -1];
typedef char assert_scripted_object_fade_ticks[(offsetof(s_scripted_object, fade_ticks) == 0x108) ? 1 : -1];
typedef char assert_scripted_object_size[(sizeof(s_scripted_object) == 0x10c) ? 1 : -1];

struct s_scripted_object_header
{
	byte unknown00[8];
	s_scripted_object *object;
};

#define SCRIPTED_OBJECT_GET(index) (((s_scripted_object_header *)g_4e0300->data)[(index) & 0xffff].object)

void object_interpolated_value_clear(long object_index, long name);
void object_interpolated_values_clear_all(long object_index);
void object_fade_start(long object_index, real target, real seconds);

#endif
