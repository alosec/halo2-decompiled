// @flags /O2 /Gr
/* GARBAGE.CPP: creation of temporary debris objects.
   See docs/garbage.md for the original-object mapping. */

#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"
#include "unknown_0259d0.h"

struct s_garbage_object_view
{
	long definition_index;
	struct
	{
		dword : 16;
		dword shadowless : 1;
		dword deleted_when_deactivated : 1;
		dword : 14;
	} object;
	byte unknown008[0x16c - 8];
	long expiration_tick;
};

struct s_garbage_header_view
{
	byte unknown00[8];
	s_garbage_object_view *object;
};

void function_bb950(long object_index, bool add, long delta);

// score 16 (was 16): retail stores the expiration first and then sets the
// second flag with a memory `or`, and adds the ticks into the time register; ours
// loads the flags early (4 bytes longer). Tried: a named `seconds` product 14
// (fixes the multiply order); then `ticks + current_time` 14, the second flag set
// before the expiration store 14, `real_to_long` without forceinline 45, with a
// forceinline declaration 14, game time read in the expression 27.
// @retail 0x11bb40
bool __stdcall function_11bb40(long object_index, long creation_argument1, long creation_argument2)
{
	s_garbage_object_view *garbage =
		((s_garbage_header_view *)g_4e0300->data)[object_index & 0xffff].object;
	function_bb950(object_index, true, 0);
	garbage->object.deleted_when_deactivated = true;
	real sample = function_x82e52f(&g_4e7408->unknown0, NULL, 0);
	long current_time = g_510c54->game_time;
	real duration = (sample + 1.0f) * 10.0f * g_510c54->field_2_3;
	long ticks;
	__asm
	{
		fld duration
		fistp ticks
	}
	garbage->expiration_tick = current_time + ticks;
	garbage->object.shadowless = true;
	return true;
}

/* Actual garbage type-definition prefix at 0x467e60, through creation.
   Later fields and parent types are outside this view. Retail's creation
   ABI has three stack arguments; the unused arguments remain provisional. */
struct s_garbage_type_definition_view
{
	char const *name;
	dword group_tag;
	short datum_size;
	short unknown0a;
	short unknown0c;
	short unknown0e;
	void *unknown10[7];
	bool (__stdcall *create)(long, long, long);
};

s_garbage_type_definition_view g_467e60 =
{
	"garbage", 'garb', 0x170, NONE, NONE, NONE,
	{ NULL, NULL, NULL, NULL, NULL, NULL, NULL },
	function_11bb40
};
