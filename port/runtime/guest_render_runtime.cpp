#include "h2/host_abi.h"
#include "h2/render_protocol.h"

union point3f {
    float n[3];
};
struct color4f { float alpha, red, green, blue; };
struct short_rectangle2d { short top, left, bottom, right; };

struct s_interface_draw_request_native {
    unsigned int flags;
    unsigned char unknown04[8];
    unsigned int bitmap_address;
};
static_assert(sizeof(s_interface_draw_request_native) == 16,
              "recovered interface request prefix layout");

struct s_interface_draw_vertex_native {
    float position_x;
    float position_y;
    float texture_x;
    float texture_y;
    unsigned int color;
};
static_assert(sizeof(s_interface_draw_vertex_native) == 20,
              "recovered interface vertex layout");

// @retail 0x52040, native renderer boundary
void function_52040(void const *source_request, void const *source_vertices)
{
    const auto *interface_request =
        static_cast<const s_interface_draw_request_native *>(source_request);
    const auto *source =
        static_cast<const s_interface_draw_vertex_native *>(source_vertices);
    if (!source)
        return;
    h2_render_immediate_request request = {};
    request.kind = H2_RENDER_REQUEST_INTERFACE_QUAD;
    request.version = H2_RENDER_PROTOCOL_VERSION;
    request.primitive_type = 7;
    request.vertex_count = 4;
    request.color = source[0].color;
    request.texture_tag = interface_request
        ? static_cast<int>(interface_request->bitmap_address) : -1;
    request.texture_index = -1;
    request.layer = -1;
    h2_render_immediate_vertex vertices[4] = {};
    for (long i = 0; i < 4; ++i) {
        vertices[i].texture_x = source[i].texture_x;
        vertices[i].texture_y = source[i].texture_y;
        vertices[i].x = source[i].position_x;
        vertices[i].y = source[i].position_y;
        vertices[i].w = 1.0f;
    }
    host_h2_render_request(&request, vertices);
}

/* Native menu rendering does not use the Xbox cache-prediction system yet.
 * The recovered callers use this as a best-effort preload hint, so fail
 * conservatively without fabricating loaded texture, geometry, or audio state.
 * Actual resource ownership remains at the explicit host rendering boundary. */
struct s_predicted_resource_block;

bool function_bacc0(long, long, point3f const *)
{
    /* Menu-model marker state has no native model renderer yet. */
    return false;
}

/* The native immediate protocol carries all per-draw state explicitly. */
void function_1bbd0()
{
}

/* The Xbox loading-screen image buffer has no native owner yet. */
void function_2133a1()
{
}

void function_213484()
{
}

bool function_16e5e0(s_predicted_resource_block const *block, short mode)
{
    (void)block;
    (void)mode;
    return false;
}

namespace {

struct s_immediate_state {
    h2_render_immediate_request request;
    h2_render_immediate_vertex vertices[H2_RENDER_IMMEDIATE_MAX_VERTICES];
    float texture_x;
    float texture_y;
    bool active;
};

s_immediate_state g_immediate;

}  // namespace

extern "C" void D3DDevice_Begin(unsigned int primitive_type)
{
    g_immediate.request.kind = H2_RENDER_REQUEST_IMMEDIATE;
    g_immediate.request.version = H2_RENDER_PROTOCOL_VERSION;
    g_immediate.request.primitive_type = primitive_type;
    g_immediate.request.vertex_count = 0;
    g_immediate.request.color = 0xffffffffu;
    g_immediate.request.texture_tag = -1;
    g_immediate.request.texture_index = -1;
    g_immediate.request.layer = -1;
    g_immediate.texture_x = 0.0f;
    g_immediate.texture_y = 0.0f;
    g_immediate.active = true;
}

extern "C" void D3DDevice_SetVertexDataColor(unsigned int, unsigned int color)
{
    if (g_immediate.active)
        g_immediate.request.color = color;
}

extern "C" void D3DDevice_SetVertexData2f(unsigned int, float x, float y)
{
    if (!g_immediate.active)
        return;
    g_immediate.texture_x = x;
    g_immediate.texture_y = y;
}

extern "C" void D3DDevice_SetVertexData4f(unsigned int, float x, float y,
                                            float z, float w)
{
    if (!g_immediate.active ||
        g_immediate.request.vertex_count >= H2_RENDER_IMMEDIATE_MAX_VERTICES)
        return;

    h2_render_immediate_vertex &vertex =
        g_immediate.vertices[g_immediate.request.vertex_count++];
    vertex.texture_x = g_immediate.texture_x;
    vertex.texture_y = g_immediate.texture_y;
    vertex.x = x;
    vertex.y = y;
    vertex.z = z;
    vertex.w = w;
}

