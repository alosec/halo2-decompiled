// @flags /O2 /arch:SSE /Gr /Ob1
/* UNKNOWN_10A8B0.CPP: the index of a scenery location among the structure
   bsp's locations. Moved out of scenery.cpp into an /Ob1 file: retail calls
   it out of line from 0x10a820, its only caller. */

#include "unknown_11c920.h"
#include "globals.h"

/* as in scenery.cpp */
struct s_scenery_location
{
	long value_00;
	short value_04;
	byte value_06;
	byte value_07;
};

struct s_bsp_location_entry
{
	long value_00;
	short value_04;
	byte value_06;
	byte value_07;
	long value_08;
};

struct s_bsp_locations
{
	byte unknown00[0x58];
	long count;
	s_bsp_location_entry *entries;
};

// @retail 0x10a8b0
long function_10a8b0(s_scenery_location *location, long value)
{
	long result = NONE;

	if (g_4e0344 && g_4e0344->count > 0)
	{
		s_bsp_locations *locations = g_4e0344->locations;
		long i = 0;
		long count = locations->count;

		for (; i < count; i++)
		{
			s_bsp_location_entry *entry = &locations->entries[i];
			byte match = (entry->value_06 == location->value_06) & (entry->value_07 == location->value_07) & (entry->value_00 == location->value_00);

			match &= 1;
			if (match && !entry->value_07)
				match &= entry->value_04 == location->value_04;
			if (match && entry->value_08 == value)
			{
				result = i;
				goto done;
			}
		}
	}
done:
	return result;
}
