// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1470F0.CPP: the game's c_havok_abstract_allocator (vtable 0x45378c), which Havok
   allocates through: it takes memory from the fixed buffer while only that
   exists (g_479890 is 1), otherwise from the pool, then the ai's scratch
   buffers, then the buffer in physical memory, and frees an address to the
   allocator whose memory holds it (see unknown_146a20.cpp) */

#include "unknown_11c920.h"
#include "unknown_146a20.h"

extern long g_479890;
extern c_havok_abstract_allocator *g_479894;
extern c_havok_abstract_allocator *g_479898;
extern c_havok_fixed_memory *g_47989c;

c_havok_abstract_allocator *function_146e30(void);

class c_game_havok_memory : public c_havok_abstract_allocator
{
public:
	virtual void *allocate(long size, long memory_class);
	virtual void deallocate(void *address);
	virtual void *allocate_aligned(long alignment, long size, long memory_class);
	virtual void deallocate_aligned(void *address);
	virtual void *allocate16(long size, long memory_class);
	virtual void deallocate16(void *address, long size, long memory_class);
	virtual void method6(long unknown) {}
	virtual void method7(long unknown) {}
};

/* whether the address lies in the block */
#define ADDRESS_IN_BLOCK(address, start, end) ((long)(start) <= (long)(address) && (long)(end) >= (long)(address))

// @retail 0x1470f0 deleting c_game_havok_memory

// @retail 0x147110
void *c_game_havok_memory::allocate(long size, long memory_class)
{
	void *result;

	switch (g_479890)
	{
	case 1:
		return g_479894->allocate(size, memory_class);
	}
	result = g_479898->allocate(size, memory_class);
	if (!result)
	{
		result = g_47989c->allocate(size, memory_class);
		if (!result)
		{
			result = function_146e30()->allocate(size, memory_class);
		}
	}
	return result;
}

extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)
// score 9 (was 9): first and third now share source dispatch, but retail's second uses edx for
// retail tests the end of the block with >= and gives the second block its own
// deallocate, ours shares the first and the second.
// Y346 (not kept: a redundant '>= || ==' end test and a hand-merged dispatch for the first and third ranges): 9.
// Inverse admission with separate range lifetimes 41/49/52/49/41/49/52, shared end 52; native range lifetimes
// 31/9/31/9/31/9/31, shared end 31; each receiver global read once on its path 9/9/9.
// first and third now share source dispatch, but retail's second uses edx for
// its vtable and eax for bounds; ours uses eax for the vtable, so the third joins the second.
// Third end addition and fallback argument order also differ.
// Round 97: plain four-call chain 38; third in the inclusive end form of the first two 7 (not kept,
// two points); four calls with inclusive first/third 29, all inclusive 29; shared dispatch with
// inclusive second/third only 31.
// Tried: third abstract/fixed-base dispatch 19/19; nested receivers 19; first/second/both inclusive
// 18/28/27; shared receiver name 19 and with inclusive 27; allocation local 19; fallback
// inline/noinline/consistent-inline declarations 19/19/19; inverted third 19; third predicate 19;
// first/third shared dispatch 11 with concrete/same-local second 11/11 and inclusive 9;
// early third 39 (132/121); late second 9; second predicate 9; dispatch flag 38 (109/121).
// Round 221: void completion second/fallback/both 26/26/26; common 9;
// second/common and all completions 9/9; fallback/common 9; entry null return with
// all completions 26. Original dispatch graph restored.
// Inclusive third with second receiver lifetime 7; eager second/first ends 9/32;
// address-first second lower 8; scoped first/second range results 7/7; fallback result local 7;
// entry/reversed receiver declarations 7/7; eager first receiver 7; named second/first upper
// predicates 7/7; standalone third range 7.
// @retail 0x147160
void c_game_havok_memory::deallocate(void *address)
{
	if (address)
	{
		c_havok_abstract_allocator *allocator;
		if (((long)((byte *)g_479898 + 0x4340) <= (long)address && ((long)((byte *)g_479898 + 0xf0340) >= (long)address || (byte *)g_479898 + 0xf0340 == address)))
		{
			_ReadWriteBarrier();
			allocator = g_479898;
		}
		else if (((long)((byte *)g_479894 + 0x30) <= (long)address && ((long)((byte *)g_479894 + 0x15030) >= (long)address || (byte *)g_479894 + 0x15030 == address)))
		{
			allocator = g_479894;
			allocator->deallocate(address);
			return;
		}
		else if (ADDRESS_IN_BLOCK(address, g_47989c->m_buffer, (byte *)g_47989c->m_buffer + g_47989c->m_size))
		{
			allocator = g_47989c;
		}
		else
		{
			function_146e30()->deallocate(address);
			return;
		}
		allocator->deallocate(address);
	}
}
#pragma function(_ReadWriteBarrier)

