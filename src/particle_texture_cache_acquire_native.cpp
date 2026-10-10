// Source-local native reconstruction of material bitmap cache acquisition.
// The Xbox body remains in unknown_01cf50.cpp.

#include "unknown_03bcb0.h"

struct s_bitmap_view;

PRIVATE __forceinline D3DTexture *fetch_bitmap_texture_native(
    s_bitmap_view *bitmap, dword flags, real priority)
{
    s_bitmap_predict_view *view = (s_bitmap_predict_view *)bitmap;
    D3DTexture *texture = NULL;
    if (view->last_frame > g_4e6488)
        texture = view->texture;
    if (!texture)
    {
        __builtin_prefetch(&view->flags, 0, 3);
        __builtin_prefetch(&view->data_offset, 0, 3);
        __builtin_prefetch(&view->data_offset1, 0, 3);
        __builtin_prefetch(&view->data_offset2, 0, 3);
        __builtin_prefetch(view->texture, 0, 3);
        texture = texture_cache_bitmap_get_texture(
            (s_bitmap_data *)bitmap, flags, priority);
        if (!texture)
            texture = function_12ce00((s_bitmap_data *)bitmap, flags, priority);
    }
    return texture;
}

// @retail 0x12310
D3DTexture *function_12310(s_bitmap_view *bitmap, real priority)
{
    return fetch_bitmap_texture_native(bitmap, 3, priority);
}
