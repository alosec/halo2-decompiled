// Source-local native reconstruction of texture-cache request lookup/wait policy.
// The Xbox body remains in unknown_12c0d0.cpp.

#include "unknown_11c920.h"
#include "data_array.h"
#include "physical_memory.h"
#include "async.h"

struct D3DTexture;

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
    long unknown40[4];
    D3DTexture *texture;
    long unknown54;
    real minimum_scale;
    long hardware_common;
    dword hardware_data;
    long hardware_lock;
    dword hardware_format;
    dword hardware_size;
    long last_frame;
};
static_assert(sizeof(s_bitmap_data) == 0x74, "s_bitmap_data");

struct s_texture_cache_entry
{
    byte unknown00[2];
    byte flags;
    bool resident;
    long pending;
    byte unknown08[4];
    long hardware_format;
    s_bitmap_data *bitmap;
    byte resource[0x0c];
    byte unknown20[8];
};
static_assert(sizeof(s_texture_cache_entry) == 0x28, "s_texture_cache_entry");

extern s_record_pool *g_4e6454;
extern s_physical_object *g_4e6464;
extern long g_4e6484;

bool function_12d5a0(s_bitmap_data *bitmap, long level, bool blocking,
    bool alternate, long priority);
bool function_120ce0(long task, long priority);
void function_120d50(volatile bool *complete, bool wait);

static __forceinline s_texture_cache_entry *texture_cache_entry_get_native(long datum_index)
{
    return (s_texture_cache_entry *)g_4e6454->data + (datum_index & 0xffff);
}

// @retail 0x12c990
D3DTexture *function_12c990(s_bitmap_data *bitmap, long level, bool request,
    bool blocking, bool alternate, long priority, bool *waited, bool *failed)
{
    D3DTexture *texture = NULL;
    bool throttled = async_globals.tasks_added > 25 || g_4e6484 >= 10;
    if (request && (blocking || priority > 2 || !throttled))
    {
        if (bitmap->data_offsets[level] != NONE && bitmap->level_sizes[level] &&
            (!level || bitmap->level_sizes[level] > 0x400))
        {
            if (bitmap->block_indices[level] == NONE)
            {
                if (!function_12d5a0(bitmap, level, blocking, alternate, priority))
                    *failed = true;
            }
            if (bitmap->block_indices[level] != NONE)
            {
                long block_index = bitmap->block_indices[level];
                s_texture_cache_entry *entry = texture_cache_entry_get_native(block_index);
                ((s_physical_block *)g_4e6464->blocks->data)[block_index & 0xffff].time =
                    g_4e6464->time;
                if (!*(volatile bool *)&entry->resident)
                {
                    if (blocking)
                    {
                        *waited = true;
                        function_120ce0(entry->hardware_format, 7);
                        function_120d50(&entry->resident, true);
                    }
                    else if (priority > *(long *)entry->unknown08)
                    {
                        *(long *)entry->unknown08 = priority;
                        function_120ce0(entry->hardware_format, priority);
                    }
                }
                if (*(volatile bool *)&entry->resident)
                    texture = (D3DTexture *)&entry->resource;
            }
        }
    }
    else if (bitmap->block_indices[level] != NONE)
    {
        long block_index = bitmap->block_indices[level];
        s_texture_cache_entry *entry = texture_cache_entry_get_native(block_index);
        ((s_physical_block *)g_4e6464->blocks->data)[block_index & 0xffff].time =
            g_4e6464->time;
        if (*(volatile bool *)&entry->resident)
            texture = (D3DTexture *)&entry->resource;
    }
    return texture;
}
