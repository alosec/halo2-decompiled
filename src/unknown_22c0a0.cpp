// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_22C0A0.CPP: the fixed buffer allocator Havok's memory uses
   (vtable 0x4576a4; unknown_146a20.h): allocations grow up from the buffer's
   start and their table (an offset and a size each) down from its end */

#include <string.h>
#include "unknown_11c920.h"
#include "unknown_146a20.h"

struct s_fixed_memory_entry
{
	long offset;
	long size;
};

/* places an allocation at the offset (rounded up to 16 bytes for an
   alignment above 1) when it ends within the limit, and fills its entry */
// @retail 0x22c330
void *c_havok_fixed_memory::function_22c330(long alignment, long limit,
	long size, long offset, s_fixed_memory_entry *entry)
{
	void *result = NULL;
	if (limit >= offset)
	{
		switch (alignment)
		{
		case 1:
			if (limit >= size + offset)
			{
				entry->offset = offset;
				entry->size = size;
				result = (byte *)m_buffer + offset;
			}
			break;
		case 2:
		case 4:
		case 8:
		case 16:
		{
			long aligned_offset = (offset + 15) & ~15;
			if (limit >= aligned_offset + size)
			{
				entry->offset = aligned_offset;
				entry->size = size;
				result = (byte *)m_buffer + aligned_offset;
			}
			break;
		}
		default:
			__assume(0);
		}
	}
	return result;
}

// @retail 0x22c0a0
void *c_havok_fixed_memory::allocate(long size, long memory_class)
{
	return allocate_aligned(1, size, memory_class);
}


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
		if ((byte *)m_buffer + ((s_fixed_memory_entry *)((byte *)m_buffer + m_size))[-1 - i].offset == address)
		{
			for (long j = i; j < m_count - 1; j++)
			{
				((s_fixed_memory_entry *)((byte *)m_buffer + m_size))[-1 - j] = ((s_fixed_memory_entry *)((byte *)m_buffer + m_size))[-2 - j];
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
	memory->m_unknown08[0] = 0;
	memory->m_unknown08[1] = 0;
	memory->m_unknown08[2] = 0;
	memory->m_unknown08[3] = NONE;
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
		memset(statistics, 0, sizeof(*statistics));
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
		memset(statistics, 0, sizeof(*statistics));
	}
}


// @retail 0x22c0d0
void *c_havok_fixed_memory::allocate_aligned(long alignment, long size, long memory_class)
{
    void *result = NULL;
    long used = 0;
    long offset = 0;
    s_fixed_memory_entry entry;
    if (m_count > 0)
    {
        s_fixed_memory_entry *last = (s_fixed_memory_entry *)((byte *)m_buffer + m_size - m_count * 8);
        used = last->offset + last->size;
    }
    if (used < m_size - (m_count * 8 + 8))
    {
        long i;
        for (i = 0; i < m_count; ++i)
        {
            s_fixed_memory_entry *current = (s_fixed_memory_entry *)((byte *)m_buffer + m_size - (i * 8 + 8));
            long limit = current->offset;
            if (limit >= size + offset)
            {
                result = function_22c330(alignment, limit, size, offset, &entry);
                if (result)
                {
                    m_count++;
                    for (long j = m_count - 1; j > i; --j)
                    {
                        s_fixed_memory_entry *items = (s_fixed_memory_entry *)((byte *)m_buffer + m_size);
                        items[-j - 1] = items[-j];
                    }
                    s_fixed_memory_entry *items = (s_fixed_memory_entry *)((byte *)m_buffer + m_size);
                    items[-i - 1] = entry;
                    break;
                }
            }
            offset = limit + current->size;
        }
        if (!result)
        {
            long limit = m_size - (m_count * 8 + 8);
            if (limit >= offset)
            {
                switch (alignment)
                {
                case 0: break;
                case 1: break;
                case 2: offset = (offset + 15) & ~15; break;
                case 4: offset = (offset + 15) & ~15; break;
                case 8: offset = (offset + 15) & ~15; break;
                case 16: offset = (offset + 15) & ~15; break;
                default: __assume(0);
                }
                if (limit >= size + offset) result = (byte *)m_buffer + offset;
            }
            if (result)
            {
                m_count++;
                s_fixed_memory_entry *items = (s_fixed_memory_entry *)((byte *)m_buffer + m_size);
                items[-m_count].offset = offset;
                items[-m_count].size = size;
            }
        }
    }
    return result;
}


