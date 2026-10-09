#include "h2/host_abi.h"
#include "h2/render_protocol.h"

/* Native menu rendering does not use the Xbox cache-prediction system yet.
 * The recovered callers use this as a best-effort preload hint, so fail
 * conservatively without fabricating loaded texture, geometry, or audio state.
 * Actual resource ownership remains at the explicit host rendering boundary. */
struct s_predicted_resource_block;

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

extern "C" void D3DDevice_End()
{
    if (!g_immediate.active)
        return;
    g_immediate.active = false;
    host_h2_render_request(&g_immediate.request, g_immediate.vertices);
}
