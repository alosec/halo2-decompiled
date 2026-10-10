// Source-local native ownership for texture-cache physical-block callbacks.
// The Xbox bodies remain in unknown_12c0d0.cpp.

#include "unknown_11c920.h"
#include "async.h"
#include "data_array.h"
#include "physical_memory.h"

struct D3DTexture
{
    dword Common;
    dword Data;
    dword Lock;
    dword Format;
    dword Size;
};
static_assert(sizeof(D3DTexture) == 0x14, "D3DTexture");

struct s_bitmap_data
{
    byte unknown00[0x0a];
    short type;
    byte unknown0c[2];
    word flags;
    byte unknown10[6];
    byte level_bias;
    byte cache_format;
    byte unknown18[4];
    long data_offsets[3];
    long block_indices[3];
    long level_sizes[3];
    long level_addresses[4];
    D3DTexture *texture;
    long unknown54;
    real minimum_scale;
    D3DTexture hardware_texture;
    long last_frame;
};
static_assert(sizeof(s_bitmap_data) == 0x74, "s_bitmap_data");

struct s_texture_cache_entry
{
    short salt;
    byte flags;
    bool resident;
    long pending;
    long priority;
    long hardware_format;
    s_bitmap_data *bitmap;
    D3DTexture resource;
};
static_assert(sizeof(s_texture_cache_entry) == 0x28, "s_texture_cache_entry");

extern s_record_pool *g_4e6454;
extern s_physical_object *g_4e6464;
extern bool g_4e6479;

static __forceinline s_texture_cache_entry *texture_cache_entry_get_native(
    long datum_index)
{
    return (s_texture_cache_entry *)g_4e6454->data + (datum_index & 0xffff);
}

// @retail 0x12d160
byte __stdcall texture_cache_entry_state(long datum_index)
{
    s_texture_cache_entry *entry = texture_cache_entry_get_native(datum_index);
    byte result = 0;
    if (entry->flags & 1)
        return 0x10;
    if (entry->pending)
        result = 0x20;
    return result;
}

// @retail 0x12d1a0
bool __stdcall texture_cache_entry_busy(long datum_index)
{
    s_texture_cache_entry *entry = texture_cache_entry_get_native(datum_index);
    if ((entry->flags & 1) || ((entry->flags & 2) && !g_4e6479))
        return true;
    if (entry->hardware_format == NONE)
        return false;

    // Native cache entries contain an Xbox texture header over host-owned guest
    // upload memory, not a live D3D resource. The async resident bit is therefore
    // the complete native busy state until a host renderer lease is introduced.
    return !*(volatile bool *)&entry->resident;
}

// @retail 0x12d200
void __stdcall texture_cache_block_delete(long datum_index)
{
    s_texture_cache_entry *entry = texture_cache_entry_get_native(datum_index);
    if (entry->bitmap)
    {
        // The allocator consults busy_proc before choosing an eviction candidate.
        // Preserve retail's defensive wait if a direct caller races completion,
        // but do not fabricate an Xbox D3D fence for a header-only resource.
        while (texture_cache_entry_busy(datum_index))
            SwitchToThread();
        entry->bitmap->block_indices[entry->pending] = NONE;
        entry->bitmap->level_addresses[entry->pending] = 0;
        if (entry->pending == 0)
        {
            entry->bitmap->unknown54 = 0;
            entry->bitmap->texture = NULL;
            entry->bitmap->last_frame = 0;
        }
    }
    record_pool_release(g_4e6454, datum_index);
}

// Native map teardown calls the recovered physical allocator flush only after
// the async worker has stopped and the guest queue is gone. This releases every
// cache entry while its source bitmap still belongs to the old map.
extern "C" int h2_native_texture_cache_flush(void)
{
    physical_memory_flush(g_4e6464);
    return g_4e6464->blocks->actual_count == 0 &&
        g_4e6454->actual_count == 0;
}
