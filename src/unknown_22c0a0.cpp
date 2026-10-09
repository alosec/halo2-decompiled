// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_22C0A0.CPP: the fixed buffer allocator Havok's memory uses
   (vtable 0x4576a4; unknown_146a20.h): allocations grow up from the buffer's
   start and their table (an offset and a size each) down from its end */

#include "unknown_11c920.h"
#include "unknown_146a20.h"

struct s_fixed_memory_entry
{
	long offset;
	long size;
};

// @retail 0x22c0a0
void *c_havok_fixed_memory::allocate(long size, long memory_class)
{
	return allocate_aligned(1, size, memory_class);
}

#ifdef H2_NATIVE_PORT
// Portable scalar form of the recovered gap-search allocator at retail 0x22c0d0.
void *c_havok_fixed_memory::allocate_aligned(long alignment, long size, long memory_class)
{
	(void)memory_class;
	long cursor = 0;
	s_fixed_memory_entry *entries = (s_fixed_memory_entry *)((byte *)m_buffer + m_size);
	if (m_count > 0)
	{
		s_fixed_memory_entry const &last = entries[-m_count];
		cursor = last.offset + last.size;
	}

	for (long i = 0; i <= m_count; ++i)
	{
		long limit = i < m_count ? entries[-1 - i].offset : m_size - (m_count + 1) * (long)sizeof(s_fixed_memory_entry);
		long offset = alignment == 16 ? (cursor + 15) & ~15 : cursor;
		if (offset <= limit && size <= limit - offset)
		{
			for (long j = m_count; j > i; --j)
				entries[-1 - j] = entries[-j];
			entries[-1 - i].offset = offset;
			entries[-1 - i].size = size;
			m_count++;
			return (byte *)m_buffer + offset;
		}
		if (i < m_count)
			cursor = entries[-1 - i].offset + entries[-1 - i].size;
	}
	return NULL;
}
#endif

// @retail 0x22c0c0
void c_havok_fixed_memory::deallocate(void *address)
{
	deallocate_aligned(address);
}

/* drops the allocation's entry from the table */
// @retail 0x22c280
void c_havok_fixed_memory::deallocate_aligned(void *address)
{
	for (long i = 0; i < m_count; i++)
	{
		s_fixed_memory_entry *entries = (s_fixed_memory_entry *)((byte *)m_buffer + m_size);

		if ((byte *)m_buffer + entries[-1 - i].offset == address)
		{
			for (; i < m_count - 1; i++)
			{
				entries = (s_fixed_memory_entry *)((byte *)m_buffer + m_size);
				entries[-1 - i] = entries[-2 - i];
			}
			m_count--;
			return;
		}
	}
}

// @retail 0x22c300
void *c_havok_fixed_memory::allocate16(long size, long memory_class)
{
	return allocate_aligned(16, size, memory_class);
}

// @retail 0x22c320
void c_havok_fixed_memory::deallocate16(void *address, long size, long memory_class)
{
	deallocate_aligned(address);
}

#ifdef H2_NATIVE_PORT
static_assert(sizeof(c_havok_fixed_memory) == 0x30, "fixed Havok allocator layout");

static void *h2_native_fixed_allocate(c_havok_fixed_memory *memory, long size, long memory_class)
{
	return memory->c_havok_fixed_memory::allocate(size, memory_class);
}

static void h2_native_fixed_deallocate(c_havok_fixed_memory *memory, void *address)
{
	memory->c_havok_fixed_memory::deallocate(address);
}

static void *h2_native_fixed_allocate_aligned(c_havok_fixed_memory *memory, long alignment,
	long size, long memory_class)
{
	return memory->c_havok_fixed_memory::allocate_aligned(alignment, size, memory_class);
}

static void h2_native_fixed_deallocate_aligned(c_havok_fixed_memory *memory, void *address)
{
	memory->c_havok_fixed_memory::deallocate_aligned(address);
}

static void *h2_native_fixed_allocate16(c_havok_fixed_memory *memory, long size, long memory_class)
{
	return memory->c_havok_fixed_memory::allocate16(size, memory_class);
}

