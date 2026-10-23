// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_0B7430.CPP: moving an object: its position and orientation are
   set (a position outside the world bounds is refused for an unparented
   object), its physics and partition entries follow. */

#include "unknown_11c920.h"
#include "globals.h"
#include "object_queries.h"

struct s_object_move_view
{
	long definition_index;
	dword flags;
	byte unknown08[0x14 - 8];
	long parent_index;
	byte unknown18[0x64 - 0x18];
	point3f position;
	vector3f forward;
	vector3f up;
	byte unknown88[0xd4 - 0x88];
	long havok_component_index;
};

struct s_object_move_header
{
	byte unknown00[8];
	s_object_move_view *object;
};

#define OBJECT_MOVE_GET(index) (((s_object_move_header *)g_4e0300->data)[(index) & 0xffff].object)

void function_b58c0(long index, dword mask);
void __stdcall function_b87b0(long object_index);
void __stdcall function_b8600(long object_index, long location);
void __stdcall function_bd020(long object_index);
void function_1c4a80(long object_index, bool position_unchanged, bool force);

// Disabled: matches byte for byte, but closes the call cycle b75a0 -> b7430 -> b8600 -> b7300 -> 1c38a0 -> e18d0 -> b75a0, which gives matched b75a0 its default convention and costs it and ten callers; with 0xb7b40 and 0x2902b0 also enabled it costs nothing of its own (their 3: d6bc0, 11a320, 1ca290) (build/codex-report.md, rounds 26 to 28).
#if 0
// @retail 0xb7430 disabled
bool __stdcall function_b7430(long object_index, point3f const *position, vector3f const *forward,
	vector3f const *up, s_location const *location, bool update, bool physics, bool force, bool keep_partition)
{
	s_object_move_view *object = OBJECT_MOVE_GET(object_index);
	bool placed = (object->flags & 0x100) != 0;
	bool result = true;
	if (placed && !keep_partition)
		function_b87b0(object_index);
	if (position)
	{
		if (object->parent_index == NONE &&
			!(position->x >= -32768.0f && 32768.0f >= position->x && position->y >= -32768.0f &&
				32768.0f >= position->y && position->z >= -32768.0f && 32768.0f >= position->z))
			result = false;
		else
		{
			object->position = *position;
			long component = OBJECT_MOVE_GET(object_index)->havok_component_index;
			if (component != NONE)
				function_b58c0(component, 2);
		}
	}
	if (forward)
	{
		object->forward = *forward;
		object->up = *up;
		object = OBJECT_MOVE_GET(object_index);
		if (object->havok_component_index != NONE)
			function_b58c0(object->havok_component_index, 4);
	}
	if (physics)
	{
		bool no_position = position == 0;
		function_1c4a80(object_index, no_position, force);
	}
	if (update)
		function_bd020(object_index);
	if (placed && !keep_partition)
		function_b8600(object_index, (long)location);
	return result;
}
#endif