struct s_preview_vertex {
    float x;
    float y;
    float z;
    float w;
    float texture_x;
    float texture_y;
    unsigned int color;
};
static_assert(sizeof(s_preview_vertex) == 28, "recovered preview vertex layout");

extern "C" void h2_render_preview_quad(long bitmap_tag, long bitmap_index,
                                         long layer, void const *vertices)
{
    const auto *source = static_cast<const s_preview_vertex *>(vertices);
    g_immediate.request.kind = H2_RENDER_REQUEST_PREVIEW_QUAD;
    g_immediate.request.version = H2_RENDER_PROTOCOL_VERSION;
    g_immediate.request.primitive_type = 7;
    g_immediate.request.vertex_count = H2_RENDER_IMMEDIATE_MAX_VERTICES;
    g_immediate.request.color = source[0].color;
    g_immediate.request.texture_tag = static_cast<int>(bitmap_tag);
    g_immediate.request.texture_index = static_cast<int>(bitmap_index);
    g_immediate.request.layer = static_cast<int>(layer);
    for (unsigned int i = 0; i < H2_RENDER_IMMEDIATE_MAX_VERTICES; ++i) {
        g_immediate.vertices[i].texture_x = source[i].texture_x;
        g_immediate.vertices[i].texture_y = source[i].texture_y;
        g_immediate.vertices[i].x = source[i].x;
        g_immediate.vertices[i].y = source[i].y;
        g_immediate.vertices[i].z = source[i].z;
        g_immediate.vertices[i].w = source[i].w;
    }
    host_h2_render_request(&g_immediate.request, g_immediate.vertices);
}

extern "C" void D3DDevice_End()
{
    if (!g_immediate.active)
        return;
    g_immediate.active = false;
    host_h2_render_request(&g_immediate.request, g_immediate.vertices);
}

struct D3DTexture;
struct s_bitmap_view;

bool function_10f5c0(long, float, long, long, long)
{
    /* Character-model animation has no native model renderer yet. */
    return false;
}

D3DTexture *function_12360(s_bitmap_view *, float)
{
    /* Native texture ownership stays on the host side of render requests. */
    return 0;
}

void function_13ed90(long)
{
    /* Host text rendering does not consume an Xbox font object. */
}

bool function_13ee20(unsigned short const *text, long)
{
    /* The host owns glyph availability synchronously, so a valid recovered
     * string can proceed to the explicit text request without an Xbox cache. */
    return text != 0;
}

void function_13e9c0(unsigned short const *, short_rectangle2d const *bounds,
                     short_rectangle2d *ink_bounds,
                     short_rectangle2d *line_bounds, float)
{
    /* Until the host exposes text metrics, preserve deterministic layout by
     * using the caller's recovered bounding rectangle conservatively. */
    if (ink_bounds && bounds)
        *ink_bounds = *bounds;
    if (line_bounds && bounds)
        *line_bounds = *bounds;
}

long function_211a0(long source, float, float, float, long, long, long, long,
                    long, float, float)
{
    /* Native bootstrap has no post-processing surface chain yet. */
    return source;
}

#ifdef H2_MS_ABI_FRONTEND
void function_2360c3(short_rectangle2d const *, point3f *);
#endif

static unsigned int h2_pack_color4f(color4f const *color)
{
    auto channel = [](float value) -> unsigned int {
        if (value < 0.0f)
            value = 0.0f;
        else if (value > 1.0f)
            value = 1.0f;
        return static_cast<unsigned int>(value * 255.0f + 0.5f);
    };
    return (channel(color->alpha) << 24) | (channel(color->red) << 16) |
           (channel(color->green) << 8) | channel(color->blue);
}

void function_235d69(short_rectangle2d const *rectangle, float depth,
                     short_rectangle2d const *screen, color4f const *color)
{
    point3f points[4];
    points[0].n[0] = static_cast<float>(rectangle->left);
    points[0].n[1] = static_cast<float>(rectangle->top);
    points[1].n[0] = static_cast<float>(rectangle->left);
    points[1].n[1] = static_cast<float>(rectangle->bottom);
    points[2].n[0] = static_cast<float>(rectangle->right);
    points[2].n[1] = static_cast<float>(rectangle->bottom);
    points[3].n[0] = static_cast<float>(rectangle->right);
    points[3].n[1] = static_cast<float>(rectangle->top);
    for (auto &point : points) {
        point.n[2] = depth;
#ifdef H2_MS_ABI_FRONTEND
        function_2360c3(screen, &point);
#else
        (void)screen;
#endif
    }

    D3DDevice_Begin(7);
    D3DDevice_SetVertexDataColor(0, h2_pack_color4f(color));
    for (const auto &point : points)
        D3DDevice_SetVertexData4f(0, point.n[0], point.n[1], point.n[2], 1.0f);
    D3DDevice_End();
}
