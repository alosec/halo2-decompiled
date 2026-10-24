// @flags /O2
// Odds and ends of the other libraries linked in 0x36d1d7-0x372030:
//
//   0x36d1d7  a jump to the Xbox crypto library's XcspFreeEverything
//             (0x36c538)
//   RAD's (Bink's) memory helpers, next to radmalloc (0x36d3b0) and radfree
//   (0x36d420):
//     0x36d390  RADSetMemory: the allocation and free callbacks radmalloc
//               and radfree use
//     0x36d220  queues a block of a batch: where its pointer goes and its
//               size, rounded to 32 bytes and padded so successive blocks
//               start in different 32-byte cache sets
//     0x36d300  the batch's total size so far
//     0x36d310  allocates the batch in one block (plus `extra` bytes in
//               front), handing each queued pointer its place
//   0x372030  whether to use the MMX paths: retail asks cpuid once (bit 23
//             of the features) and caches it; flag 0x40000 turns them off
//
// Nothing calls these by address (Bink's callers are not in this image's
// reach); they are recovered with the rest of the range. The batch's two
// tables start in static storage of 32 entries each (the table pointers at
// 0x466d60 and 0x466d64), are doubled from radmalloc as they fill, and the
// static ones are never freed.
#include <string.h>

// XcspFreeEverything (not recovered)
extern "C" void h2_retail_36c538(void);
// radmalloc and radfree (not recovered)
extern "C" void *h2_retail_36d3b0(unsigned long size);
extern "C" void h2_retail_36d420(void *block);

// @retail 0x36d1d7
extern "C" void __stdcall library_36d1d7(void)
{
	h2_retail_36c538();
}

typedef void *(__stdcall *rad_malloc_36d390)(unsigned long size);
typedef void(__stdcall *rad_free_36d390)(void *block);

extern rad_malloc_36d390 g_484af4;
extern rad_free_36d390 g_484af8;

// @retail 0x36d390
extern "C" void __stdcall library_36d390(rad_malloc_36d390 allocate, rad_free_36d390 release)
{
	g_484af4 = allocate;
	g_484af8 = release;
}

// the queued blocks' pointers' places and sizes, the tables' capacity, the
// count queued, and their total size
extern void ***g_466d60;
extern unsigned long *g_466d64;
extern long g_466d68;
extern unsigned long g_484ad4;
extern unsigned long g_484ad0;

// @retail 0x36d220
extern "C" void __stdcall library_36d220(void **place, unsigned long size)
{
	unsigned long count = g_484ad4;
	if ((unsigned long)g_466d68 == count)
	{
		long const capacity = g_466d68;
		char *const tables = (char *)h2_retail_36d3b0(capacity << 4);
		memcpy(tables, g_466d60, capacity * 4);
		unsigned long *const sizes = (unsigned long *)(tables + capacity * 8);
		memcpy(sizes, g_466d64, capacity * 4);
		g_466d68 = capacity * 2;
		if (capacity * 2 != 0x40)
		{
			h2_retail_36d420(g_466d60);
		}
		count = g_484ad4;
		g_466d64 = sizes;
		g_466d60 = (void ***)tables;
	}
	unsigned long rounded = (size + 0x1f) & ~0x1fu;
	unsigned long const total = g_484ad0;
	rounded += (((total >> 5) - (rounded >> 5) + 1) & 0x1f) << 5;
	g_484ad0 = total + rounded;
	g_466d64[count] = rounded;
	g_466d60[count] = place;
	g_484ad4 = count + 1;
}

// @retail 0x36d300
extern "C" unsigned long __stdcall library_36d300(void)
{
	return g_484ad0;
}

// @retail 0x36d310
extern "C" void *__stdcall library_36d310(unsigned long extra)
{
	unsigned long const front = (extra + 0x1f) & ~0x1fu;
	char *const block = (char *)h2_retail_36d3b0(g_484ad0 + front);
	g_484ad0 = 0;
	if (block)
	{
		char *next = block + front;
		for (unsigned long i = 0; i < g_484ad4; i++)
		{
			*g_466d60[i] = next;
			next += g_466d64[i];
		}
	}
	g_484ad4 = 0;
	return block;
}

// whether the CPU has MMX: -1 until asked
extern long g_466d6c;
// use the MMX paths
extern long g_5637dc;

// @retail 0x372030
extern "C" void __cdecl library_372030(unsigned long flags)
{
	long has_mmx = g_466d6c;
	if (has_mmx < 0)
	{
		// retail: cpuid, when the flags' ID bit can be toggled, and bit 23
		// of its features; the Xbox's Pentium III has MMX
		has_mmx = 1;
		g_466d6c = has_mmx;
	}
	if (has_mmx != 0 && (flags & 0x40000) == 0)
	{
		g_5637dc = 1;
	}
	else
	{
		g_5637dc = 0;
	}
}
