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
// score 19 (was 19): retail tests the end of the block with >= and gives the second block its own
// deallocate, ours shares the first and the second.
// @retail 0x147160
void c_game_havok_memory::deallocate(void *address)
{
	if (address)
	{
		if (((long)((byte *)g_479898 + 0x4340) <= (long)address && ((long)((byte *)g_479898 + 0xf0340) > (long)address || (byte *)g_479898 + 0xf0340 == address)))
		{
			_ReadWriteBarrier();
			g_479898->deallocate(address);
		}
		else if (((long)((byte *)g_479894 + 0x30) <= (long)address && ((long)((byte *)g_479894 + 0x15030) > (long)address || (byte *)g_479894 + 0x15030 == address)))
		{
			g_479894->deallocate(address);
		}
		else if (ADDRESS_IN_BLOCK(address, g_47989c->m_buffer, (byte *)g_47989c->m_buffer + g_47989c->m_size))
		{
			g_47989c->deallocate(address);
		}
		else
		{
			function_146e30()->deallocate(address);
		}
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

// score 36 (was 36): First upper-bound branch pair and EAX bound reuse still differ; shared bounds 36;
// outer bounds 36; shared bounds with delayed fixed receiver 43; numeric bounds 36;
// shared upper/lower 36/36; read-only bounds 36; shared bounds with exits 45.
// Fixed membership 95; paired memberships 146; independent pool/fixed/physical bounds 106/48/48;
// all independent bounds 134; separate pool-bound decisions 36; eager memberships 114.
// First upper-bound branch pair, EAX bound reuse and shared-dispatch placement still differ.
// first upper-bound branch pair is missing and bound
// registers differ. Typed pool bounds 36; typed bounds and pool dispatch 73;
// direct pool fields 36; typed bounds with delayed fixed receiver 43; fixed
// object size 36; physical object reference 36; pool object reference 36;
// typed fixed receiver 36.
// Shared inclusive begin/end helper 43; begin/size helper 44; helper with the fixed
// receiver 36; begin/size helper with early returns 44; size-first end 43. Retail's
// jge/jne pair and EAX ends come only from a second (equality) test of each end.
// @retail 0x147320
void c_game_havok_memory::deallocate16(void *address, long size, long memory_class)
{
	if (address)
	{
		c_havok_abstract_allocator *fixed = g_479894;
		if (ADDRESS_IN_BLOCK(address, (byte *)g_479898 + 0x4340, (byte *)g_479898 + 0xf0340))
		{
			g_479898->deallocate16(address, size, memory_class);
		}
		else if (ADDRESS_IN_BLOCK(address, (byte *)fixed + 0x30, (byte *)fixed + 0x15030))
		{
			fixed->deallocate16(address, size, memory_class);
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
