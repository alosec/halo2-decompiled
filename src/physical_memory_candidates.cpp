// @flags /O2 /Gr
/* PHYSICAL_MEMORY_CANDIDATES.CPP: the physical memory allocator's
   (physical_memory.h) choice between two blocks it could free */

#include "unknown_11c920.h"
#include "physical_memory.h"

/* the part of a candidate block (0x13d370's s_block_candidate) read here: the
   allocator's clock when it was last used, and its pages */
struct s_physical_block_view
{
	byte unknown00[4];
	dword time;
	byte unknown08[4];
	long pages;
};

/* whether block a should go before block b: the one used longer ago (by more
   than 150 ticks, ages counting up to 300), else the smaller */
#pragma auto_inline(off)
// @retail 0x13d320
bool physical_memory_candidate_is_better(s_physical_object const *physical, s_physical_block_view const *a, s_physical_block_view const *b)
{
	bool result = false;
	dword age_a = physical->time - a->time;
	if (age_a > 300)
	{
		age_a = 300;
	}
	dword age_b = physical->time - b->time;
	if (age_b > 300)
	{
		age_b = 300;
	}

	if ((long)(age_a + 150) < (long)age_b)
	{
		return false;
	}
	if ((long)age_a > (long)(age_b + 150) || a->pages < b->pages)
	{
		result = true;
	}
	return result;
}
#pragma auto_inline(on)
