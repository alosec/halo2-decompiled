#if H2_NATIVE_PORT

// @flags /O2 /Gr

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_184250.h"

struct s_native_scenery
{
	byte unknown000[0x1a];
	short placement_index;
	byte unknown01c[0x12c - 0x1c];
	dword flags;
};
static_assert(__builtin_offsetof(s_native_scenery, placement_index) == 0x1a, "s_native_scenery::placement_index");
static_assert(__builtin_offsetof(s_native_scenery, flags) == 0x12c, "s_native_scenery::flags");

struct s_native_scenery_header
{
	byte unknown00[8];
	s_native_scenery *scenery;
};
static_assert(sizeof(s_native_scenery_header) == 0x0c, "s_native_scenery_header");

struct s_native_scenery_placement_entry
{
	byte unknown00[0x58];
	short name_index;
	byte unknown5a[2];
};
static_assert(sizeof(s_native_scenery_placement_entry) == 0x5c, "s_native_scenery_placement_entry");

struct s_native_scenery_scenario
{
	byte unknown00[0x54];
	s_native_scenery_placement_entry *entries;
};
static_assert(__builtin_offsetof(s_native_scenery_scenario, entries) == 0x54, "s_native_scenery_scenario::entries");

extern long *g_51e9cc;
class c_shape_global_owner;
extern c_shape_global_owner *g_51e9d0;
void function_1c5710(long object_index);

// @retail 0x10a250
void __stdcall function_10a250(long scenery_index)
{
	s_native_scenery *scenery =
		((s_native_scenery_header *)g_4e0300->data)[scenery_index & 0xffff].scenery;
	s_184251 state;
	state.field_0 = false;
	function_2e90a0(state);
	if (!(scenery->flags & 2))
	{
		if (scenery->placement_index != NONE)
		{
			long name_index = ((s_native_scenery_scenario *)g_4e0350)->entries[
				scenery->placement_index].name_index - 1;
			if (name_index != NONE)
			{
				if (g_51e9d0 != NULL)
					function_1c5710(scenery_index);
				g_51e9cc[name_index] = NONE;
			}
		}
		scenery->flags |= 2;
	}
}

#endif
