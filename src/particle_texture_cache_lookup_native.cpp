// Source-local native reconstruction of resident texture-cache lookup.
// The Xbox body remains in unknown_12c0d0.cpp.

#include "unknown_11c920.h"
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
    long unknown40[4];
    D3DTexture *texture;
    long unknown54;
    real minimum_scale;
    long hardware_common;
    dword hardware_data;
    long hardware_lock;
    union
    {
        dword hardware_format;
        struct { short format, width; };
    };
    union
    {
        dword hardware_size;
        struct { short height, depth; };
    };
    long last_frame;
};
static_assert(sizeof(s_bitmap_data) == 0x74, "s_bitmap_data");

struct s_texture_cache_entry
{
    byte unknown00[3];
    bool resident;
    long pending;
    byte unknown08[0x14 - 0x08];
    byte resource[0x0c];
    byte unknown20[8];
};
static_assert(sizeof(s_texture_cache_entry) == 0x28, "s_texture_cache_entry");

extern bool g_468841;
extern bool g_4e6468;
extern s_record_pool *g_4e6454;
extern s_physical_object *g_4e6464;
extern bool g_4e647b;
extern real g_4e647c;
extern long g_4e6488;
extern long g_4e646c;
extern long g_450768[8][24];

short bitmap_get_mipmap_count(short width, short height, short depth,
    short format, bool linear, short maximum_levels);
long log2_floor(dword value);

// @retail 0x12c820
void texture_cache_bitmap_build_texture(s_bitmap_data *bitmap)
{
    D3DTexture *texture = (D3DTexture *)&bitmap->hardware_common;
    dword data = bitmap->hardware_data + g_4e646c;
    short format = bitmap->format;
    long hardware_format = g_450768[0][format];
    if ((bitmap->flags & 0x20) && (format == 10 || format == 11))
        hardware_format = 0x33;
    short depth = bitmap->depth;
    short height = bitmap->height;
    short width = bitmap->width;
    short levels = bitmap_get_mipmap_count(width, height, depth, format,
        ((bitmap->flags >> 4) & 1) != 0, 0);
    texture->Data = 0;
    texture->Lock = 0;
    texture->Common = 0x40001;
    texture->Format = bitmap->type == 2 ? 0x0d : 9;
    texture->Format |= ((((bitmap->type != 1) ? 2 : 3) |
        (hardware_format << 4)) << 4) | ((levels + 1) << 16);
    texture->Format |= (short)log2_floor(width) << 20;
    texture->Format |= (short)log2_floor(height) << 24;
    texture->Format |= (short)log2_floor(depth) << 28;
    texture->Size = 0;
    texture->Data = data & 0x0fffffff;
}

// @retail 0x12c960
D3DTexture *texture_cache_bitmap_get_shared_texture(s_bitmap_data *bitmap)
{
    D3DTexture *result = NULL;
    if (g_4e6468)
    {
        D3DTexture *texture = (D3DTexture *)&bitmap->hardware_common;
        if (texture->Common || bitmap->hardware_lock > 0)
        {
            if ((long)texture->Common <= 0)
                texture_cache_bitmap_build_texture(bitmap);
            result = texture;
        }
    }
    return result;
}

// @retail 0x12cb10
long texture_cache_bitmap_level(s_bitmap_data const *bitmap, real scale)
{
    long result = 0;
    for (long i = 0; i < 3; ++i)
    {
        if (bitmap->data_offsets[i] != NONE)
        {
            long size = bitmap->level_sizes[i];
            if (size && (!i || size > 0x400))
            {
                if ((real)i > scale)
                    break;
                result = i;
            }
        }
    }
    return result;
}

// @retail 0x12ccb0
bool texture_cache_format_scalable(long format)
{
    return (format >= 1 && format <= 11) || format == 17 || format == 18;
}

// @retail 0x12ccf0
D3DTexture *texture_cache_bitmap_get_texture(
    s_bitmap_data *bitmap, dword flags, real bias)
{
    D3DTexture *result = NULL;
    real scale = bias;
    if (g_468841)
    {
        scale += (real)bitmap->level_bias * 0.01f;
        scale += g_4e647c;
    }
    bool unscaled = ((flags >> 2) & 1) != 0;
    if (unscaled)
        scale = 0.0f;
    if (!(bitmap->flags & 0x0200))
        result = bitmap->texture;
    else if (g_4e647b && texture_cache_format_scalable(bitmap->cache_format))
        result = unscaled ? NULL : texture_cache_bitmap_get_shared_texture(bitmap);
    else if (bitmap->minimum_scale > scale)
    {
        long level = scale >= 2.0f ? 2 : scale >= 1.0f ? 1 : 0;
        long block_index = bitmap->block_indices[level];
        if (block_index != NONE)
        {
            s_texture_cache_entry *entry =
                (s_texture_cache_entry *)g_4e6454->data + (block_index & 0xffff);
            ((s_physical_block *)g_4e6464->blocks->data)[block_index & 0xffff].time =
                g_4e6464->time;
            if (entry->resident)
            {
                result = (D3DTexture *)entry->resource;
                if (level == 0)
                {
                    bitmap->last_frame = g_4e6488 + 2;
                    bitmap->texture = result;
                }
            }
        }
    }
    if (bitmap->last_frame >= g_4e6488 && bitmap->texture)
        return bitmap->texture;
    return result;
}
