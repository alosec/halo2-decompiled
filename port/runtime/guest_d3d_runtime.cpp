/* Xbox Direct3D 8 for whole images: a native device that keeps the state the
 * recovered game sets, so its real initialization and main loop run against a
 * valid device, and hands every draw to the host renderer.
 *
 * Written from the public Direct3D 8 API semantics and from what the recovered
 * source does with the results (see the comments citing src/ units); no SDK,
 * xemu or Cxbx-Reloaded material.
 *
 * Draws, clears and Begin/End batches go to the host renderer as packets
 * (h2/gpu_protocol.h).
 *
 * Memory model. Resource headers carry Data as a physical address and the game
 * reads storage back as Data | 0x80000000 (src/unknown_0158f0.cpp,
 * src/unknown_03f2b0.cpp), so all storage comes from the host's contiguous
 * region (0x88000000 up), where that conversion is the identity. */
#include "h2/host_abi.h"
#include "h2/gpu_protocol.h"
#include <xtl.h>
#include <stddef.h>
#include <string.h>

extern "C" int _snprintf(char *destination, __SIZE_TYPE__ capacity, const char *format, ...);

/* Declared the same way as the recovered units that call them
   (src/unknown_015d00.cpp, unknown_020560.cpp, unknown_023540.cpp), so the
   mangled names match. */
namespace D3D
{
struct CDevice
{
    void LazySetStateVB(DWORD value);
    static DWORD m_PushSegmentSize;
};

DWORD WINAPI SetFence(DWORD flags);
DWORD volatile *WINAPI MakeRequestedSpace(DWORD fallback_size, DWORD requested_size);

namespace PixelJar
{
void __stdcall FindSurfaceWithinTexture(D3DPixelContainer *container, D3DCUBEMAP_FACES face,
                                        unsigned int level, BYTE **bits, DWORD *pitch,
                                        DWORD *width, DWORD *height, DWORD *slice);
}
} // namespace D3D

/* Library state the recovered code reads and writes directly. The render and
   texture-stage state of the device lives here, not in the device struct:
   src/unknown_01cf50.cpp stores D3D__RenderState and D3D__TextureState itself
   and marks D3D__DirtyFlags. */
D3DDevice *D3D__pDevice;
DWORD D3D__DirtyFlags;
DWORD D3D__RenderState[D3DRS_MAX];
DWORD D3D__TextureState[4][D3DTSS_MAX];
DWORD D3D__CommandBufferSize = 0x80000;
DWORD D3D__SegmentSize = 0x8000;
DWORD D3D::CDevice::m_PushSegmentSize = 0x8000;

/* PROVISIONAL: the port's own encoding. The game passes these values straight
   back to D3DDevice_SetRenderState_Simple (src/unknown_01cf50.cpp), so the
   state index itself serves until the NV2A methods are recovered. */
#define H2_ENCODE4(n) (n), (n) + 1, (n) + 2, (n) + 3
#define H2_ENCODE16(n) H2_ENCODE4(n), H2_ENCODE4((n) + 4), H2_ENCODE4((n) + 8), H2_ENCODE4((n) + 12)
DWORD const D3DSIMPLERENDERSTATEENCODE[D3DRS_MAX] = {
    H2_ENCODE16(0x00), H2_ENCODE16(0x10), H2_ENCODE16(0x20), H2_ENCODE16(0x30),
    H2_ENCODE16(0x40), H2_ENCODE16(0x50), H2_ENCODE16(0x60), H2_ENCODE16(0x70),
    H2_ENCODE16(0x80), H2_ENCODE16(0x90), H2_ENCODE4(0xa0), 0xa4, 0xa5,
};
#undef H2_ENCODE16
#undef H2_ENCODE4
static_assert(sizeof(D3DSIMPLERENDERSTATEENCODE) == D3DRS_MAX * 4, "one encoding per state");

namespace {

/* Resource header bits, as the recovered code fills them: Common is a 16-bit
   reference count under a type (palette 0x30001, texture 0x40001, surface
   0x50001: src/unknown_0158f0.cpp, unknown_12c0d0.cpp, unknown_01cf50.cpp).
   Format packs flags (4 = cube map), the dimension count at bits 4-7, the
   hardware format at 8-15, the level count at 16-19 and the log2 sizes at
   20-31; linear formats describe themselves in Size instead (width - 1,
   height - 1 << 12, pitch / 64 - 1 << 24; src/unknown_01dcc0.cpp). */
constexpr DWORD k_common_refcount = 0x0000ffff;
constexpr DWORD k_common_palette = 0x00030000;
constexpr DWORD k_common_texture = 0x00040000;
constexpr DWORD k_common_surface = 0x00050000;
constexpr DWORD k_format_cube = 0x00000004;
/* the low flag bits every header in the recovered source carries */
constexpr DWORD k_format_base = 0x00000009;

/* the game's back buffer and depth formats: function_1d6f0 (src/unknown_0359e0.cpp)
   builds its linear colour surfaces with 0x12 and depth ones with 0x2e */
constexpr DWORD k_hardware_linear_a8r8g8b8 = 0x12;
constexpr DWORD k_hardware_linear_depth = 0x2e;

constexpr long k_vertex_constant_bias = 96;
constexpr long k_vertex_constant_count = 192;
constexpr long k_stream_count = 16;
constexpr long k_vertex_attribute_count = 16;
constexpr long k_scissor_count = 8;
constexpr long k_visibility_count = 4096;

/* The game writes up to 0x80 dwords past the threshold without asking for
   room (function_015d00, src/unknown_015d00.cpp), so the threshold stays well
   short of the end of the buffer. */
constexpr DWORD k_push_slack_dwords = 0x400;

struct h2_d3d_stream
{
    DWORD buffer;
    DWORD stride;
    DWORD offset;
};
static_assert(sizeof(h2_d3d_stream) == 12, "src/unknown_01cf50.cpp s_shader_cache streams");

/* the lock rectangle: the public RECT layout */
struct h2_d3d_lock_rect
{
    LONG left, top, right, bottom;
};

struct h2_d3d_push_stats
{
    DWORD segments;
    DWORD dwords;
    DWORD wraps;
    DWORD grows;
    DWORD const *last_begin;
    DWORD const *last_end;
};

/* Xbox D3DVBLANKDATA: main_vblank_callback's function_14280 compares the
   second dword with the last one it saw to learn whether a frame was
   presented (src/unknown_01cf50.cpp). */
struct h2_d3d_vblank_data
{
    DWORD vblank;
    DWORD swap;
    DWORD flags;
};

struct h2_d3d_device
{
    /* +0x00 put, +0x04 threshold, +0x08 state flags: the recovered code
       reserves push-buffer room through these in place (src/unknown_015d00.cpp,
       src/unknown_020560.cpp function_1ef70 and function_1ed70) */
    DWORD *push_put;
    DWORD *push_threshold;
    DWORD state_flags;

    DWORD reference_count;
    bool created;
    D3DPRESENT_PARAMETERS parameters;

    DWORD *push_base;
    DWORD *push_end;
    DWORD *push_consumed;
    h2_d3d_push_stats push;

    D3DBaseTexture *textures[4];
    D3DPalette *palettes[4];
    DWORD vertex_shader;
    DWORD const *vertex_program;
    DWORD vertex_program_address;
    void const *vertex_attribute_formats;
    DWORD vertex_shader_select_address;
    float vertex_constants[k_vertex_constant_count][4];
    h2_d3d_stream streams[k_stream_count];
    long stream_count;
    void const *stream_formats;
    DWORD vertex_data[k_vertex_attribute_count][4];
    D3DIndexBuffer *indices;
    UINT base_vertex;
    DWORD pixel_shader;
    D3DPIXELSHADERDEF pixel_shader_program;
    bool pixel_shader_program_set;

    D3DSurface *back_buffers[2]; /* [0] the back buffer, [1] the front buffer */
    D3DSurface *auto_depth_stencil;
    D3DSurface *render_target;
    D3DSurface *depth_stencil;
    D3DVIEWPORT8 viewport;
    D3DRECT scissors[k_scissor_count];
    DWORD scissor_count;
    BOOL scissor_exclusive;
    DWORD stipple[32];
    float clip_near;
    float clip_far;
    DWORD clip_flags;
    float back_buffer_scale_x;
    float back_buffer_scale_y;
    DWORD flicker_filter;
    BOOL soft_display_filter;
    D3DGAMMARAMP gamma_ramp;