// @retail 0x22c390
void function_22c390(c_havok_abstract_allocator *memory, s_fixed_memory_statistics_16 *statistics)
{
    c_havok_fixed_memory *storage = (c_havok_fixed_memory *)memory;
    statistics->header_size = 0x15040;
    statistics->allocation_count = storage->m_count;
    statistics->allocated_size = 0;
    for (long i = 0; i < storage->m_count; i++)
    {
        s_fixed_memory_entry *entries = (s_fixed_memory_entry *)((byte *)storage->m_buffer + storage->m_size);
        statistics->allocated_size += entries[-1 - i].size;
    }
    statistics->unknown0c = 0;
}

/* the size class of an allocation: 1 to 16, or NONE past 0x2000 */
static __forceinline long pool_size_class(long size)
{
    if (size <= 8)
        return 1;
    if (size <= 16)
        return 2;
    if (size <= 32)
        return 3;
    if (size <= 48)
        return 4;
    if (size <= 64)
        return 5;
    if (size <= 96)
        return 6;
    if (size <= 128)
        return 7;
    if (size <= 160)
        return 8;
    if (size <= 192)
        return 9;
    if (size <= 256)
        return 10;
    if (size <= 320)
        return 11;
    if (size <= 512)
        return 12;
    if (size <= 1024)
        return 13;
    if (size <= 2048)
        return 14;
    if (size <= 4096)
        return 15;
    if (size <= 8192)
        return 16;
    return NONE;
}

// @retail 0x22c3e0
c_havok_page_pool_allocator::c_havok_page_pool_allocator()
{
    unknown18 = 0;
    unknown1c = 0;
    unknown20 = 0;
    page_count = 0;
    memset(unknownf0344, 0, sizeof(unknownf0344));
    memset(unknownf0388, 0, sizeof(unknownf0388));
    for (long i = 16; i >= 0; i--)
        unknown24[i] = 0;
    for (long size = 0; size <= 0x200; size++)
    {
        long size_class = pool_size_class(size);
        small_classes[size] = (byte)size_class;
        class_sizes[size_class] = size;
    }
    for (long i = 0; i < 8; i++)
    {
        long size = (i + 1) * 0x400;
        long size_class = pool_size_class(size);
        large_classes[i] = size_class;
        class_sizes[size_class] = size;
    }
    unknown2d0[0] = 0;
    unknown2d0[1] = 0;
    unknown2d0[2] = 0;
    unknown2d0[3] = 0;
    page_size = 0x2000;
    unknown2e4 = 0x40;
    function_2d6fc0((void *)(((dword)pages + 0xf) & ~0xf), 0x4000);
}


struct s_physics_pool_statistics
{
    long allocated, pages, used_pages, page_size, free_size;
    real used_fraction;
    struct s_entry { long size, stride, count, used; } entries[16];
    long metadata_size, entry_count;
};

struct s_22cb00_memory
{
    byte unknown00[0x1c];
    long field_1c;
    long field_20;
    byte unknown24[0x6c - 0x24];
    long field_6c[16];
    byte unknownac[0xf0340 - 0xac];
    long field_f0340;
    byte unknownf0344[4];
    long field_f0348[16];
    byte unknownf0388[4];
    long field_f038c[16];
};

// @retail 0x22cb00
void function_22cb00(c_havok_abstract_allocator *memory, s_physics_pool_statistics *statistics)
{
    s_22cb00_memory *storage = (s_22cb00_memory *)memory;
    statistics->allocated = 0xf03d0;
    statistics->pages = 0x76;
    statistics->used_pages = storage->field_f0340;
    statistics->page_size = 0x2000;
    statistics->free_size = storage->field_1c - storage->field_20 + 1;
    long used = 0;
    for (long i = 15; i >= 0; --i)
    {
        used += (storage->field_f0348[i] - storage->field_f038c[i]) * storage->field_6c[i];
    }
    statistics->used_fraction = (real)used * (1.0f / 966656.0f);
    statistics->entry_count = 16;
    statistics->metadata_size = 0;
    memset(statistics->entries, 0, sizeof(statistics->entries));
}

/* the fraction of the pool's blocks in use */
// @retail 0x22cb90
real c_havok_page_pool_allocator::get_used_fraction(void)
{
    s_22cb00_memory *storage = (s_22cb00_memory *)this;
    long used = 0;
    for (long i = 15; i >= 0; --i)
    {
        used += (storage->field_f0348[i] - storage->field_f038c[i]) * storage->field_6c[i];
    }
    return (real)used * (1.0f / 966656.0f);
}
