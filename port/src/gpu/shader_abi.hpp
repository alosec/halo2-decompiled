/* The contract between the Metal backend and the shader translators.
 *
 * The backend (metal_renderer.mm) owns pipelines, vertex fetch and the
 * conversion of NV2A screen-space positions to Metal clip space. The
 * translators turn Xbox shader data into Metal Shading Language functions
 * that the backend wraps in its own entry points:
 *
 *   - translate_vertex_program: NV2A vertex program microcode into
 *     `H2VertexResult h2_vertex_program(thread const float4 (&v)[16],
 *                                       constant float4 *c)`
 *   - translate_pixel_shader: a D3DPIXELSHADERDEF (register combiners and
 *     texture modes) into
 *     `float4 h2_pixel_shader(thread const H2PixelInput &in,
 *                             constant H2PixelConstants &k,
 *                             H2Textures t)`
 *
 * Both get kShaderPrelude prepended, so they may use its types. Generated
 * source must compile with the Metal 3 language (`xcrun metal -std=metal3.0`)
 * and must not declare entry points (`vertex`/`fragment` functions), buffers
 * or attributes of its own.
 *
 * Nothing in here comes from SDK headers or GPL emulator sources; encodings
 * are documented next to the code that decodes them, with their public
 * sources. */
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace h2::gpu {

inline constexpr const char* kShaderPrelude = R"msl(
#include <metal_stdlib>
using namespace metal;

/* What a vertex program writes. Registers it never writes keep these
   defaults: position (0,0,0,1), colours (0,0,0,1) for oD0/oD1/oB0/oB1
   (the hardware default is black opaque), texture coordinates (0,0,0,1),
   fog 0, point size 1. position is the raw oPos: NV2A screen space (x, y in
   render-target pixels and z in depth-buffer units, all still multiplied by
   w); the backend divides and maps it to clip space. */
struct H2VertexResult {
    float4 position;
    float4 diffuse;          /* oD0 */
    float4 specular;         /* oD1 */
    float4 back_diffuse;     /* oB0 */
    float4 back_specular;    /* oB1 */
    float4 texcoord[4];      /* oT0..oT3 */
    float fog;               /* oFog.x */
    float point_size;        /* oPts.x */
};

/* What the pixel stage receives: the interpolated vertex results (back
   colours already chosen for back faces by the backend), plus the fog factor
   and whether the fragment belongs to a front face. */
struct H2PixelInput {
    float4 diffuse;
    float4 specular;
    float4 texcoord[4];
    float fog;
    bool front_facing;
    float4 position;         /* window x, y in pixels, z 0..1, w = 1/clip w */
};

/* Constants a pixel shader reads. The per-stage combiner constants c0/c1 are
   already resolved from the definition's PSConstant0/1 words (or the render
   state values the game set for them); final_c0/c1 likewise. Colours are
   RGBA in 0..1. */
struct H2PixelConstants {
    float4 c0[8];
    float4 c1[8];
    float4 final_c0;
    float4 final_c1;
    float4 fog_color;        /* rgb used, a ignored */
    float4 bump_env[4];      /* stage n: (m00, m01, m10, m11) of D3DTSS_BUMPENVMAT */
    float4 bump_luminance[4];/* stage n: (scale, offset, 0, 0) */
    float4 color_key[4];     /* stage n: colour-key colour, RGBA */
};

/* The four texture stages. Each stage is bound as all three kinds so a
   pixel shader can sample whichever its texture mode needs; the backend binds
   a 1x1 placeholder to the kinds that are not in use. */
struct H2Textures {
    texture2d<float> tex2d[4];
    texturecube<float> cube[4];
    texture3d<float> volume[4];
    sampler samplers[4];
};
)msl";

/* What a translated vertex program needs from the backend beyond its code. */
struct VertexProgramInfo {
    std::string source;               /* MSL defining h2_vertex_program */
    std::uint32_t attributes_read = 0; /* bit n: reads v[n] */
    std::string error;                /* non-empty: could not translate */
};

/* `instructions` holds 4 words per instruction, as the game uploads them to
   the NV2A; translation starts at instruction `start` and ends at the first
   instruction flagged final (or the end of the array). */
VertexProgramInfo translate_vertex_program(const std::uint32_t* instructions,
                                           std::size_t instruction_count,
                                           std::size_t start);

/* Per-stage facts the pixel shader translator cannot read from the
   definition itself. */
struct PixelShaderContext {
    /* 2 = 2D texture, 3 = 3D (volume), 6 = cube map, 0 = no texture bound */
    std::uint8_t texture_dimension[4] = {0, 0, 0, 0};
    /* the texture's format is a signed one (texture values -1..1 already) */
    bool texture_signed[4] = {false, false, false, false};
};

struct PixelShaderInfo {
    std::string source;               /* MSL defining h2_pixel_shader */
    std::string error;
};

/* `definition` is the 60-word D3DPIXELSHADERDEF exactly as the game built it. */
PixelShaderInfo translate_pixel_shader(const std::uint32_t (&definition)[60],
                                       const PixelShaderContext& context);

} // namespace h2::gpu
