#include "unknown_11c920.h"
#include "unknown_146a20.h"
#include <string.h>

#ifdef H2_NATIVE_PORT
static_assert(sizeof(hkPoolMemory) == 0xf03d0, "Havok pool allocator layout");

namespace
{
constexpr dword k_pool_magic = 0x4832504d;
constexpr long k_pool_begin = 0x4340;
constexpr long k_pool_end = 0xf0340;
constexpr dword k_block_free = 1;

struct s_native_pool_state
{
    dword magic;
    long bump;
    long allocation_count;
    long used_bytes;
};

struct s_native_pool_block
{
    long span;
    long requested;
    long payload_offset;
    dword flags;
};
static_assert(sizeof(s_native_pool_block) == 0x10, "native pool block layout");

static s_native_pool_state *pool_state(hkPoolMemory *memory)
{
    return (s_native_pool_state *)((byte *)memory + 0x18);
}

static bool valid_alignment(long alignment)
{
    return alignment > 0 && alignment <= 0x1000 && !(alignment & (alignment - 1));
}

static long align_up(long value, long alignment)
{
    return (value + alignment - 1) & -alignment;
}

static void *pool_allocate(hkPoolMemory *memory, long alignment, long size)
{
    if (!memory || size < 0 || !valid_alignment(alignment))
        return NULL;
    s_native_pool_state *state = pool_state(memory);
    if (state->magic != k_pool_magic)
        return NULL;

    for (long offset = k_pool_begin; offset < state->bump;)
    {
        s_native_pool_block *block = (s_native_pool_block *)((byte *)memory + offset);
        if (block->span < (long)sizeof(*block) || offset > state->bump - block->span)
            return NULL;
        long payload = offset + block->payload_offset;
        if ((block->flags & k_block_free) && !(payload & (alignment - 1)) &&
            size <= block->span - block->payload_offset)
        {
            block->requested = size;
            block->flags &= ~k_block_free;
            state->allocation_count++;
            state->used_bytes += size;
            return (byte *)memory + payload;
        }
        offset += block->span;
    }

    long start = state->bump;
    long payload = align_up(start + (long)sizeof(s_native_pool_block), alignment);
    long end = align_up(payload + size, 16);
    if (payload < start || end < payload || end > k_pool_end)
        return NULL;
    s_native_pool_block *block = (s_native_pool_block *)((byte *)memory + start);
    block->span = end - start;
    block->requested = size;
    block->payload_offset = payload - start;
    block->flags = 0;
    state->bump = end;
    state->allocation_count++;
    state->used_bytes += size;
    return (byte *)memory + payload;
}

static void pool_deallocate(hkPoolMemory *memory, void *address)
{
    if (!memory || !address)
        return;
    s_native_pool_state *state = pool_state(memory);
    if (state->magic != k_pool_magic)
        return;
    for (long offset = k_pool_begin; offset < state->bump;)
    {
        s_native_pool_block *block = (s_native_pool_block *)((byte *)memory + offset);
        if (block->span < (long)sizeof(*block) || offset > state->bump - block->span)
            return;
        if ((byte *)memory + offset + block->payload_offset == address)
        {
            if (!(block->flags & k_block_free))
            {
                block->flags |= k_block_free;
                state->allocation_count--;
                state->used_bytes -= block->requested;
            }
            return;
        }
        offset += block->span;
    }
}
}

extern "C" void *h2_native_pool_allocate(hkPoolMemory *memory, long size, long)
{
    return pool_allocate(memory, 1, size);
}

extern "C" void h2_native_pool_deallocate(hkPoolMemory *memory, void *address)
{
    pool_deallocate(memory, address);
}

extern "C" void *h2_native_pool_allocate_aligned(hkPoolMemory *memory, long alignment,
    long size, long)
{
    return pool_allocate(memory, alignment, size);
}

extern "C" void h2_native_pool_deallocate_aligned(hkPoolMemory *memory, void *address)
{
    pool_deallocate(memory, address);
}

extern "C" void *h2_native_pool_allocate16(hkPoolMemory *memory, long size, long)
{
    return pool_allocate(memory, 16, size);
}

extern "C" void h2_native_pool_deallocate16(hkPoolMemory *memory, void *address, long, long)
{
    pool_deallocate(memory, address);
}

extern "C" void h2_native_pool_idle(hkPoolMemory *, long)
{
}

extern "C" hkPoolMemory *h2_native_pool_destroy(hkPoolMemory *memory, dword)
{
    return memory;
}

typedef void (*h2_native_pool_method)();
extern "C" h2_native_pool_method const g_h2_native_pool_vtable[] =
{
    (h2_native_pool_method)h2_native_pool_allocate,
    (h2_native_pool_method)h2_native_pool_deallocate,
    (h2_native_pool_method)h2_native_pool_allocate_aligned,
    (h2_native_pool_method)h2_native_pool_deallocate_aligned,
    (h2_native_pool_method)h2_native_pool_allocate16,
    (h2_native_pool_method)h2_native_pool_deallocate16,
    (h2_native_pool_method)h2_native_pool_idle,
    (h2_native_pool_method)h2_native_pool_idle,
    (h2_native_pool_method)h2_native_pool_destroy,
    (h2_native_pool_method)h2_native_pool_idle,
    (h2_native_pool_method)h2_native_pool_idle,
};

extern "C" hkPoolMemory *h2_native_havok_pool_memory_construct(void *storage)
{
    hkPoolMemory *memory = (hkPoolMemory *)storage;
    memset(memory, 0, sizeof(*memory));
    *(h2_native_pool_method const **)memory = g_h2_native_pool_vtable;
    memory->m_unknown04 = 1;
    memory->m_unknown14 = NONE;
    s_native_pool_state *state = pool_state(memory);
    state->magic = k_pool_magic;
    state->bump = k_pool_begin;
    return memory;
}

real hkPoolMemory::get_used_fraction(void)
{
    s_native_pool_state *state = pool_state(this);
    if (state->magic != k_pool_magic)
        return 0.0f;
    return (real)state->used_bytes / (real)(k_pool_end - k_pool_begin);
}
#endif
