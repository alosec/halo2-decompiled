// Source-local native reconstruction of the complete texture upload request policy.
// The Xbox body remains represented by the retail function inventory.

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
    dword signature;
    short width;
    short height;
    char depth;
    byte unknown09;
    short type;
    short source_format;
    word flags;
    byte unknown10[4];
    short mipmap_count;
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

extern bool g_46883c;
extern bool g_46883d;
extern bool g_468841;
extern s_record_pool *g_4e6454;
extern s_physical_object *g_4e6464;
extern bool g_4e6468;
extern long g_4e6470;
extern long g_4e6474;
extern bool g_4e647a;
extern bool g_4e647b;
extern real g_4e647c;
struct D3DResource;
extern D3DResource *g_485ae4;
extern D3DResource *g_485ae8;
extern D3DResource *g_485aec;

void texture_cache_bitmap_build_texture(s_bitmap_data *bitmap);
long texture_cache_bitmap_level(s_bitmap_data const *bitmap, real scale);
bool texture_cache_format_scalable(long format);
D3DTexture *function_12c990(s_bitmap_data *bitmap, long level, bool request,
    bool blocking, bool alternate, long priority, bool *waited, bool *failed);

// @retail 0x12ce00
D3DTexture *function_12ce00(s_bitmap_data *bitmap, dword flags, real bias)
{
    bool request = (flags & 1) != 0;
    bool blocking = ((flags >> 1) & 1) != 0;
    bool unscaled = ((flags >> 2) & 1) != 0;
    D3DTexture *shared_texture = NULL;
    bool alternate = false;
    bool waited = false;
    bool failed = false;

    real scale = bias;
    if (g_468841)
        scale += (real)bitmap->level_bias * 0.01f + g_4e647c;
    if (g_46883d)
        scale = 0.0f;
    if (g_46883c)
        scale = 1.0f;

    if (!(bitmap->flags & 0x0200))
        return bitmap->texture;

    if (!unscaled)
    {
        if (g_4e6468)
        {
            D3DTexture *texture = &bitmap->hardware_texture;
            if (texture->Common || bitmap->hardware_texture.Lock > 0)
            {
                if ((long)texture->Common <= 0)
                    texture_cache_bitmap_build_texture(bitmap);
                shared_texture = texture;
            }
        }

        if (g_4e6470 > 0 &&
            (!g_4e6474 || (g_4e6474 == 1 && bitmap->cache_format == 3)))
        {
            alternate = true;
            blocking = g_4e6470 == 1;
        }
        else if (shared_texture && !(bitmap->flags & 0x0080))
        {
            blocking = false;
        }
    }

    if (((g_4e647b && texture_cache_format_scalable(bitmap->cache_format)) ||
         scale >= bitmap->minimum_scale) && shared_texture)
    {
        return shared_texture;
    }

    long level = unscaled ? 0 : texture_cache_bitmap_level(bitmap, scale);
    long priority = 2;
    if (!alternate)
    {
        if (blocking)
            priority = 7;
        else if (flags & 8)
            priority = 5;
        else if (flags & 0x10)
            priority = 1;
    }

    D3DTexture *texture = function_12c990(bitmap, level, request, blocking,
        alternate, priority, &waited, &failed);
    if (!texture && !waited)
    {
        for (long index = 0; index < 3; index++)
        {
            long block_index = bitmap->block_indices[index];
            if (block_index == NONE)
                continue;
            s_texture_cache_entry *entry =
                (s_texture_cache_entry *)g_4e6454->data + (block_index & 0xffff);
            ((s_physical_block *)g_4e6464->blocks->data)[block_index & 0xffff].time =
                g_4e6464->time;
            if (entry->resident)
            {
                texture = &entry->resource;
                blocking = false;
            }
        }

        if (blocking && !texture)
        {
            texture = (D3DTexture *)(bitmap->type == 0 ? g_485ae4 :
                bitmap->type == 1 ? g_485ae8 : g_485aec);
            g_4e647a = true;
        }
    }

    if (failed)
        g_4e647a = true;
    return texture;
}
