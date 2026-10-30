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
	union
	{
		struct
		{
			dword : 16;
			dword shadowless : 1;
			dword deleted_when_deactivated : 1;
			dword : 14;
		} object;
		dword object_flag_bits;
	};
	byte unknown008[0x16c - 8];
	long expiration_tick;
};

struct s_garbage_header_view
{
	byte unknown00[8];
	s_garbage_object_view *object;
};

void function_bb950(long object_index, bool add, long delta);

// score 2 (was 2): only the expiration addition/store uses the ticks register
// instead of the time register. Earlier: named `seconds` 14; `ticks + current_time` 14;
// second flag before expiration 14; `real_to_long` plain/forceinline 45/14;
// game time read in the expression 27. Union alone 16; final/both direct masks 16/16;
// shared flag-word pointer 4; pointer and named seconds 2; expiration accumulator 2;
// flag pointer before the call 2; direct late game time 27; settings pointer 25;
// ticks declared first 2; named expiration result 2. Round 14 (lane 10): `ticks + current_time`
// 2; the existing real_to_long with a forceinline declaration 2; game time through the existing
// 0x146650 2; the sample call folded into the seconds product 2; rate-first product 2.
// Round 22: early time declaration 2; early/late expiration pointers 2/2; early expiration
// accumulator 16; result-first initialized/uninitialized 2/2; ticks before seconds 2; early
// duration declaration 2.
// Further trials: local names ticks to tick_delta 2; local names current_time to
// start_tick 2; local names ticks to tick_delta, current_time to start_tick 2; local names
// ticks to duration_ticks, current_time to base_tick 2; local names duration to
// tick_duration 2; native int conversion result 2; unsigned time carrier 2; staged seconds
// product 2.
// Further forms: time-first/ticks-first destination accumulation 2/2; time read after
// seconds/duration 2/16; immutable sample products 2; conversion-only duration scope 2;
// immutable object pointer 2; immutable flag pointer and time 2. Original body restored.
// Round 39: the file-private seconds-to-ticks inline (rate * seconds, fld/fistp) as
// `current_time + ticks(seconds)` / `ticks(seconds) + current_time` 2/2, with the game time
// read in the expression 27; an inline returning `game_time + ticks` (0x20f1a0's form,
// which gives retail's `add time, ticks` there) with the time read first 2, the product
// written seconds-first 2, `__forceinline` 2, the seconds product as its argument 2, and the
// time read after the product 17 (that gives retail's add direction, but the time load moves
// after the random conversion).
// @retail 0x11bb40
bool __stdcall function_11bb40(long object_index, long creation_argument1, long creation_argument2)
{
	s_garbage_object_view *garbage =
		((s_garbage_header_view *)g_4e0300->data)[object_index & 0xffff].object;
	function_bb950(object_index, true, 0);
	dword *flags = &garbage->object_flag_bits;
	*flags |= 0x20000;
	real sample = function_x82e52f(&g_4e7408->unknown0, NULL, 0);
	long current_time = g_510c54->game_time;
	real seconds = (sample + 1.0f) * 10.0f;
	real duration = seconds * g_510c54->field_2_3;
	long ticks;
	__asm
	{
		fld duration
		fistp ticks
	}
	garbage->expiration_tick = current_time + ticks;
	*flags |= 0x10000;
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