    D3DVBLANKCALLBACK vblank_callback;
    DWORD vblank_count;
    DWORD presented_frames;
    DWORD fence;
    DWORD visibility[k_visibility_count];
    DWORD draw_calls;
    DWORD clears;
};

h2_d3d_device the_device;

/* Headers the port creates, with the storage they own. A header outside this
   pool belongs to the game, which builds many in its own memory. */
struct h2_d3d_resource_slot
{
    DWORD header[6]; /* the largest header, D3DSurface */
    DWORD storage;
    D3DResource *parent;
    bool in_use;
    bool device_owned;
};

constexpr long k_resource_slots = 2048;
h2_d3d_resource_slot resource_pool[k_resource_slots];
long resource_next;

void log_message(char const *format, DWORD a = 0, DWORD b = 0, DWORD c = 0)
{
    char text[192];
    _snprintf(text, sizeof(text), format, a, b, c);
    host_h2_log(text);
}

h2_d3d_device *device()
{
    return &the_device;
}

h2_d3d_resource_slot *slot_of(void const *resource)
{
    DWORD address = reinterpret_cast<DWORD>(resource);
    DWORD base = reinterpret_cast<DWORD>(&resource_pool[0]);
    if (address < base || address >= base + sizeof(resource_pool))
        return NULL;
    if ((address - base) % sizeof(h2_d3d_resource_slot))
        return NULL;
    return &resource_pool[(address - base) / sizeof(h2_d3d_resource_slot)];
}

h2_d3d_resource_slot *slot_allocate()
{
    for (long i = 0; i < k_resource_slots; ++i) {
        h2_d3d_resource_slot *slot = &resource_pool[(resource_next + i) % k_resource_slots];
        if (!slot->in_use) {
            resource_next = (resource_next + i + 1) % k_resource_slots;
            memset(slot, 0, sizeof(*slot));
            slot->in_use = true;
            return slot;
        }
    }
    log_message("h2-port: d3d resource pool exhausted (%u headers)", k_resource_slots);
    return NULL;
}

DWORD storage_allocate(DWORD size)
{
    DWORD address = host_h2_xbox_memory_alloc(size ? size : 1, 0, 1, 0);
    if (address)
        memset(reinterpret_cast<void *>(address), 0, size);
    return address;
}

void storage_free(DWORD address)
{
    if (address)
        host_h2_xbox_memory_free(address);
}

DWORD physical_of(DWORD address)
{
    return address & 0x7fffffff;
}

BYTE *pointer_of(DWORD data)
{
    return data ? reinterpret_cast<BYTE *>(data | 0x80000000) : NULL;
}

DWORD add_reference(D3DResource *resource)
{
    if (!resource)
        return 0;
    DWORD count = (resource->Common & k_common_refcount) + 1;
    if (count <= k_common_refcount)
        resource->Common = (resource->Common & ~k_common_refcount) | count;
    return resource->Common & k_common_refcount;
}

DWORD release_reference(D3DResource *resource);

void slot_destroy(h2_d3d_resource_slot *slot)
{
    storage_free(slot->storage);
    D3DResource *parent = slot->parent;
    memset(slot, 0, sizeof(*slot));
    if (parent)
        release_reference(parent);
}

DWORD release_reference(D3DResource *resource)
{
    if (!resource)
        return 0;
    DWORD count = resource->Common & k_common_refcount;
    if (count)
        resource->Common = (resource->Common & ~k_common_refcount) | --count;
    h2_d3d_resource_slot *slot = slot_of(resource);
    if (!count && slot && slot->in_use && !slot->device_owned)
        slot_destroy(slot);
    return count;
}

/* Hardware formats. The game's linear set is function_1d4b0's
   (src/unknown_01cf50.cpp); the sizes follow the bitmap formats that
   g_450768 (src/unknown_0158f0.cpp) maps onto each code. */
bool format_linear(DWORD code)
{
    switch (code) {
    case 0x10: case 0x11: case 0x12: case 0x13: case 0x16: case 0x17:
    case 0x1b: case 0x1c: case 0x1d: case 0x1e: case 0x1f: case 0x20:
    case 0x2e: case 0x35: case 0x37: case 0x3d: case 0x3e: case 0x3f: case 0x40: case 0x41:
        return true;
    }
    return false;
}

bool format_compressed(DWORD code)
{
    return code == 0x0c || code == 0x0e || code == 0x0f;
}

DWORD format_bits(DWORD code)
{
    switch (code) {
    case 0x00: case 0x01: case 0x0b: case 0x0e: case 0x0f: case 0x13: case 0x19:
    case 0x1b: case 0x1f:
        return 8;
    case 0x0c:
        return 4;
    case 0x02: case 0x03: case 0x04: case 0x05: case 0x10: case 0x11: case 0x17:
    case 0x1a: case 0x1d: case 0x20: case 0x28:
        return 16;
    }
    /* PROVISIONAL: every other code is taken as 32 bits per pixel */
    return 32;
}

DWORD log2_ceiling(DWORD value)
{
    DWORD bits = 0;
    while ((1u << bits) < value && bits < 15)
        ++bits;
    return bits;
}

DWORD align_up(DWORD value, DWORD alignment)
{
    return (value + alignment - 1) & ~(alignment - 1);
}

/* the geometry a pixel container's Format and Size describe */
struct h2_d3d_layout
{
    DWORD code;
    DWORD width, height, depth;
    DWORD levels;
    DWORD pitch; /* linear only */
    bool linear;
    bool cube;
};

struct h2_d3d_level
{
    DWORD offset;
    DWORD pitch;
    DWORD slice;
    DWORD width, height, depth;
};

h2_d3d_layout layout_of(D3DPixelContainer const *container)
{
    h2_d3d_layout layout = {};
    DWORD format = container->Format;
    DWORD size = container->Size;
    layout.code = (format >> 8) & 0xff;
    layout.cube = (format & k_format_cube) != 0;
    layout.levels = (format >> 16) & 0xf;
    if (!layout.levels)
        layout.levels = 1;
    if (size) {
        layout.linear = true;
        layout.width = (size & 0xfff) + 1;
        layout.height = ((size >> 12) & 0xfff) + 1;
        layout.pitch = ((size >> 24) + 1) * 64;
        layout.depth = 1;
        layout.levels = 1;
    } else {
        layout.width = 1u << ((format >> 20) & 0xf);
        layout.height = 1u << ((format >> 24) & 0xf);
        layout.depth = ((format >> 4) & 0xf) == 3 ? 1u << ((format >> 28) & 0xf) : 1;
    }
    return layout;
}

h2_d3d_level level_of(h2_d3d_layout const &layout, DWORD level)
{
    h2_d3d_level result = {};
    DWORD bits = format_bits(layout.code);
    for (DWORD i = 0;; ++i) {
        DWORD width = layout.width >> i ? layout.width >> i : 1;
        DWORD height = layout.height >> i ? layout.height >> i : 1;
        DWORD depth = layout.depth >> i ? layout.depth >> i : 1;
        DWORD pitch, rows;
        if (layout.linear) {
            pitch = layout.pitch;
            rows = height;
        } else if (format_compressed(layout.code)) {
            /* 4x4 blocks of 8 (4 bits per pixel) or 16 bytes */
            pitch = ((width + 3) / 4) * bits * 2;
            rows = (height + 3) / 4;
        } else {
            pitch = width * bits / 8;
            rows = height;
        }
        if (!pitch)
            pitch = 1;
        if (i == level) {
            result.pitch = pitch;
            result.slice = pitch * rows;
            result.width = width;
            result.height = height;
            result.depth = depth;
            return result;
        }
        result.offset += pitch * rows * depth;
    }
}

/* PROVISIONAL: cube faces follow each other, each with its whole mip chain,
   128-byte aligned. Only the port's own textures rely on this. */
DWORD face_size(h2_d3d_layout const &layout)
{
    h2_d3d_level last = level_of(layout, layout.levels - 1);
    return align_up(last.offset + last.slice * last.depth, 128);
}

DWORD storage_size(h2_d3d_layout const &layout)
{
    return face_size(layout) * (layout.cube ? 6 : 1);
}

BYTE *level_pointer(D3DPixelContainer const *container, DWORD face, DWORD level,
                    h2_d3d_level *out)
{
    h2_d3d_layout layout = layout_of(container);
    if (level >= layout.levels)
        level = layout.levels - 1;
    *out = level_of(layout, level);
    BYTE *base = pointer_of(container->Data);
    if (!base)
        return NULL;
    DWORD offset = out->offset + (layout.cube ? (face % 6) * face_size(layout) : 0);
    return base + offset;
}

DWORD linear_size_field(DWORD width, DWORD height, DWORD pitch)
{
    return ((pitch / 64 - 1) << 24) | ((height - 1) << 12) | (width - 1);
}

/* fills a pixel container header and gives it fresh storage */
bool pixel_container_create(h2_d3d_resource_slot *slot, DWORD common, DWORD width, DWORD height,
                            DWORD depth, DWORD levels, DWORD code, bool cube, bool volume)
{
    D3DPixelContainer *container = reinterpret_cast<D3DPixelContainer *>(slot->header);
    if (!width) width = 1;
    if (!height) height = 1;
    if (!depth) depth = 1;
    container->Common = common | 1;
    container->Lock = 0;
    if (format_linear(code)) {
        DWORD pitch = align_up(width * format_bits(code) / 8, 64);
        container->Format = k_format_base | (2 << 4) | (code << 8) | (1 << 16);
        container->Size = linear_size_field(width, height, pitch);
    } else {
        DWORD width_bits = log2_ceiling(width);
        DWORD height_bits = log2_ceiling(height);
        DWORD depth_bits = volume ? log2_ceiling(depth) : 0;
        DWORD most = width_bits > height_bits ? width_bits : height_bits;
        most = depth_bits > most ? depth_bits : most;
        if (!levels || levels > most + 1)
            levels = most + 1;
        if (levels > 15)
            levels = 15;
        container->Format = k_format_base | (cube ? k_format_cube : 0) | ((volume ? 3u : 2u) << 4) |
                            (code << 8) | (levels << 16) | (width_bits << 20) |
                            (height_bits << 24) | (depth_bits << 28);
        container->Size = 0;
    }
    DWORD size = storage_size(layout_of(container));
    slot->storage = storage_allocate(size);
    if (!slot->storage) {
        log_message("h2-port: d3d storage of 0x%x bytes failed", size);
        return false;
    }
    container->Data = physical_of(slot->storage);
    return true;
}

D3DSurface *surface_create(DWORD width, DWORD height, DWORD code)
{
    h2_d3d_resource_slot *slot = slot_allocate();
    if (!slot)
        return NULL;
    slot->device_owned = true;
    if (!pixel_container_create(slot, k_common_surface, width, height, 1, 1, code, false, false)) {
        slot_destroy(slot);
        return NULL;
    }
    return reinterpret_cast<D3DSurface *>(slot->header);
}

void surface_destroy(D3DSurface *surface)
{
    h2_d3d_resource_slot *slot = slot_of(surface);
    if (slot && slot->in_use)
        slot_destroy(slot);
}

/* PROVISIONAL: the present parameters use public PC format values (d3d8.h) */
DWORD hardware_format(D3DFORMAT format)
{
    if (format == D3DFMT_A8R8G8B8)
        return k_hardware_linear_a8r8g8b8;
    if (format == D3DFMT_D24S8 || format == D3DFMT_D24S8 + 1)
        return k_hardware_linear_depth;
    return static_cast<DWORD>(format) & 0xff;
}

/* the push buffer */

/* Where a future NV2A method parser consumes what the game and the library
   wrote between two submissions. */
/* NV2A method counts for the trace (H2_TRACE=d3d): methods are 4-byte
   aligned, so index by method / 4 within the 3D class's 0x2000 range */
DWORD g_trace_method_counts[0x800];

void h2_d3d_draw_pushed_array(DWORD primitive, DWORD const *data, DWORD words);

/* NV2A methods the game pushes itself (src/unknown_01cf50.cpp function_4b220,
   src/unknown_23d970.cpp): begin/end (0x17fc) around an inline vertex array
   (0x1818, sent without incrementing the method) is a draw */
constexpr DWORD k_method_begin_end = 0x17fc;
constexpr DWORD k_method_inline_array = 0x1818;

void h2_d3d_parse_push(DWORD const *begin, DWORD const *end)
{
    const bool trace = host_h2_trace_flags() & 1;
    DWORD primitive = 0;
    for (DWORD const *word = begin; word < end;)
    {
        const DWORD header = *word++;
        if (header & 3)
            break; /* a jump or call: nothing the game writes yet */
        const DWORD count = (header >> 18) & 0x7ff;
        const DWORD method = header & 0x1ffc;
        if (trace)
            ++g_trace_method_counts[method / 4];
        if (method == k_method_begin_end && count >= 1)
            primitive = word[0];
        else if (method == k_method_inline_array && primitive && count)
            h2_d3d_draw_pushed_array(primitive, word, count);
        word += count;
    }
}

/* hands everything written since the last submission to the parser */
void push_submit(h2_d3d_device *d)
{
    if (!d->push_base)
        return;
    DWORD *put = d->push_put;
    if (put < d->push_consumed || put > d->push_end) {
        log_message("h2-port: d3d push pointer 0x%x left the buffer 0x%x-0x%x",
                    reinterpret_cast<DWORD>(put), reinterpret_cast<DWORD>(d->push_base),
                    reinterpret_cast<DWORD>(d->push_end));
        put = d->push_put = d->push_consumed;
    }
    if (put == d->push_consumed)
        return;
    h2_d3d_parse_push(d->push_consumed, put);
    ++d->push.segments;
    d->push.dwords += static_cast<DWORD>(put - d->push_consumed);
    d->push.last_begin = d->push_consumed;
    d->push.last_end = put;
    d->push_consumed = put;
}

bool push_allocate(h2_d3d_device *d, DWORD bytes)
{
    bytes = align_up(bytes < 0x10000 ? 0x10000 : bytes, 0x1000);
    DWORD storage = storage_allocate(bytes);
    if (!storage) {
        log_message("h2-port: d3d push buffer of 0x%x bytes failed", bytes);
        return false;
    }
    storage_free(reinterpret_cast<DWORD>(d->push_base));
    d->push_base = reinterpret_cast<DWORD *>(storage);
    d->push_end = d->push_base + bytes / 4;
    d->push_put = d->push_consumed = d->push_base;
    d->push_threshold = d->push_end - k_push_slack_dwords;
    return true;
}

/* room for 'dwords' at the put pointer. Nothing reads the buffer behind the
   game, so once submitted the buffer simply restarts from its base. */
DWORD *push_reserve(h2_d3d_device *d, DWORD dwords)
{
    if (!d->push_base)
        return NULL;
    push_submit(d);
    if (d->push_put + dwords + k_push_slack_dwords <= d->push_end)
        return d->push_put;
    DWORD capacity = static_cast<DWORD>(d->push_end - d->push_base);
    if (dwords + k_push_slack_dwords > capacity) {
        ++d->push.grows;
        if (!push_allocate(d, (dwords + k_push_slack_dwords) * 8))
            return NULL;
    } else {
        ++d->push.wraps;
        d->push_put = d->push_consumed = d->push_base;
    }
    return d->push_put;
}

/* draw submission: each draw and clear becomes a packet for the host
   renderer (h2/gpu_protocol.h), with the fixed-function state normalised
   from this header's D3D values. The push buffer is submitted first so
   packets and what the game pushed itself stay in call order. */

h2_gpu_draw g_draw_packet;
h2_gpu_clear g_clear_packet;

/* the NV2A's vertex program memory: 136 instructions of 4 words */
constexpr long k_program_slots = 136;
DWORD g_program_memory[k_program_slots][4];
DWORD g_program_start;
bool g_vertex_program_selected;

/* Begin/End vertices: every register's value as each vertex was emitted */
constexpr long k_inline_capacity = 4096;
float g_inline_vertices[k_inline_capacity][k_vertex_attribute_count][4];
long g_inline_count;
DWORD g_inline_primitive;
bool g_inline_active;
float g_vertex_registers[k_vertex_attribute_count][4];

h2_gpu_u32 guest_address(void const *pointer)
{
    return static_cast<h2_gpu_u32>(reinterpret_cast<DWORD>(pointer));
}

/* resource storage: Data is physical; the game reads it through the
   0x80000000 mirror */
h2_gpu_u32 resource_data(DWORD data)
{
    return data ? (data | 0x80000000u) : 0;
}

h2_gpu_u32 normal_compare(DWORD value)
{
    return value >= D3DCMP_NEVER && value <= D3DCMP_ALWAYS ? value - D3DCMP_NEVER : H2_GPU_ALWAYS;
}

h2_gpu_u32 normal_blend(DWORD value)
{
    switch (value) {
    case D3DBLEND_ZERO: return H2_GPU_ZERO;
    case D3DBLEND_ONE: return H2_GPU_ONE;
    case D3DBLEND_SRCCOLOR: return H2_GPU_SRC_COLOR;
    case D3DBLEND_INVSRCCOLOR: return H2_GPU_INV_SRC_COLOR;
    case D3DBLEND_SRCALPHA: return H2_GPU_SRC_ALPHA;
    case D3DBLEND_INVSRCALPHA: return H2_GPU_INV_SRC_ALPHA;
    case D3DBLEND_DESTALPHA: return H2_GPU_DST_ALPHA;
    case D3DBLEND_INVDESTALPHA: return H2_GPU_INV_DST_ALPHA;
    case D3DBLEND_DESTCOLOR: return H2_GPU_DST_COLOR;
    case D3DBLEND_INVDESTCOLOR: return H2_GPU_INV_DST_COLOR;
    case D3DBLEND_SRCALPHASAT: return H2_GPU_SRC_ALPHA_SATURATE;
    case D3DBLEND_CONSTANTCOLOR: return H2_GPU_CONSTANT_COLOR;
    case D3DBLEND_INVCONSTANTCOLOR: return H2_GPU_INV_CONSTANT_COLOR;
    case D3DBLEND_CONSTANTALPHA: return H2_GPU_CONSTANT_ALPHA;
    case D3DBLEND_INVCONSTANTALPHA: return H2_GPU_INV_CONSTANT_ALPHA;
    default: return H2_GPU_ONE;
    }
}

/* the blend equations take the OpenGL values the verified D3DBLENDOP_ADD
   (0x8006) belongs to */
h2_gpu_u32 normal_blend_op(DWORD value)
{
    switch (value) {
    case 0x800a: return H2_GPU_SUBTRACT;
    case 0x800b: return H2_GPU_REVERSE_SUBTRACT;
    case 0x8007: return H2_GPU_MIN;
    case 0x8008: return H2_GPU_MAX;
    default: return H2_GPU_ADD;
    }
}

h2_gpu_u32 normal_stencil_op(DWORD value)
{
    switch (value) {
    case D3DSTENCILOP_KEEP: return H2_GPU_KEEP;
    case D3DSTENCILOP_ZERO: return H2_GPU_STENCIL_ZERO;
    case D3DSTENCILOP_REPLACE: return H2_GPU_REPLACE;
    case D3DSTENCILOP_INCRSAT: return H2_GPU_INCREMENT_CLAMP;
    case D3DSTENCILOP_DECRSAT: return H2_GPU_DECREMENT_CLAMP;
    case D3DSTENCILOP_INVERT: return H2_GPU_INVERT;
    case 0x8507: return H2_GPU_INCREMENT_WRAP;   /* the OpenGL values, as above */
    case 0x8508: return H2_GPU_DECREMENT_WRAP;
    default: return H2_GPU_KEEP;
    }
}

h2_gpu_u32 normal_address(DWORD value)
{
    switch (value) {
    case D3DTADDRESS_WRAP: return H2_GPU_WRAP;
    case D3DTADDRESS_MIRROR: return H2_GPU_MIRROR;
    case D3DTADDRESS_CLAMP: return H2_GPU_CLAMP;
    case D3DTADDRESS_BORDER: return H2_GPU_BORDER;
    case 5: return H2_GPU_CLAMP_TO_EDGE;   /* D3DTADDRESS_CLAMPTOEDGE */
    default: return H2_GPU_WRAP;
    }
}

h2_gpu_u32 normal_filter(DWORD value)
{
    switch (value) {
    case D3DTEXF_NONE: return H2_GPU_FILTER_NONE;
    case D3DTEXF_POINT: return H2_GPU_FILTER_POINT;
    default: return H2_GPU_FILTER_LINEAR;   /* linear, anisotropic, quincunx, gaussian */
    }
}

/* D3DRS_COLORWRITEENABLE: the Xbox puts red at bit 16, green at 8, blue at
   0 and alpha at 24; a value with only the low four bits set is taken to be
   the PC layout (red 1, green 2, blue 4, alpha 8) */
h2_gpu_u32 normal_color_mask(DWORD value)
{
    if (value & ~0xfu)
        return ((value >> 16) & 1) | (((value >> 8) & 1) << 1) | ((value & 1) << 2) |
               (((value >> 24) & 1) << 3);
    return value & 0xf;
}

float float_bits(DWORD value)
{
    float result;
    memcpy(&result, &value, 4);
    return result;
}

void fill_texture(h2_gpu_texture *out, D3DBaseTexture *texture, D3DPalette *palette,
                  DWORD const *stage)
{
    memset(out, 0, sizeof(*out));
    if (texture) {
        DWORD const *header = reinterpret_cast<DWORD const *>(texture);
        out->header = guest_address(texture);
        out->data = resource_data(header[1]);
        out->format = header[3];
        out->size = header[4];
    }
    if (palette)
        out->palette = resource_data(reinterpret_cast<DWORD const *>(palette)[1]);
    out->address_u = normal_address(stage[D3DTSS_ADDRESSU]);
    out->address_v = normal_address(stage[D3DTSS_ADDRESSV]);
    out->address_w = normal_address(stage[D3DTSS_ADDRESSW]);
    out->mag_filter = normal_filter(stage[D3DTSS_MAGFILTER]);
    out->min_filter = normal_filter(stage[D3DTSS_MINFILTER]);
    out->mip_filter = normal_filter(stage[D3DTSS_MIPFILTER]);
    out->border_color = stage[D3DTSS_BORDERCOLOR];
    out->lod_bias = float_bits(stage[D3DTSS_MIPMAPLODBIAS]);
    out->max_mip_level = stage[D3DTSS_MAXMIPLEVEL];
    out->color_key = stage[D3DTSS_COLORKEYCOLOR];
    out->color_key_op = stage[D3DTSS_COLORKEYOP];
}

/* the state every draw carries */
void fill_draw_state(h2_d3d_device *d, h2_gpu_draw *p)
{
    DWORD const *rs = D3D__RenderState;
    p->kind = H2_GPU_PACKET_DRAW;
    p->version = H2_GPU_PROTOCOL_VERSION;
    memcpy(p->vertex_defaults, g_vertex_registers, sizeof(p->vertex_defaults));

    p->vertex_mode = g_vertex_program_selected ? H2_GPU_VERTEX_PROGRAM : H2_GPU_VERTEX_SCREEN;
    p->vertex_program = guest_address(g_program_memory);
    p->vertex_program_length = k_program_slots;
    p->vertex_program_start = g_program_start;
    p->vertex_constants = guest_address(d->vertex_constants);

    p->has_pixel_shader = d->pixel_shader_program_set;
    if (d->pixel_shader_program_set)
        memcpy(p->pixel_shader, &d->pixel_shader_program, sizeof(p->pixel_shader));
    for (long stage = 0; stage < 4; ++stage) {
        fill_texture(&p->textures[stage], d->textures[stage], d->palettes[stage],
                     D3D__TextureState[stage]);
        p->stage_ops[stage].texcoord_index = D3D__TextureState[stage][D3DTSS_TEXCOORDINDEX];
    }

    p->blend_enable = rs[D3DRS_ALPHABLENDENABLE] != 0;
    p->blend_src_color = p->blend_src_alpha = normal_blend(rs[D3DRS_SRCBLEND]);
    p->blend_dst_color = p->blend_dst_alpha = normal_blend(rs[D3DRS_DESTBLEND]);
    p->blend_op_color = p->blend_op_alpha = normal_blend_op(rs[D3DRS_BLENDOP]);
    p->blend_constant = rs[D3DRS_BLENDCOLOR];
    p->color_write_mask = normal_color_mask(rs[D3DRS_COLORWRITEENABLE]);
    p->alpha_test_enable = rs[D3DRS_ALPHATESTENABLE] != 0;
    p->alpha_func = normal_compare(rs[D3DRS_ALPHAFUNC]);
    p->alpha_ref = static_cast<float>(rs[D3DRS_ALPHAREF] & 0xff) / 255.0f;
    p->depth_test_enable = rs[D3DRS_ZENABLE] != 0;
    p->depth_write_enable = rs[D3DRS_ZWRITEENABLE] != 0;
    p->depth_func = normal_compare(rs[D3DRS_ZFUNC]);
    p->depth_bias = static_cast<float>(rs[D3DRS_ZBIAS]) + float_bits(rs[D3DRS_POLYGONOFFSETZOFFSET]);
    p->depth_slope_scale = float_bits(rs[D3DRS_POLYGONOFFSETZSLOPESCALE]);
    p->stencil_enable = rs[D3DRS_STENCILENABLE] != 0;
    p->stencil_func = normal_compare(rs[D3DRS_STENCILFUNC]);
    p->stencil_ref = rs[D3DRS_STENCILREF];
    p->stencil_mask = rs[D3DRS_STENCILMASK];
    p->stencil_write_mask = rs[D3DRS_STENCILWRITEMASK];
    p->stencil_fail = normal_stencil_op(rs[D3DRS_STENCILFAIL]);
    p->stencil_depth_fail = normal_stencil_op(rs[D3DRS_STENCILZFAIL]);
    p->stencil_pass = normal_stencil_op(rs[D3DRS_STENCILPASS]);
    p->cull = rs[D3DRS_CULLMODE] == D3DCULL_CW ? H2_GPU_CULL_CLOCKWISE
            : rs[D3DRS_CULLMODE] == D3DCULL_CCW ? H2_GPU_CULL_COUNTER_CLOCKWISE
            : H2_GPU_CULL_NONE;
    p->front_face_clockwise = rs[D3DRS_FRONTFACE] == 0x900;   /* GL_CW */
    p->fill_wireframe = rs[D3DRS_FILLMODE] == D3DFILL_WIREFRAME;
    p->fog_enable = rs[D3DRS_FOGENABLE] != 0;
    p->fog_color = rs[D3DRS_FOGCOLOR];
    p->texture_factor = rs[D3DRS_TEXTUREFACTOR];

    p->viewport_x = static_cast<float>(d->viewport.X);
    p->viewport_y = static_cast<float>(d->viewport.Y);
    p->viewport_width = static_cast<float>(d->viewport.Width);
    p->viewport_height = static_cast<float>(d->viewport.Height);
    p->viewport_min_z = d->viewport.MinZ;
    p->viewport_max_z = d->viewport.MaxZ;
    p->scissor_enable = d->scissor_count != 0 && !d->scissor_exclusive;
    if (p->scissor_enable) {
        p->scissor_left = d->scissors[0].x1;
        p->scissor_top = d->scissors[0].y1;
        p->scissor_right = d->scissors[0].x2;
        p->scissor_bottom = d->scissors[0].y2;
    }
    p->render_target = d->render_target == d->back_buffers[0] ? 0 : guest_address(d->render_target);
    p->depth_stencil = guest_address(d->depth_stencil);
    p->target_width = d->parameters.BackBufferWidth;
    p->target_height = d->parameters.BackBufferHeight;
    p->two_sided_lighting = rs[D3DRS_TWOSIDEDLIGHTING] != 0;
    p->back_buffer_data = d->back_buffers[0] ? resource_data(d->back_buffers[0]->Data) : 0;
    p->front_buffer_data = d->back_buffers[1] ? resource_data(d->back_buffers[1]->Data) : 0;
}

/* The vertex attribute formats the game selects (SelectVertexShaderDirect,
   SetVertexShaderInputDirect): 16 inputs of {stream, offset, NV2A format,
   two flag bytes and a short}, as function_1c330 (src/unknown_01cf50.cpp)
   builds them, unused inputs having format 2. */
struct h2_d3d_attribute_input
{
    DWORD stream;
    DWORD offset;
    DWORD format;
    DWORD tessellation;
};

void fill_vertex_arrays(h2_d3d_device *d, h2_gpu_draw *p)
{
    h2_d3d_attribute_input const *inputs =
        static_cast<h2_d3d_attribute_input const *>(d->stream_formats ? d->stream_formats
                                                                     : d->vertex_attribute_formats);
    for (long index = 0; index < k_vertex_attribute_count; ++index) {
        h2_gpu_vertex_array *array = &p->vertex_arrays[index];
        array->address = 0;
        array->stride = 0;
        array->format = 0x02;   /* unused */
        if (!inputs)
            continue;
        h2_d3d_attribute_input const &input = inputs[index];
        DWORD const format = input.format & 0xff;
        if ((format >> 4) == 0 || input.stream >= static_cast<DWORD>(k_stream_count))
            continue;
        h2_d3d_stream const &stream = d->streams[input.stream];
        if (!stream.buffer)
            continue;
        DWORD const *buffer = reinterpret_cast<DWORD const *>(stream.buffer);
        array->address = resource_data(buffer[1]) + stream.offset + input.offset;
        array->stride = stream.stride;
        array->format = format;
    }
}

void h2_d3d_draw(h2_d3d_device *d, D3DPRIMITIVETYPE primitive, UINT start, UINT count,
                 WORD const *indices)
{
    push_submit(d);
    ++d->draw_calls;
    h2_gpu_draw *p = &g_draw_packet;
    fill_draw_state(d, p);
    p->primitive = primitive;
    p->first_vertex = start + (indices ? d->base_vertex : 0);
    p->vertex_count = count;
    p->index_data = guest_address(indices);
    p->vertex_source = H2_GPU_VERTICES_ARRAYS;
    fill_vertex_arrays(d, p);
    host_h2_gpu_draw(p);
}

/* a Begin/End batch, or an inline array the game pushed */
void h2_d3d_draw_inline(h2_d3d_device *d, DWORD primitive, void const *vertices, long count)
{
    if (!count)
        return;
    ++d->draw_calls;
    h2_gpu_draw *p = &g_draw_packet;
    fill_draw_state(d, p);
    p->primitive = primitive;
    p->first_vertex = 0;
    p->vertex_count = static_cast<h2_gpu_u32>(count);
    p->index_data = 0;
    p->vertex_source = H2_GPU_VERTICES_INLINE;
    p->inline_vertices = guest_address(vertices);
    p->inline_vertex_count = static_cast<h2_gpu_u32>(count);
    for (long index = 0; index < k_vertex_attribute_count; ++index)
        p->vertex_arrays[index].format = 0x02;
    host_h2_gpu_draw(p);
}

/* an inline array: interleaved vertices laid out as the selected attribute
   formats say, the stride being the end of the furthest attribute */
void h2_d3d_draw_pushed_array(DWORD primitive, DWORD const *data, DWORD words)
{
    h2_d3d_device *d = device();
    h2_d3d_attribute_input const *inputs =
        static_cast<h2_d3d_attribute_input const *>(d->vertex_attribute_formats);
    if (!inputs)
        return;
    static DWORD const element_bytes[16] = {1, 2, 4, 0, 1, 2, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    DWORD stride = 0;
    for (long index = 0; index < k_vertex_attribute_count; ++index) {
        DWORD const format = inputs[index].format & 0xff;
        DWORD const components = format >> 4;
        DWORD const type = format & 0xf;
        if (!components)
            continue;
        DWORD const size = type == 6 ? 4 : element_bytes[type] * components;
        if (inputs[index].offset + size > stride)
            stride = inputs[index].offset + size;
    }
    if (!stride)
        return;
    ++d->draw_calls;
    h2_gpu_draw *p = &g_draw_packet;
    fill_draw_state(d, p);
    p->primitive = primitive;
    p->first_vertex = 0;
    p->vertex_count = words * 4 / stride;
    p->index_data = 0;
    p->vertex_source = H2_GPU_VERTICES_ARRAYS;
    for (long index = 0; index < k_vertex_attribute_count; ++index) {
        h2_gpu_vertex_array *array = &p->vertex_arrays[index];
        DWORD const format = inputs[index].format & 0xff;
        array->address = (format >> 4) ? guest_address(data) + inputs[index].offset : 0;
        array->stride = stride;
        array->format = (format >> 4) ? format : 0x02;
    }
    host_h2_gpu_draw(p);
}

void h2_d3d_clear(h2_d3d_device *d, DWORD count, D3DRECT const *rects, DWORD flags,
                  D3DCOLOR color, float z, DWORD stencil)
{
    push_submit(d);
    ++d->clears;
    h2_gpu_clear *p = &g_clear_packet;
    p->kind = H2_GPU_PACKET_CLEAR;
    p->version = H2_GPU_PROTOCOL_VERSION;
    /* the Xbox's flags, as function_14b60 builds them: D3DCLEAR_ZBUFFER 1,
       _STENCIL 2 and a bit per target channel (0x10 red, 0x20 green, 0x40
       blue, 0x80 alpha; 0xf0 all). The game clears alpha alone (0x80,
       function_13c20) under the finished scene. */
    p->flags = ((flags & 0xf0) ? 1u : 0u) | (flags & 0xf0) | ((flags & 1) ? 2u : 0u) | ((flags & 2) ? 4u : 0u);
    p->color = color;
    p->depth = z;
    p->stencil = stencil;
    p->rect_count = rects && count ? (count > 8 ? 8 : count) : 0;
    for (DWORD index = 0; index < p->rect_count; ++index) {
        p->rects[index][0] = rects[index].x1;
        p->rects[index][1] = rects[index].y1;
        p->rects[index][2] = rects[index].x2;
        p->rects[index][3] = rects[index].y2;
    }
    if (!p->rect_count) {
        /* no rectangles: the viewport */
        p->rect_count = 1;
        p->rects[0][0] = static_cast<h2_gpu_s32>(d->viewport.X);
        p->rects[0][1] = static_cast<h2_gpu_s32>(d->viewport.Y);
        p->rects[0][2] = static_cast<h2_gpu_s32>(d->viewport.X + d->viewport.Width);
        p->rects[0][3] = static_cast<h2_gpu_s32>(d->viewport.Y + d->viewport.Height);
    }
    p->render_target = d->render_target == d->back_buffers[0] ? 0 : guest_address(d->render_target);
    p->depth_stencil = guest_address(d->depth_stencil);
    p->target_width = d->parameters.BackBufferWidth;
    p->target_height = d->parameters.BackBufferHeight;
    host_h2_gpu_clear(p);
}

/* registers written outside Begin/End stay the defaults of later draws;
   inside one, writing register 0 (the position) emits a vertex */
void vertex_register_set(DWORD vertex_register, float x, float y, float z, float w)
{
    if (vertex_register >= static_cast<DWORD>(k_vertex_attribute_count))
        return;
    float *value = g_vertex_registers[vertex_register];
    value[0] = x;
    value[1] = y;
    value[2] = z;
    value[3] = w;
    if (vertex_register == 0 && g_inline_active && g_inline_count < k_inline_capacity)
        memcpy(g_inline_vertices[g_inline_count++], g_vertex_registers, sizeof(g_vertex_registers));
}

/* lazily pushed state (D3D__DirtyFlags) would be encoded here */
void h2_d3d_flush_lazy_state(h2_d3d_device *d)
{
    (void)d;
    D3D__DirtyFlags = 0;
}

/* device lifetime */

void viewport_reset(h2_d3d_device *d)
{
    d->viewport.X = 0;
    d->viewport.Y = 0;
    d->viewport.Width = d->parameters.BackBufferWidth;
    d->viewport.Height = d->parameters.BackBufferHeight;
    d->viewport.MinZ = 0.0f;
    d->viewport.MaxZ = 1.0f;
}

void present_parameters_take(h2_d3d_device *d, D3DPRESENT_PARAMETERS const *parameters)
{
    if (parameters)
        d->parameters = *parameters;
    /* function_128c0 sizes the back buffer from a screen rectangle that may
       still be empty */
    if (!d->parameters.BackBufferWidth || d->parameters.BackBufferWidth > 4096)
        d->parameters.BackBufferWidth = 640;
    if (!d->parameters.BackBufferHeight || d->parameters.BackBufferHeight > 4096)
        d->parameters.BackBufferHeight = 480;
}

void surfaces_destroy(h2_d3d_device *d)
{
    surface_destroy(d->back_buffers[0]);
    surface_destroy(d->back_buffers[1]);
    surface_destroy(d->auto_depth_stencil);
    d->back_buffers[0] = d->back_buffers[1] = d->auto_depth_stencil = NULL;
    d->render_target = d->depth_stencil = NULL;
}

bool surfaces_create(h2_d3d_device *d)
{
    DWORD width = d->parameters.BackBufferWidth;
    DWORD height = d->parameters.BackBufferHeight;
    DWORD color = hardware_format(d->parameters.BackBufferFormat);
    if (!format_linear(color))
        color = k_hardware_linear_a8r8g8b8;
    d->back_buffers[0] = surface_create(width, height, color);
    d->back_buffers[1] = surface_create(width, height, color);
    if (d->parameters.EnableAutoDepthStencil) {
        DWORD depth = hardware_format(d->parameters.AutoDepthStencilFormat);
        if (!format_linear(depth))
            depth = k_hardware_linear_depth;
        d->auto_depth_stencil = surface_create(width, height, depth);
    }
    d->render_target = d->back_buffers[0];
    d->depth_stencil = d->auto_depth_stencil;
    return d->back_buffers[0] && d->back_buffers[1] &&
           (!d->parameters.EnableAutoDepthStencil || d->auto_depth_stencil);
}

void state_reset(h2_d3d_device *d)
{
    memset(D3D__RenderState, 0, sizeof(D3D__RenderState));
    memset(D3D__TextureState, 0, sizeof(D3D__TextureState));
    D3D__DirtyFlags = 0;
    memset(d->textures, 0, sizeof(d->textures));
    memset(d->palettes, 0, sizeof(d->palettes));
    memset(d->vertex_constants, 0, sizeof(d->vertex_constants));
    memset(d->streams, 0, sizeof(d->streams));
    memset(d->vertex_data, 0, sizeof(d->vertex_data));
    d->stream_count = 0;
    d->stream_formats = NULL;
    d->vertex_shader = 0;
    d->vertex_program = NULL;
    d->vertex_attribute_formats = NULL;
    d->indices = NULL;
    d->base_vertex = 0;
    d->pixel_shader = 0;
    d->pixel_shader_program_set = false;
    d->scissor_count = 0;
    memset(d->stipple, 0xff, sizeof(d->stipple));
    d->clip_near = 0.0f;
    d->clip_far = 1.0f;
    d->back_buffer_scale_x = d->back_buffer_scale_y = 1.0f;
    for (DWORD i = 0; i < 256; ++i)
        d->gamma_ramp.red[i] = d->gamma_ramp.green[i] = d->gamma_ramp.blue[i] =
            static_cast<WORD>(i * 0x101);
    viewport_reset(d);
}

} // namespace

/* Direct3D */

extern "C" void __stdcall Direct3D_SetPushBufferSize(DWORD push_buffer_size, DWORD kickoff_size)
{
    D3D__CommandBufferSize = push_buffer_size;
    D3D__SegmentSize = kickoff_size;
    D3D::CDevice::m_PushSegmentSize = kickoff_size ? kickoff_size : 0x8000;
}

extern "C" HRESULT __stdcall Direct3D_CreateDevice(UINT adapter, D3DDEVTYPE type,
                                                   void *focus_window, DWORD behavior,
                                                   D3DPRESENT_PARAMETERS *parameters,
                                                   D3DDevice **out)
{
    (void)adapter;
    (void)type;
    (void)focus_window;
    (void)behavior;
    h2_d3d_device *d = device();
    if (d->created) {
        ++d->reference_count;
        if (out)
            *out = D3D__pDevice;
        return S_OK;
    }
    memset(d, 0, sizeof(*d));
    present_parameters_take(d, parameters);
    if (!push_allocate(d, D3D__CommandBufferSize) || !surfaces_create(d)) {
        surfaces_destroy(d);
        storage_free(reinterpret_cast<DWORD>(d->push_base));
        memset(d, 0, sizeof(*d));
        if (out)
            *out = NULL;
        return E_OUTOFMEMORY;
    }
    state_reset(d);
    d->created = true;
    d->reference_count = 1;
    D3D__pDevice = reinterpret_cast<D3DDevice *>(d);
    if (out)
        *out = D3D__pDevice;
    return S_OK;
}

/* the device */

extern "C" ULONG __stdcall D3DDevice_Release(void)
{
    h2_d3d_device *d = device();
    if (!d->created)
        return 0;
    if (--d->reference_count)
        return d->reference_count;
    push_submit(d);
    surfaces_destroy(d);
    storage_free(reinterpret_cast<DWORD>(d->push_base));
    d->push_base = d->push_end = d->push_put = d->push_threshold = d->push_consumed = NULL;
    d->created = false;
    D3D__pDevice = NULL;
    return 0;
}

extern "C" void __stdcall D3DDevice_Reset(D3DPRESENT_PARAMETERS *parameters)
{
    h2_d3d_device *d = device();
    if (!d->created)
        return;
    push_submit(d);
    DWORD width = d->parameters.BackBufferWidth;
    DWORD height = d->parameters.BackBufferHeight;
    present_parameters_take(d, parameters);
    if (width != d->parameters.BackBufferWidth || height != d->parameters.BackBufferHeight) {
        /* keep the headers the game already holds; only their storage changes */
        D3DSurface *surfaces[3] = {d->back_buffers[0], d->back_buffers[1], d->auto_depth_stencil};
        for (D3DSurface *surface : surfaces) {
            h2_d3d_resource_slot *slot = slot_of(surface);
            if (!slot)
                continue;
            DWORD common = surface->Common;
            DWORD code = (surface->Format >> 8) & 0xff;
            storage_free(slot->storage);
            slot->storage = 0;
            pixel_container_create(slot, k_common_surface, d->parameters.BackBufferWidth,
                                   d->parameters.BackBufferHeight, 1, 1, code, false, false);
            surface->Common = common;
        }
    }
    d->render_target = d->back_buffers[0];
    d->depth_stencil = d->auto_depth_stencil;
    viewport_reset(d);
}

extern "C" void __stdcall D3DDevice_GetBackBuffer(INT index, D3DBACKBUFFER_TYPE type,
                                                  D3DSurface **surface)
{
    (void)type;
    h2_d3d_device *d = device();
    /* index -1 is the front buffer (function_14600, src/unknown_01cf50.cpp) */
    D3DSurface *result = index < 0 ? d->back_buffers[1] : d->back_buffers[0];
    add_reference(result);
    if (surface)
        *surface = result;
}

extern "C" D3DSurface *__stdcall D3DDevice_GetBackBuffer2(INT index)
{
    D3DSurface *surface;
    D3DDevice_GetBackBuffer(index, D3DBACKBUFFER_TYPE_MONO, &surface);
    return surface;
}

extern "C" void __stdcall D3DDevice_GetDepthStencilSurface(D3DSurface **surface)
{
    h2_d3d_device *d = device();
    add_reference(d->depth_stencil);
    if (surface)
        *surface = d->depth_stencil;
}

extern "C" void __stdcall D3DDevice_GetRenderTarget(D3DSurface **target)
{
    h2_d3d_device *d = device();
    add_reference(d->render_target);
    if (target)
        *target = d->render_target;
}

/* The state cache keeps weak references: the game tests Common itself to
   learn whether its texture headers are built (src/unknown_12c0d0.cpp), so
   binding must not change it. */
extern "C" void __stdcall D3DDevice_SetRenderTarget(D3DSurface *target, D3DSurface *depth)
{
    h2_d3d_device *d = device();
    push_submit(d);
    if (target)
        d->render_target = target;
    d->depth_stencil = depth;
}

extern "C" void __stdcall D3DDevice_SetRenderTargetFast(D3DSurface *target, D3DSurface *depth,
                                                        DWORD flags)
{
    (void)flags;
    D3DDevice_SetRenderTarget(target, depth);
}

extern "C" void __stdcall D3DDevice_Clear(DWORD count, D3DRECT const *rects, DWORD flags,
                                          D3DCOLOR color, float z, DWORD stencil)
{
    h2_d3d_clear(device(), count, rects, flags, color, z, stencil);
}

extern "C" void __stdcall D3DDevice_SetViewport(D3DVIEWPORT8 const *viewport)
{
    if (viewport)
        device()->viewport = *viewport;
}

extern "C" void __stdcall D3DDevice_GetViewport(D3DVIEWPORT8 *viewport)
{
    if (viewport)
        *viewport = device()->viewport;
}

extern "C" void __stdcall D3DDevice_SetScissors(DWORD count, BOOL exclusive, D3DRECT const *rects)
{
    h2_d3d_device *d = device();
    if (!rects || count > k_scissor_count)
        count = rects ? k_scissor_count : 0;
    d->scissor_count = count;
    d->scissor_exclusive = exclusive;
    if (count)
        memcpy(d->scissors, rects, count * sizeof(D3DRECT));
}

/* a 32x32 bit pattern (function_1c1b0, src/unknown_01cf50.cpp) */
extern "C" void __stdcall D3DDevice_SetStipple(DWORD const *pattern)
{
    if (pattern)
        memcpy(device()->stipple, pattern, sizeof(device()->stipple));
}

extern "C" void __stdcall D3DDevice_SetDepthClipPlanes(float near_plane, float far_plane,
                                                       DWORD flags)
{
    h2_d3d_device *d = device();
    d->clip_near = near_plane;
    d->clip_far = far_plane;
    d->clip_flags = flags;
}

extern "C" void __stdcall D3DDevice_SetBackBufferScale(float x, float y)
{
    device()->back_buffer_scale_x = x;
    device()->back_buffer_scale_y = y;
}

extern "C" void __stdcall D3DDevice_SetFlickerFilter(DWORD filter)
{
    device()->flicker_filter = filter;
}

extern "C" void __stdcall D3DDevice_SetSoftDisplayFilter(BOOL enable)
{
    device()->soft_display_filter = enable;
}

extern "C" void __stdcall D3DDevice_SetGammaRamp(DWORD flags, D3DGAMMARAMP const *ramp)
{
    (void)flags;
    if (ramp)
        device()->gamma_ramp = *ramp;
}

/* render and texture-stage state */

extern "C" void __stdcall D3DDevice_SetRenderState(DWORD state, DWORD value)
{
    if (state < D3DRS_MAX)
        D3D__RenderState[state] = value;
}

extern "C" void __stdcall D3DDevice_SetRenderStateNotInline(long state, DWORD value)
{
    D3DDevice_SetRenderState(static_cast<DWORD>(state), value);
}

extern "C" void __stdcall D3DDevice_SetRenderState_Simple(DWORD method, DWORD value)
{
    /* D3DSIMPLERENDERSTATEENCODE above is the identity */
    D3DDevice_SetRenderState(method, value);
}

extern "C" void __stdcall D3DDevice_SetRenderState_ZEnable(DWORD value)
{
    D3D__RenderState[D3DRS_ZENABLE] = value;
}

extern "C" void __stdcall D3DDevice_SetRenderState_ZBias(DWORD value)
{
    D3D__RenderState[D3DRS_ZBIAS] = value;
}

extern "C" void __stdcall D3DDevice_GetRenderState(DWORD state, DWORD *value)
{
    if (value)
        *value = state < D3DRS_MAX ? D3D__RenderState[state] : 0;
}

extern "C" void __stdcall D3DDevice_SetTextureStageState(DWORD stage, DWORD type, DWORD value)
{
    if (stage >= 4 || type >= D3DTSS_MAX)
        return;
    D3D__TextureState[stage][type] = value;
    D3D__DirtyFlags |= D3DDIRTYFLAG_TEXTURE_STATE_0 << stage;
}

extern "C" void __stdcall D3DDevice_GetTextureStageState(DWORD stage, DWORD type, DWORD *value)
{
    if (value)
        *value = stage < 4 && type < D3DTSS_MAX ? D3D__TextureState[stage][type] : 0;
}

extern "C" void __stdcall D3DDevice_SetTexture(DWORD stage, void *texture)
{
    if (stage < 4) {
        device()->textures[stage] = static_cast<D3DBaseTexture *>(texture);
        D3D__DirtyFlags |= D3DDIRTYFLAG_TEXTURE_STATE_0 << stage;
    }
}

extern "C" void __stdcall D3DDevice_SetPalette(DWORD stage, D3DPalette *palette)
{
    if (stage < 4)
        device()->palettes[stage] = palette;
}

/* shaders */

/* D3DFVF_XYZRHW (the public value): already transformed positions */
constexpr DWORD k_fvf_xyzrhw = 0x004;

extern "C" void __stdcall D3DDevice_SetVertexShader(DWORD handle)
{
    device()->vertex_shader = handle;
    if (handle & k_fvf_xyzrhw)
        g_vertex_program_selected = false;
}

/* A program is a header word (instruction count << 16 | 0x2078, as the
   game's own g_43f2b0 is) followed by 4 words per instruction; it goes into
   program memory at instruction `address`. */
extern "C" void __stdcall D3DDevice_LoadVertexShaderProgram(DWORD const *program, DWORD address)
{
    device()->vertex_program = program;
    device()->vertex_program_address = address;
    if (!program)
        return;
    DWORD count = program[0] >> 16;
    if (address >= static_cast<DWORD>(k_program_slots))
        return;
    if (count > k_program_slots - address)
        count = k_program_slots - address;
    memcpy(g_program_memory[address], program + 1, count * 16);
}

extern "C" void __stdcall D3DDevice_SelectVertexShaderDirect(void const *formats, long shader)
{
    device()->vertex_attribute_formats = formats;
    device()->vertex_shader_select_address = static_cast<DWORD>(shader);
    g_program_start = static_cast<DWORD>(shader) < static_cast<DWORD>(k_program_slots)
                          ? static_cast<DWORD>(shader) : 0;
    g_vertex_program_selected = true;
}

extern "C" void __stdcall D3DDevice_SetVertexShaderInputDirect(void const *formats,
                                                               long stream_count,
                                                               void const *streams)
{
    h2_d3d_device *d = device();
    if (stream_count < 0 || !streams)
        stream_count = 0;
    if (stream_count > k_stream_count)
        stream_count = k_stream_count;
    d->stream_formats = formats;
    d->stream_count = stream_count;
    memset(d->streams, 0, sizeof(d->streams));
    if (stream_count)
        memcpy(d->streams, streams, stream_count * sizeof(h2_d3d_stream));
}

/* registers -96..95 (the recovered callers pass both signs) */
extern "C" void __stdcall D3DDevice_SetVertexShaderConstant(int register_index,
                                                            void const *constant_data,
                                                            DWORD constant_count)
{
    if (!constant_data)
        return;
    long first = register_index + k_vertex_constant_bias;
    long count = static_cast<long>(constant_count);
    float const *source = static_cast<float const *>(constant_data);
    if (first < 0) {
        source -= first * 4;
        count += first;
        first = 0;
    }
    if (first + count > k_vertex_constant_count)
        count = k_vertex_constant_count - first;
    if (count > 0)
        memcpy(device()->vertex_constants[first], source, count * 16);
}

extern "C" void __stdcall D3DDevice_SetVertexShaderConstantFast(int register_index,
                                                                void const *constant_data,
                                                                DWORD constant_count)
{
    D3DDevice_SetVertexShaderConstant(register_index, constant_data, constant_count);
}

extern "C" void __stdcall D3DDevice_SetVertexShaderConstantNotInline(int register_index,
                                                                     void const *constant_data,
                                                                     DWORD constant_count)
{
    D3DDevice_SetVertexShaderConstant(register_index, constant_data, constant_count);
}

extern "C" void __stdcall D3DDevice_SetPixelShader(DWORD handle)
{
    device()->pixel_shader = handle;
    if (!handle)
        device()->pixel_shader_program_set = false;
}

extern "C" void __stdcall D3DDevice_SetPixelShaderProgram(void const *program)
{
    h2_d3d_device *d = device();
    d->pixel_shader_program_set = program != NULL;
    if (program)
        memcpy(&d->pixel_shader_program, program, sizeof(d->pixel_shader_program));
}

/* vertex data and drawing */

extern "C" void __stdcall D3DDevice_SetVertexData2s(DWORD vertex_register, SHORT a, SHORT b)
{
    vertex_register_set(vertex_register, a, b, 0.0f, 1.0f);
}

extern "C" void __stdcall D3DDevice_SetVertexData4ub(DWORD vertex_register, BYTE a, BYTE b,
                                                     BYTE c, BYTE d)
{
    vertex_register_set(vertex_register, a / 255.0f, b / 255.0f, c / 255.0f, d / 255.0f);
}

extern "C" void __stdcall D3DDevice_SetVertexData2f(int vertex_register, float x, float y)
{
    vertex_register_set(static_cast<DWORD>(vertex_register), x, y, 0.0f, 1.0f);
}

extern "C" void __stdcall D3DDevice_SetVertexData4f(DWORD vertex_register, float x, float y,
                                                    float z, float w)
{
    vertex_register_set(vertex_register, x, y, z, w);
}

extern "C" void __stdcall D3DDevice_SetVertexData4s(DWORD vertex_register, SHORT a, SHORT b,
                                                    SHORT c, SHORT d)
{
    vertex_register_set(vertex_register, a, b, c, d);
}

/* a D3DCOLOR (ARGB) as the (red, green, blue, alpha) a D3D colour input reads */
extern "C" void __stdcall D3DDevice_SetVertexDataColor(DWORD vertex_register, D3DCOLOR color)
{
    vertex_register_set(vertex_register, ((color >> 16) & 0xff) / 255.0f,
                        ((color >> 8) & 0xff) / 255.0f, (color & 0xff) / 255.0f,
                        (color >> 24) / 255.0f);
}

extern "C" void __stdcall D3DDevice_Begin(DWORD primitive_type)
{
    push_submit(device());
    g_inline_primitive = primitive_type;
    g_inline_count = 0;
    g_inline_active = true;
}

extern "C" void __stdcall D3DDevice_End(void)
{
    if (!g_inline_active)
        return;
    g_inline_active = false;
    h2_d3d_draw_inline(device(), g_inline_primitive, g_inline_vertices, g_inline_count);
}

extern "C" void __stdcall D3DDevice_SetIndices(D3DIndexBuffer *indices, UINT base_vertex)
{
    device()->indices = indices;
    device()->base_vertex = base_vertex;
}

extern "C" void __stdcall D3DDevice_DrawVertices(D3DPRIMITIVETYPE primitive_type, UINT start,
                                                 UINT count)
{
    h2_d3d_draw(device(), primitive_type, start, count, NULL);
}

extern "C" void __stdcall D3DDevice_DrawIndexedVertices(DWORD primitive_type, DWORD count,
                                                        WORD const *indices)
{
    h2_d3d_draw(device(), primitive_type, 0, count, indices);
}

/* push buffer */

DWORD volatile *WINAPI D3D::MakeRequestedSpace(DWORD fallback_size, DWORD requested_size)
{
    (void)fallback_size;
    h2_d3d_device *d = device();
    DWORD *push = push_reserve(d, (requested_size + 3) / 4);
    if (!push) {
        /* without a device there is nowhere to write: the caller would fault
           on NULL anyway, so say why first */
        log_message("h2-port: d3d push space of 0x%x bytes with no push buffer", requested_size);
    }
    return push;
}

extern "C" DWORD *__stdcall D3DDevice_MakeSpace(void)
{
    DWORD segment = D3D::CDevice::m_PushSegmentSize;
    return const_cast<DWORD *>(D3D::MakeRequestedSpace(segment >> 1, segment));
}

extern "C" DWORD *__stdcall D3DDevice_BeginPush(DWORD dword_count)
{
    return push_reserve(device(), dword_count + 1);
}

extern "C" void __stdcall D3DDevice_EndPush(DWORD *push)
{
    h2_d3d_device *d = device();
    if (push < d->push_put || push > d->push_end) {
        log_message("h2-port: d3d EndPush at 0x%x outside 0x%x-0x%x",
                    reinterpret_cast<DWORD>(push), reinterpret_cast<DWORD>(d->push_put),
                    reinterpret_cast<DWORD>(d->push_end));
        return;
    }
    d->push_put = push;
    /* what the game pushed draws with the state it set before pushing: parsed
       later, a draw's texture could already be the next caller's (the
       interface sprites of function_52040 drew with the glyph cache) */
    push_submit(d);
}

void D3D::CDevice::LazySetStateVB(DWORD value)
{
    (void)value;
    h2_d3d_flush_lazy_state(reinterpret_cast<h2_d3d_device *>(this));
}

DWORD WINAPI D3D::SetFence(DWORD flags)
{
    (void)flags;
    h2_d3d_device *d = device();
    push_submit(d);
    return ++d->fence;
}

extern "C" void __stdcall D3DDevice_KickPushBuffer(void)
{
    push_submit(device());
}

/* Nothing runs behind the game, so the device is never busy and every fence
   and callback has been reached by the time it is asked about. */
extern "C" BOOL __stdcall D3DDevice_IsBusy(void)
{
    push_submit(device());
    return FALSE;
}

extern "C" void __stdcall D3DDevice_FlushVertexCache(void)
{
}

extern "C" void __stdcall D3DDevice_InsertCallback(D3DCALLBACKTYPE type, D3DCALLBACK callback,
                                                   DWORD context)
{
    (void)type;
    push_submit(device());
    if (callback)
        callback(context);
}

/* PROVISIONAL: with no rasterizer every test reports zero samples */
extern "C" HRESULT __stdcall D3DDevice_GetVisibilityTestResult(DWORD index, UINT *result,
                                                               ULONGLONG *timestamp)
{
    h2_d3d_device *d = device();
    if (result)
        *result = index < k_visibility_count ? d->visibility[index] : 0;
    if (timestamp)
        *timestamp = d->fence;
    return S_OK;
}

extern "C" HRESULT __stdcall D3DDevice_EndVisibilityTest(DWORD index)
{
    h2_d3d_device *d = device();
    push_submit(d);
    if (index < k_visibility_count)
        d->visibility[index] = 0;
    return S_OK;
}

/* presentation */

/* The vertical blank is the display's 60 Hz interrupt, not something a
   swap causes: the game waits on the count its callback keeps
   (main_time_wait_for_vblank, src/unknown_12b070.cpp) also while it does
   not present. A guest thread stands in for the interrupt. */
DWORD WINAPI vblank_thread(void *)
{
    h2_d3d_device *d = device();
    unsigned long long next = host_h2_get_milliseconds() * 3ull;   /* thirds of a millisecond */
    for (;;) {
        next += 50;   /* 16.67 ms */
        while (host_h2_get_milliseconds() * 3ull < next)
            host_h2_sleep(1);
        ++d->vblank_count;
        if (d->vblank_callback) {
            h2_d3d_vblank_data data = {d->vblank_count, d->presented_frames, 0};
            d->vblank_callback(reinterpret_cast<D3DVBLANKDATA *>(&data));
        }
    }
    return 0;
}

extern "C" void __stdcall D3DDevice_SetVerticalBlankCallback(D3DVBLANKCALLBACK callback)
{
    static bool started;
    device()->vblank_callback = callback;
    if (!started) {
        started = true;
        CreateThread(NULL, 0x10000, vblank_thread, NULL, 0, NULL);
    }
}

extern "C" void __stdcall D3DDevice_Swap(DWORD flags)
{
    h2_d3d_device *d = device();
    push_submit(d);
    /* the headers the game holds keep their roles; their storage flips */
    if (d->back_buffers[0] && d->back_buffers[1]) {
        DWORD data = d->back_buffers[0]->Data;
        d->back_buffers[0]->Data = d->back_buffers[1]->Data;
        d->back_buffers[1]->Data = data;
    }
    ++d->presented_frames;
    if (host_h2_trace_flags() & 1)
    {
        static DWORD last_draws, last_clears, last_dwords;
        if (d->presented_frames <= 4 || d->presented_frames % 120 == 0)
        {
            log_message("h2-d3d: frame %u draws %u clears %u", d->presented_frames,
                        d->draw_calls - last_draws, d->clears - last_clears);
            log_message("h2-d3d:   push dwords %u", d->push.dwords - last_dwords);
            for (DWORD method = 0; method < 0x800; ++method)
            {
                if (g_trace_method_counts[method])
                    log_message("h2-d3d:   method 0x%04x x%u", method * 4, g_trace_method_counts[method]);
                g_trace_method_counts[method] = 0;
            }
        }
        last_draws = d->draw_calls;
        last_clears = d->clears;
        last_dwords = d->push.dwords;
    }
    (void)flags;
    host_h2_frame();
}

/* keeps the last frame on screen across a title launch (function_22376b) */
extern "C" void __stdcall D3DDevice_PersistDisplay(void)
{
    push_submit(device());
}

/* resources */

extern "C" D3DTexture *__stdcall D3DDevice_CreateTexture2(UINT width, UINT height, UINT depth,
                                                          UINT levels, DWORD usage,
                                                          D3DFORMAT format, D3DRESOURCETYPE type)
{
    (void)usage;
    h2_d3d_resource_slot *slot = slot_allocate();
    if (!slot)
        return NULL;
    bool cube = type == D3DRTYPE_CUBETEXTURE;
    bool volume = type == D3DRTYPE_VOLUMETEXTURE;
    DWORD code = static_cast<DWORD>(format) & 0xff;
    if (!pixel_container_create(slot, k_common_texture, width, cube ? width : height,
                                volume ? depth : 1, levels, code, cube, volume)) {
        slot_destroy(slot);
        return NULL;
    }
    return reinterpret_cast<D3DTexture *>(slot->header);
}

extern "C" D3DPalette *__stdcall D3DDevice_CreatePalette2(D3DPALETTESIZE size)
{
    h2_d3d_resource_slot *slot = slot_allocate();
    if (!slot)
        return NULL;
    /* PROVISIONAL: D3DPALETTESIZE n holds 256 >> n entries (d3d8.h) */
    DWORD entries = 256u >> (static_cast<DWORD>(size) & 3);
    slot->storage = storage_allocate(entries * sizeof(D3DCOLOR));
    if (!slot->storage) {
        slot_destroy(slot);
        return NULL;
    }
    D3DPalette *palette = reinterpret_cast<D3DPalette *>(slot->header);
    palette->Common = k_common_palette | 1;
    palette->Data = physical_of(slot->storage);
    palette->Lock = 0;
    return palette;
}

extern "C" DWORD __stdcall D3DResource_Release(D3DResource *resource)
{
    return release_reference(resource);
}

extern "C" BOOL __stdcall D3DResource_IsBusy(D3DResource *resource)
{
    (void)resource;
    return FALSE;
}

extern "C" void __stdcall D3DResource_BlockUntilNotBusy(D3DResource *resource)
{
    (void)resource;
}

/* rebases a header the game built with offsets onto its storage
   (function_1df20, src/unknown_01dcc0.cpp) */
extern "C" void __stdcall D3DResource_Register(D3DResource *resource, void *base)
{
    if (resource)
        resource->Data += physical_of(reinterpret_cast<DWORD>(base));
}

extern "C" void __stdcall D3DSurface_GetDesc(D3DSurface *surface, D3DSURFACE_DESC *desc)
{
    if (!desc)
        return;
    memset(desc, 0, sizeof(*desc));
    if (!surface)
        return;
    h2_d3d_layout layout = layout_of(surface);
    h2_d3d_level level = level_of(layout, 0);
    desc->Format = static_cast<D3DFORMAT>(layout.code);
    desc->Type = D3DRTYPE_SURFACE;
    desc->Size = level.slice;
    desc->Width = layout.width;
    desc->Height = layout.height;
}

extern "C" void __stdcall D3DSurface_LockRect(D3DSurface *surface, D3DLOCKED_RECT *locked,
                                              void const *rect, DWORD flags)
{
    (void)flags;
    if (!locked)
        return;
    locked->Pitch = 0;
    locked->pBits = NULL;
    if (!surface)
        return;
    h2_d3d_level level;
    BYTE *bits = level_pointer(surface, 0, 0, &level);
    h2_d3d_layout layout = layout_of(surface);
    if (bits && rect && !format_compressed(layout.code)) {
        h2_d3d_lock_rect const *area = static_cast<h2_d3d_lock_rect const *>(rect);
        bits += area->top * level.pitch + area->left * (format_bits(layout.code) / 8);
    }
    locked->Pitch = static_cast<INT>(level.pitch);
    locked->pBits = bits;
}

extern "C" void __stdcall D3DSurface_UnlockRect(D3DSurface *surface)
{
    (void)surface;
}

/* a surface header over one level of a texture; it holds a reference on the
   texture until released */
extern "C" D3DSurface *__stdcall D3DTexture_GetSurfaceLevel2(D3DTexture *texture, UINT level)
{
    if (!texture)
        return NULL;
    h2_d3d_resource_slot *slot = slot_allocate();
    if (!slot)
        return NULL;
    h2_d3d_layout layout = layout_of(texture);
    h2_d3d_level found;
    BYTE *bits = level_pointer(texture, 0, level, &found);
    D3DSurface *surface = reinterpret_cast<D3DSurface *>(slot->header);
    surface->Common = k_common_surface | 1;
    surface->Data = bits ? physical_of(reinterpret_cast<DWORD>(bits)) : 0;
    surface->Lock = 0;
    if (layout.linear) {
        surface->Format = (texture->Format & ~0x000f0000) | (1 << 16);
        surface->Size = texture->Size;
    } else {
        surface->Format = (texture->Format & 0x0000fff0) | k_format_base | (1 << 16) |
                          (log2_ceiling(found.width) << 20) | (log2_ceiling(found.height) << 24);
        surface->Size = 0;
    }
    surface->Parent = texture;
    slot->parent = texture;
    add_reference(texture);
    return surface;
}

extern "C" void __stdcall D3DTexture_LockRect(D3DTexture *texture, UINT level,
                                              D3DLOCKED_RECT *locked, void const *rect,
                                              DWORD flags)
{
    (void)flags;
    if (!locked)
        return;
    h2_d3d_level found = {};
    BYTE *bits = texture ? level_pointer(texture, 0, level, &found) : NULL;
    if (bits && rect && texture->Size) {
        h2_d3d_lock_rect const *area = static_cast<h2_d3d_lock_rect const *>(rect);
        bits += area->top * found.pitch +
                area->left * (format_bits((texture->Format >> 8) & 0xff) / 8);
    }
    locked->Pitch = static_cast<INT>(found.pitch);
    locked->pBits = bits;
}

extern "C" void __stdcall D3DTexture_UnlockRect(D3DTexture *texture, UINT level)
{
    (void)texture;
    (void)level;
}

extern "C" void __stdcall D3DCubeTexture_LockRect(D3DCubeTexture *texture, D3DCUBEMAP_FACES face,
                                                  UINT level, D3DLOCKED_RECT *locked,
                                                  void const *rect, DWORD flags)
{
    (void)rect;
    (void)flags;
    if (!locked)
        return;
    h2_d3d_level found = {};
    BYTE *bits = texture ? level_pointer(texture, static_cast<DWORD>(face), level, &found) : NULL;
    locked->Pitch = static_cast<INT>(found.pitch);
    locked->pBits = bits;
}

extern "C" void __stdcall D3DCubeTexture_UnlockRect(D3DCubeTexture *texture,
                                                    D3DCUBEMAP_FACES face, UINT level)
{
    (void)texture;
    (void)face;
    (void)level;
}

extern "C" void __stdcall D3DVolumeTexture_LockBox(D3DVolumeTexture *texture, UINT level,
                                                   D3DLOCKED_BOX *locked, D3DBOX const *box,
                                                   DWORD flags)
{
    (void)box;
    (void)flags;
    if (!locked)
        return;
    h2_d3d_level found = {};
    BYTE *bits = texture ? level_pointer(texture, 0, level, &found) : NULL;
    locked->RowPitch = static_cast<INT>(found.pitch);
    locked->SlicePitch = static_cast<INT>(found.slice);
    locked->pBits = bits;
}

extern "C" void __stdcall D3DVolumeTexture_UnlockBox(D3DVolumeTexture *texture, UINT level)
{
    (void)texture;
    (void)level;
}

extern "C" D3DCOLOR *__stdcall D3DPalette_Lock2(D3DPalette *palette, DWORD flags)
{
    (void)flags;
    return palette ? reinterpret_cast<D3DCOLOR *>(pointer_of(palette->Data)) : NULL;
}

extern "C" void __stdcall D3DPalette_Unlock(D3DPalette *palette)
{
    (void)palette;
}

/* the storage of one level of any pixel container, the game's own headers
   included (function_24970 and function_24ee0, src/unknown_023540.cpp) */
void __stdcall D3D::PixelJar::FindSurfaceWithinTexture(D3DPixelContainer *container,
                                                       D3DCUBEMAP_FACES face, unsigned int level,
                                                       BYTE **bits, DWORD *pitch, DWORD *width,
                                                       DWORD *height, DWORD *slice)
{
    h2_d3d_level found = {};
    BYTE *result = container ? level_pointer(container, static_cast<DWORD>(face), level, &found)
                             : NULL;
    if (bits) *bits = result;
    if (pitch) *pitch = found.pitch;
    if (width) *width = found.width;
    if (height) *height = found.height;
    if (slice) *slice = found.slice;
}

/* for tests and the host: how many frames the game has presented */
extern "C" DWORD h2_d3d_presented_frame_count(void)
{
    return device()->presented_frames;
}
