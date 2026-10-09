#include "h2/host_abi.h"
#include "h2/render_protocol.h"

/* Native menu rendering does not use the Xbox cache-prediction system yet.
 * The recovered callers use this as a best-effort preload hint, so fail
 * conservatively without fabricating loaded texture, geometry, or audio state.
 * Actual resource ownership remains at the explicit host rendering boundary. */
struct s_predicted_resource_block;
union point3f;

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