// @retail 0x1471e0
void *c_game_havok_memory::allocate_aligned(long alignment, long size, long memory_class)
{
	void *result;

	switch (g_479890)
	{
	case 1:
		return g_479894->allocate_aligned(alignment, size, memory_class);
	}
	result = g_479898->allocate_aligned(alignment, size, memory_class);
	if (!result)
	{
		result = g_47989c->allocate_aligned(alignment, size, memory_class);
		if (!result)
		{
			result = function_146e30()->allocate_aligned(alignment, size, memory_class);
		}
	}
	return result;
}

#pragma intrinsic(_ReadWriteBarrier)
// score 19 (was 19): retail tests the end of the block with >= and gives the second block its own
// deallocate, ours shares the first and the second.
// Second return 19; first+second returns 19; address local 19; second concrete 19; second concrete
// with third base 19; primary/second branch inversions 19/19; third-before-second 19; fallback local 19;
// second predicate 19. First start/end 41; with second return/concrete/end/receiver 41/41/42/41;
// inclusive tests and inclusive returns 42/42; first start/end without the barrier 41.
// @retail 0x147240
void c_game_havok_memory::deallocate_aligned(void *address)
{
	if (address)
	{
		if (((long)((byte *)g_479898 + 0x4340) <= (long)address && ((long)((byte *)g_479898 + 0xf0340) > (long)address || (byte *)g_479898 + 0xf0340 == address)))
		{
			_ReadWriteBarrier();
			g_479898->deallocate_aligned(address);
		}
		else if (((long)((byte *)g_479894 + 0x30) <= (long)address && ((long)((byte *)g_479894 + 0x15030) > (long)address || (byte *)g_479894 + 0x15030 == address)))
		{
			g_479894->deallocate_aligned(address);
		}
		else if (ADDRESS_IN_BLOCK(address, g_47989c->m_buffer, (byte *)g_47989c->m_buffer + g_47989c->m_size))
		{
			g_47989c->deallocate_aligned(address);
		}
		else
		{
			function_146e30()->deallocate_aligned(address);
		}
	}
}
#pragma function(_ReadWriteBarrier)

// @retail 0x1472c0
void *c_game_havok_memory::allocate16(long size, long memory_class)
{
	void *result;

	switch (g_479890)
	{
	case 1:
		return g_479894->allocate16(size, memory_class);
	}
	result = g_479898->allocate16(size, memory_class);
	if (!result)
	{
		result = g_47989c->allocate16(size, memory_class);
		if (!result)
		{
			result = function_146e30()->allocate16(size, memory_class);
		}
	}
	return result;
}

// score 8 (was 36): the first range's end is now a named local tested for equality, which gives
// retail's jge/jne pair and the shared dispatch block; the first range's start and the second and
// third ranges' ends are still computed in edx where retail uses eax.
// @retail 0x147320
void c_game_havok_memory::deallocate16(void *address, long size, long memory_class)
{
	if (address)
	{
		byte *pool_end = (byte *)g_479898 + 0xf0340;
		if (((long)((byte *)g_479898 + 0x4340) <= (long)address && ((long)pool_end >= (long)address || pool_end == address)))
		{
			g_479898->deallocate16(address, size, memory_class);
		}
		else if (ADDRESS_IN_BLOCK(address, (byte *)g_479894 + 0x30, (byte *)g_479894 + 0x15030))
		{
			g_479894->deallocate16(address, size, memory_class);
		}
		else if (ADDRESS_IN_BLOCK(address, g_47989c->m_buffer, (byte *)g_47989c->m_buffer + g_47989c->m_size))
		{
			g_47989c->deallocate16(address, size, memory_class);
		}
		else
		{
			function_146e30()->deallocate16(address, size, memory_class);
		}
	}
}

/* the game's c_havok_abstract_allocator, which Havok's code asks for */
c_game_havok_memory g_4798b0;

// @retail 0x1473b0
c_havok_abstract_allocator *function_1473b0(void)
{
	return &g_4798b0;
}