static void h2_native_fixed_deallocate16(c_havok_fixed_memory *memory, void *address,
	long size, long memory_class)
{
	memory->c_havok_fixed_memory::deallocate16(address, size, memory_class);
}

static void h2_native_fixed_idle(c_havok_fixed_memory *, long)
{
}

static c_havok_fixed_memory *h2_native_fixed_destroy(c_havok_fixed_memory *memory, dword)
{
	return memory;
}

typedef void (*h2_native_fixed_method)();
static h2_native_fixed_method const g_h2_native_fixed_vtable[] =
{
	(h2_native_fixed_method)h2_native_fixed_allocate,
	(h2_native_fixed_method)h2_native_fixed_deallocate,
	(h2_native_fixed_method)h2_native_fixed_allocate_aligned,
	(h2_native_fixed_method)h2_native_fixed_deallocate_aligned,
	(h2_native_fixed_method)h2_native_fixed_allocate16,
	(h2_native_fixed_method)h2_native_fixed_deallocate16,
	(h2_native_fixed_method)h2_native_fixed_idle,
	(h2_native_fixed_method)h2_native_fixed_idle,
	(h2_native_fixed_method)h2_native_fixed_destroy,
	(h2_native_fixed_method)h2_native_fixed_idle,
	(h2_native_fixed_method)h2_native_fixed_idle,
};

extern "C" c_havok_fixed_memory *h2_native_havok_fixed_memory_construct(
	void *storage, void *buffer, long size)
{
	c_havok_fixed_memory *memory = (c_havok_fixed_memory *)storage;
	*(h2_native_fixed_method const **)memory = g_h2_native_fixed_vtable;
	memory->m_unknown04 = 1;
	memory->m_unknown08 = 0;
	memory->m_unknown0c = 0;
	memory->m_unknown10 = 0;
	memory->m_unknown14 = NONE;
	memory->m_buffer = buffer;
	memory->m_size = size & 0x0ffffffc;
	memory->m_count = 0;
	return memory;
}
#endif

/* what the fixed buffers report: the header's size, the bytes allocated and
   the allocations */
struct s_fixed_memory_statistics
{
	long header_size;
	long allocated_size;
	long unknown08;
	long allocation_count;
	long unknown10;
};

struct s_fixed_memory_statistics_16
{
	long header_size;
	long allocated_size;
	long allocation_count;
	long unknown0c;
};

// @retail 0x22cbd0
void fixed_memory_get_statistics(c_havok_fixed_memory *memory, s_fixed_memory_statistics *statistics)
{
	if (memory)
	{
		statistics->header_size = 0x30;
		statistics->allocation_count = memory->m_count;
		statistics->allocated_size = 0;
		for (long i = 0; i < memory->m_count; i++)
		{
			s_fixed_memory_entry *entries = (s_fixed_memory_entry *)((byte *)memory->m_buffer + memory->m_size);

			statistics->allocated_size += entries[-1 - i].size;
		}
		statistics->unknown10 = 0;
	}
	else
	{
		statistics->header_size = 0;
		statistics->allocated_size = 0;
		statistics->unknown08 = 0;
		statistics->allocation_count = 0;
		statistics->unknown10 = 0;
	}
}

// @retail 0x22cc30
void fixed_memory_get_statistics_16(c_havok_fixed_memory *memory, s_fixed_memory_statistics_16 *statistics)
{
	if (memory)
	{
		statistics->header_size = 0x30;
		statistics->allocation_count = memory->m_count;
		statistics->allocated_size = 0;
		for (long i = 0; i < memory->m_count; i++)
		{
			s_fixed_memory_entry *entries = (s_fixed_memory_entry *)((byte *)memory->m_buffer + memory->m_size);

			statistics->allocated_size += entries[-1 - i].size;
		}
		statistics->allocation_count = memory->m_count;
		statistics->unknown0c = 0;
	}
	else
	{
		statistics->header_size = 0;
		statistics->allocated_size = 0;
		statistics->allocation_count = 0;
		statistics->unknown0c = 0;
	}
}
