/* The Metal renderer (metal_renderer.hpp).
 *
 * One frame is one command buffer. The back buffer is an offscreen colour
 * and depth target at (640 x 480) * scale; a clear of the whole target
 * becomes the load action of the next render pass, and present blits the
 * colour target into the window's layer, letterboxed to the game's aspect.
 *
 * Per draw:
 *   - vertices: the 16 vertex registers of every vertex the draw uses, as
 *     float4s, decoded from the NV2A array formats (or copied from a
 *     Begin/End batch) into a per-frame buffer;
 *   - primitives the GPU lacks (fans, quads, quad strips, polygons, line
 *     loops) become indexed triangle or line lists;
 *   - shaders: the translated vertex program (vertex_program.cpp) or the
 *     screen-space fixed-function path, and the translated pixel shader
 *     (pixel_shader.cpp) or a fixed-function texture-times-diffuse stand-in,
 *     wrapped in entry points here and cached by source;
 *   - the NV2A's screen-space position (x, y in target pixels, z in depth
 *     units, w the clip w; see shader_abi.hpp) becomes Metal clip space.
 */
#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

#include "metal_renderer.hpp"

#include "h2/guest_arena.hpp"
#include "shader_abi.hpp"
#include "texture_formats.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <string>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <map>
#include <vector>

namespace h2::gpu {
namespace {

constexpr int kGuestWidth = 640;
constexpr int kGuestHeight = 480;
/* the depth buffer's units (D24S8) */
constexpr float kDepthUnits = 16777215.0f;
constexpr std::size_t kFrameBufferBytes = 64u << 20;

bool trace_gpu() {
    static const bool enabled = [] {
        const char* trace = std::getenv("H2_TRACE");
        return trace && std::strstr(trace, "gpu");
    }();
    return enabled;
}

std::uint64_t hash_bytes(const void* data, std::size_t size, std::uint64_t seed = 1469598103934665603ull) {
    const auto* bytes = static_cast<const std::uint8_t*>(data);
    std::uint64_t hash = seed;
    for (std::size_t i = 0; i < size; ++i) {
        hash ^= bytes[i];
        hash *= 1099511628211ull;
    }
    return hash;
}

/* a D3DCOLOR (ARGB) as RGBA floats */
std::array<float, 4> unpack_argb(std::uint32_t color) {
    return {((color >> 16) & 0xff) / 255.0f, ((color >> 8) & 0xff) / 255.0f,
            (color & 0xff) / 255.0f, (color >> 24) / 255.0f};
}

const char* kEntryPoints = R"msl(
struct H2Varyings {
    /* invariant: the title draws a surface in several passes with different
       vertex programs and tests depth EQUAL against the first pass */
    float4 position [[position, invariant]];
    float4 diffuse;
    float4 specular;
    float4 back_diffuse;
    float4 back_specular;
    float4 t0;
    float4 t1;
    float4 t2;
    float4 t3;
    float fog;
    float point_size [[point_size]];
};

struct H2ScreenUniforms {
    float4 window_to_ndc;   /* x scale, y scale, x offset, y offset */
    float z_scale;          /* depth units to 0..1 */
    float pad0;
    float pad1;
    float pad2;
};

struct H2FragmentUniforms {
    uint alpha_test;
    uint alpha_func;
    float alpha_ref;
    uint two_sided;
    float4 texel_scale[4];   /* xy: Metal texels per game texel of a scaled target */
};

vertex H2Varyings h2_vertex_main(uint vid [[vertex_id]],
                                 const device float4 *vertices [[buffer(0)]],
                                 constant float4 *c [[buffer(1)]],
                                 constant H2ScreenUniforms &u [[buffer(2)]])
{
    float4 v[16];
    for (uint i = 0; i < 16; ++i)
        v[i] = vertices[vid * 16 + i];
    H2VertexResult r = h2_vertex_program(v, c);
    float w = r.position.w;
    H2Varyings o;
    o.position = float4((r.position.x * u.window_to_ndc.x + u.window_to_ndc.z) * w,
                        (r.position.y * u.window_to_ndc.y + u.window_to_ndc.w) * w,
                        r.position.z * u.z_scale * w,
                        w);
    o.diffuse = saturate(r.diffuse);
    o.specular = saturate(r.specular);
    o.back_diffuse = saturate(r.back_diffuse);
    o.back_specular = saturate(r.back_specular);
    o.t0 = r.texcoord[0];
    o.t1 = r.texcoord[1];
    o.t2 = r.texcoord[2];
    o.t3 = r.texcoord[3];
    o.fog = r.fog;
    o.point_size = max(r.point_size, 1.0f);
    return o;
}

bool h2_compare(float value, float reference, uint function)
{
    switch (function) {
    case 0: return false;
    case 1: return value < reference;
    case 2: return value == reference;
    case 3: return value <= reference;
    case 4: return value > reference;
    case 5: return value != reference;
    case 6: return value >= reference;
    default: return true;
    }
}

fragment float4 h2_fragment_main(H2Varyings v [[stage_in]],
                                 bool front [[front_facing]],
                                 constant H2PixelConstants &k [[buffer(0)]],
                                 constant H2FragmentUniforms &f [[buffer(1)]],
                                 texture2d<float> a0 [[texture(0)]], texture2d<float> a1 [[texture(1)]],
                                 texture2d<float> a2 [[texture(2)]], texture2d<float> a3 [[texture(3)]],
                                 texturecube<float> b0 [[texture(4)]], texturecube<float> b1 [[texture(5)]],
                                 texturecube<float> b2 [[texture(6)]], texturecube<float> b3 [[texture(7)]],
                                 texture3d<float> d0 [[texture(8)]], texture3d<float> d1 [[texture(9)]],
                                 texture3d<float> d2 [[texture(10)]], texture3d<float> d3 [[texture(11)]],
                                 sampler s0 [[sampler(0)]], sampler s1 [[sampler(1)]],
                                 sampler s2 [[sampler(2)]], sampler s3 [[sampler(3)]])
{
    H2PixelInput in;
    bool back = f.two_sided != 0 && !front;
    in.diffuse = back ? v.back_diffuse : v.diffuse;
    in.specular = back ? v.back_specular : v.specular;
    in.texcoord[0] = v.t0;
    in.texcoord[1] = v.t1;
    in.texcoord[2] = v.t2;
    in.texcoord[3] = v.t3;
    for (int i = 0; i < 4; ++i)
        in.texcoord[i].xy *= f.texel_scale[i].xy;
    in.fog = v.fog;
    in.front_facing = front;
    in.position = v.position;
    H2Textures t;
    t.tex2d[0] = a0; t.tex2d[1] = a1; t.tex2d[2] = a2; t.tex2d[3] = a3;
    t.cube[0] = b0; t.cube[1] = b1; t.cube[2] = b2; t.cube[3] = b3;
    t.volume[0] = d0; t.volume[1] = d1; t.volume[2] = d2; t.volume[3] = d3;
    t.samplers[0] = s0; t.samplers[1] = s1; t.samplers[2] = s2; t.samplers[3] = s3;
    float4 color = h2_pixel_shader(in, k, t);
    if (f.alpha_test != 0 && !h2_compare(color.a, f.alpha_ref, f.alpha_func))
        discard_fragment();
    return color;
}
)msl";

/* fixed-function vertices: already transformed (x, y, z, rhw) positions in
   register 0, then the Xbox's fixed-function register assignment (diffuse 3,
   specular 4, fog 5, back colours 7 and 8, texture coordinates 9 to 12) */
const char* kScreenVertexProgram = R"msl(
H2VertexResult h2_vertex_program(thread const float4 (&v)[16], constant float4 *c)
{
    H2VertexResult o;
    float rhw = v[0].w;
    float w = (rhw != 0.0f) ? 1.0f / rhw : 1.0f;
    o.position = float4(v[0].x, v[0].y, v[0].z, w);
    o.diffuse = v[3];
    o.specular = v[4];
    o.back_diffuse = v[7];
    o.back_specular = v[8];
    o.texcoord[0] = v[9];
    o.texcoord[1] = v[10];
    o.texcoord[2] = v[11];
    o.texcoord[3] = v[12];
    o.fog = v[5].x;
    o.point_size = 1.0f;
    return o;
}
)msl";

/* the fixed-function stand-in when no pixel shader is set: the diffuse
   colour, times the stage 0 texture when one is bound */
std::string fixed_function_pixel_shader(bool texture0) {
    std::string source =
        "float4 h2_pixel_shader(thread const H2PixelInput &in, constant H2PixelConstants &k, H2Textures t)\n"
        "{\n"
        "    float4 color = in.diffuse;\n";
    if (texture0)
        source += "    color *= t.tex2d[0].sample(t.samplers[0], in.texcoord[0].xy);\n";
    source += "    return color;\n}\n";
    return source;
}

struct DecodedFormat {
    int count;
    int type;
};

/* one NV2A vertex array element as a float4 */
void decode_attribute(const std::uint8_t* element, std::uint32_t format, float out[4]) {
    const int count = static_cast<int>(format >> 4);
    const int type = static_cast<int>(format & 0xf);
    out[0] = 0.0f;
    out[1] = 0.0f;
    out[2] = 0.0f;
    out[3] = 1.0f;
    switch (type) {
    case 0: /* D3D colour: bytes B, G, R, A */
        if (count >= 4) {
            out[0] = element[2] / 255.0f;
            out[1] = element[1] / 255.0f;
            out[2] = element[0] / 255.0f;
            out[3] = element[3] / 255.0f;
        } else {
            for (int i = 0; i < count; ++i)
                out[i] = element[i] / 255.0f;
        }
        break;
    case 1: /* normalised shorts */
        for (int i = 0; i < count && i < 4; ++i) {
            std::int16_t value;
            std::memcpy(&value, element + 2 * i, 2);
            out[i] = std::max(-1.0f, value / 32767.0f);
        }
        break;
    case 2: /* floats */
        for (int i = 0; i < count && i < 4; ++i)
            std::memcpy(&out[i], element + 4 * i, 4);
        break;
    case 4: /* bytes in order */
        for (int i = 0; i < count && i < 4; ++i)
            out[i] = element[i] / 255.0f;
        break;
    case 5: /* shorts */
        for (int i = 0; i < count && i < 4; ++i) {
            std::int16_t value;
            std::memcpy(&value, element + 2 * i, 2);
            out[i] = value;
        }
        break;
    case 6: { /* packed signed 11:11:10 normals */
        std::uint32_t packed;
        std::memcpy(&packed, element, 4);
        auto field = [](std::uint32_t value, int bits) {
            const std::int32_t shifted = static_cast<std::int32_t>(value << (32 - bits)) >> (32 - bits);
            return std::max(-1.0f, shifted / static_cast<float>((1 << (bits - 1)) - 1));
        };
        out[0] = field(packed & 0x7ff, 11);
        out[1] = field((packed >> 11) & 0x7ff, 11);
        out[2] = field(packed >> 22, 10);
        break;
    }
    default:
        break;
    }
}

int element_bytes(std::uint32_t format) {
    const int count = static_cast<int>(format >> 4);
    switch (format & 0xf) {
    case 0: case 4: return count;
    case 1: case 5: return 2 * count;
    case 2: return 4 * count;
    case 6: return 4;
    default: return 0;
    }
}

struct PipelineKey {
    std::uint64_t vertex = 0;
    std::uint64_t fragment = 0;
    std::uint32_t blend = 0;
    std::uint32_t blend_factors = 0;
    bool operator==(const PipelineKey& other) const {
        return vertex == other.vertex && fragment == other.fragment && blend == other.blend &&
               blend_factors == other.blend_factors;
    }
};

struct PipelineKeyHash {
    std::size_t operator()(const PipelineKey& key) const {
        return static_cast<std::size_t>(key.vertex * 31 + key.fragment * 17 + key.blend * 7 + key.blend_factors);
    }
};

struct CachedTexture {
    id<MTLTexture> texture = nil;
    std::uint64_t content = 0;
    std::uint32_t kind = 2;       /* 2, 3 (volume) or 6 (cube) */
    bool linear = false;
    bool is_signed = false;
    /* Metal texels per game texel, for a render target drawn at a scale: the
       game addresses a linear texture in its own texels */
    float texel_scale_x = 1.0f, texel_scale_y = 1.0f;
    std::uint64_t last_frame = 0;
    std::uint64_t hashed_frame = ~0ull;  /* frame whose hash is current */
    std::uint64_t changed_frame = 0;     /* frame the contents last changed */
};

MTLBlendFactor blend_factor(std::uint32_t factor) {
    switch (factor) {
    case H2_GPU_ZERO: return MTLBlendFactorZero;
    case H2_GPU_ONE: return MTLBlendFactorOne;
    case H2_GPU_SRC_COLOR: return MTLBlendFactorSourceColor;
    case H2_GPU_INV_SRC_COLOR: return MTLBlendFactorOneMinusSourceColor;
    case H2_GPU_SRC_ALPHA: return MTLBlendFactorSourceAlpha;
    case H2_GPU_INV_SRC_ALPHA: return MTLBlendFactorOneMinusSourceAlpha;
    case H2_GPU_DST_ALPHA: return MTLBlendFactorDestinationAlpha;
    case H2_GPU_INV_DST_ALPHA: return MTLBlendFactorOneMinusDestinationAlpha;
    case H2_GPU_DST_COLOR: return MTLBlendFactorDestinationColor;
    case H2_GPU_INV_DST_COLOR: return MTLBlendFactorOneMinusDestinationColor;
    case H2_GPU_SRC_ALPHA_SATURATE: return MTLBlendFactorSourceAlphaSaturated;
    case H2_GPU_CONSTANT_COLOR: return MTLBlendFactorBlendColor;
    case H2_GPU_INV_CONSTANT_COLOR: return MTLBlendFactorOneMinusBlendColor;
    case H2_GPU_CONSTANT_ALPHA: return MTLBlendFactorBlendAlpha;
    case H2_GPU_INV_CONSTANT_ALPHA: return MTLBlendFactorOneMinusBlendAlpha;
    default: return MTLBlendFactorOne;
    }
}

MTLBlendOperation blend_operation(std::uint32_t op) {
    switch (op) {
    case H2_GPU_SUBTRACT: return MTLBlendOperationSubtract;
    case H2_GPU_REVERSE_SUBTRACT: return MTLBlendOperationReverseSubtract;
    case H2_GPU_MIN: return MTLBlendOperationMin;
    case H2_GPU_MAX: return MTLBlendOperationMax;
    default: return MTLBlendOperationAdd;
    }
}

MTLCompareFunction compare_function(std::uint32_t function) {
    switch (function) {
    case H2_GPU_NEVER: return MTLCompareFunctionNever;
    case H2_GPU_LESS: return MTLCompareFunctionLess;
    case H2_GPU_EQUAL: return MTLCompareFunctionEqual;
    case H2_GPU_LESS_EQUAL: return MTLCompareFunctionLessEqual;
    case H2_GPU_GREATER: return MTLCompareFunctionGreater;
    case H2_GPU_NOT_EQUAL: return MTLCompareFunctionNotEqual;
    case H2_GPU_GREATER_EQUAL: return MTLCompareFunctionGreaterEqual;
    default: return MTLCompareFunctionAlways;
    }
}

MTLStencilOperation stencil_operation(std::uint32_t op) {
    switch (op) {
    case H2_GPU_STENCIL_ZERO: return MTLStencilOperationZero;
    case H2_GPU_REPLACE: return MTLStencilOperationReplace;
    case H2_GPU_INCREMENT_CLAMP: return MTLStencilOperationIncrementClamp;
    case H2_GPU_DECREMENT_CLAMP: return MTLStencilOperationDecrementClamp;
    case H2_GPU_INVERT: return MTLStencilOperationInvert;
    case H2_GPU_INCREMENT_WRAP: return MTLStencilOperationIncrementWrap;
    case H2_GPU_DECREMENT_WRAP: return MTLStencilOperationDecrementWrap;
    default: return MTLStencilOperationKeep;
    }
}

MTLSamplerAddressMode address_mode(std::uint32_t mode, bool linear) {
    if (linear)
        return mode == H2_GPU_BORDER ? MTLSamplerAddressModeClampToZero : MTLSamplerAddressModeClampToEdge;
    switch (mode) {
    case H2_GPU_MIRROR: return MTLSamplerAddressModeMirrorRepeat;
    case H2_GPU_CLAMP:
    case H2_GPU_CLAMP_TO_EDGE: return MTLSamplerAddressModeClampToEdge;
    case H2_GPU_BORDER: return MTLSamplerAddressModeClampToBorderColor;
    default: return MTLSamplerAddressModeRepeat;
    }
}

class MetalRenderer final : public Renderer {
private:
    /* where draws go: the back buffer, or a surface the game renders to
       (keyed by its texels' guest address, where a texture header that
       samples it will point) */
    /* a depth/stencil buffer. The game binds one separately from the colour
       target (SetRenderTarget's z surface), and several colour targets can
       share it: the lighting pass of a model draws into an off-screen target
       with the back buffer's depth, and the colour pass after it tests EQUAL
       against that depth */
    struct DepthBuffer {
        id<MTLTexture> texture = nil;
        int width = 0, height = 0;   /* Metal pixels */
        bool has_content = false;
        bool pending_depth_clear = false;
        bool pending_stencil_clear = false;
        double clear_depth = 1.0;
        std::uint32_t clear_stencil = 0;
    };

    struct Target {
        id<MTLTexture> color = nil;
        DepthBuffer own_depth;   /* used when the game binds no depth surface */
        int width = 0, height = 0;               /* Metal pixels */
        int guest_width = 0, guest_height = 0;   /* the game's pixels */
        bool has_content = false;
        bool pending_color_clear = false;
        MTLClearColor clear_color = MTLClearColorMake(0, 0, 0, 1);
        id<MTLTexture> snapshot = nil;   /* a copy to sample while drawing into it */
        /* the guest memory under the target, hashed each frame it is drawn
           into: when it differs at a later sampling, the game has put a new
           texture there (the memory was freed and reused) and the target is
           stale */
        std::uint32_t data = 0;
        std::size_t guest_bytes = 0;
        std::uint64_t guest_hash = 0;
        std::uint64_t hashed_frame = ~0ull;
    };

public:
    MetalRenderer(const h2::port::GuestArena& arena, int scale) : arena_(arena), scale_(scale) {}

    bool initialize(std::string& error) {
        device_ = MTLCreateSystemDefaultDevice();
        if (!device_) {
            error = "no Metal device";
            return false;
        }
        queue_ = [device_ newCommandQueue];
        width_ = kGuestWidth * scale_;
        height_ = kGuestHeight * scale_;
        MTLTextureDescriptor* color = [MTLTextureDescriptor
            texture2DDescriptorWithPixelFormat:MTLPixelFormatBGRA8Unorm
                                         width:width_
                                        height:height_
                                     mipmapped:NO];
        color.usage = MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead;
        color.storageMode = MTLStorageModePrivate;
        color_ = [device_ newTextureWithDescriptor:color];
        MTLTextureDescriptor* depth = [MTLTextureDescriptor
            texture2DDescriptorWithPixelFormat:MTLPixelFormatDepth32Float_Stencil8
                                         width:width_
                                        height:height_
                                     mipmapped:NO];
        depth.usage = MTLTextureUsageRenderTarget;
        depth.storageMode = MTLStorageModePrivate;
        back_.color = color_;
        back_.own_depth.texture = [device_ newTextureWithDescriptor:depth];
        back_.own_depth.width = width_;
        back_.own_depth.height = height_;
        back_.width = width_;
        back_.height = height_;
        back_.guest_width = kGuestWidth;
        back_.guest_height = kGuestHeight;
        current_ = &back_;
        current_depth_ = &back_.own_depth;
        frame_buffers_.push_back([device_ newBufferWithLength:kFrameBufferBytes options:MTLResourceStorageModeShared]);
        if (!build_placeholders(error) || !build_present_pipeline(error) || !build_clear_pipelines(error))
            return false;
        return true;
    }

    void attach_layer(void* layer) override {
        layer_ = (__bridge CAMetalLayer*)layer;
        if (layer_) {
            layer_.device = device_;
            layer_.pixelFormat = MTLPixelFormatBGRA8Unorm;
            layer_.framebufferOnly = NO;   /* capture_window reads the drawable */
        }
    }

    /* H2_TRACE_FRAME=n: log every clear and draw of presented frame n */
    bool tracing_frame() const {
        static const int wanted = [] {
            const char* value = std::getenv("H2_TRACE_FRAME");
            return value ? std::atoi(value) : -1;
        }();
        return presented_ == wanted;
    }

    void clear(const h2_gpu_clear& packet) override {
        ++stats_.clears;
        if (tracing_frame())
            std::fprintf(stderr, "h2-frame: clear rt %08x flags %u color %08x depth %g rects %u\n",
                         packet.render_target, packet.flags, packet.color, packet.depth, packet.rect_count);
        Target* target = target_for(packet.render_target);
        if (!target)
            return;
        const bool color = packet.flags & 1;
        const bool depth = packet.flags & 2;
        const bool stencil = packet.flags & 4;
        /* rectangles short of the whole target are drawn over */
        const int guest_width = target == &back_ && packet.target_width
                                    ? static_cast<int>(packet.target_width) : target->guest_width;
        const int guest_height = target == &back_ && packet.target_height
                                     ? static_cast<int>(packet.target_height) : target->guest_height;
        /* a clear of some colour channels only is drawn, through a write mask */
        bool whole = packet.rect_count == 0;
        for (std::uint32_t index = 0; index < packet.rect_count && index < 8; ++index) {
            const auto* r = packet.rects[index];
            if (r[0] <= 0 && r[1] <= 0 && r[2] >= guest_width && r[3] >= guest_height)
                whole = true;
        }
        if (color && clear_write_mask(packet) != MTLColorWriteMaskAll)
            whole = false;
        if (!whole) {
            clear_rectangles(*target, depth_for(*target, packet.depth_stencil), packet, guest_width, guest_height);
            return;
        }
        /* depth and stencil clears go to the bound depth surface, whatever the
           colour target: the game clears depth for first-person models
           through a dummy 512x512 colour surface */
        DepthBuffer* depth_buffer = depth_for(*target, packet.depth_stencil);
        if (auto shared = depths_.find(packet.depth_stencil); packet.depth_stencil && shared != depths_.end())
            depth_buffer = &shared->second;
        if (target == current_ || depth_buffer == current_depth_)
            end_encoder();
        if (color) {
            target->pending_color_clear = true;
            const auto rgba = unpack_argb(packet.color);
            target->clear_color = MTLClearColorMake(rgba[0], rgba[1], rgba[2], rgba[3]);
        }
        if (depth) {
            depth_buffer->pending_depth_clear = true;
            depth_buffer->clear_depth = std::clamp(packet.depth, 0.0f, 1.0f);
        }
        if (stencil) {
            depth_buffer->pending_stencil_clear = true;
            depth_buffer->clear_stencil = packet.stencil & 0xff;
        }
    }

    /* the colour channels a clear writes (h2_gpu_clear flags 0x10..0x80) */
    static MTLColorWriteMask clear_write_mask(const h2_gpu_clear& packet) {
        if (!(packet.flags & 1))
            return MTLColorWriteMaskNone;
        if (!(packet.flags & 0xf0))
            return MTLColorWriteMaskAll;
        MTLColorWriteMask mask = MTLColorWriteMaskNone;
        if (packet.flags & 0x10) mask |= MTLColorWriteMaskRed;
        if (packet.flags & 0x20) mask |= MTLColorWriteMaskGreen;
        if (packet.flags & 0x40) mask |= MTLColorWriteMaskBlue;
        if (packet.flags & 0x80) mask |= MTLColorWriteMaskAlpha;
        return mask;
    }

    /* clears of parts of a target: a quad per rectangle, writing the clear
       values through scissors */
    void clear_rectangles(Target& target, DepthBuffer* depth_buffer, const h2_gpu_clear& packet, int guest_width,
                          int guest_height) {
        @autoreleasepool {
            bind(&target, depth_buffer);
            begin_encoder();
            const bool depth = packet.flags & 2;
            const bool stencil = packet.flags & 4;
            id<MTLRenderPipelineState> state = clear_pipelines_[clear_write_mask(packet)];
            [encoder_ setRenderPipelineState:state];
            const std::uint64_t key = (depth ? 1u : 0u) | (stencil ? 2u : 0u);
            id<MTLDepthStencilState> depth_state = clear_depth_states_[key];
            if (!depth_state) {
                MTLDepthStencilDescriptor* descriptor = [[MTLDepthStencilDescriptor alloc] init];
                descriptor.depthCompareFunction = MTLCompareFunctionAlways;
                descriptor.depthWriteEnabled = depth;
                if (stencil) {
                    MTLStencilDescriptor* s = [[MTLStencilDescriptor alloc] init];
                    s.stencilCompareFunction = MTLCompareFunctionAlways;
                    s.depthStencilPassOperation = MTLStencilOperationReplace;
                    s.writeMask = 0xff;
                    descriptor.frontFaceStencil = s;
                    descriptor.backFaceStencil = s;
                }
                depth_state = [device_ newDepthStencilStateWithDescriptor:descriptor];
                clear_depth_states_[key] = depth_state;
            }
            [encoder_ setDepthStencilState:depth_state];
            [encoder_ setStencilReferenceValue:packet.stencil & 0xff];
            [encoder_ setCullMode:MTLCullModeNone];
            [encoder_ setViewport:(MTLViewport){0, 0, static_cast<double>(target.width),
                                                static_cast<double>(target.height), 0, 1}];
            const auto rgba = unpack_argb(packet.color);
            const float values[5] = {rgba[0], rgba[1], rgba[2], rgba[3], std::clamp(packet.depth, 0.0f, 1.0f)};
            [encoder_ setFragmentBytes:values length:sizeof(values) atIndex:0];
            const double sx = static_cast<double>(target.width) / guest_width;
            const double sy = static_cast<double>(target.height) / guest_height;
            for (std::uint32_t index = 0; index < packet.rect_count && index < 8; ++index) {
                const auto* r = packet.rects[index];
                const double left = std::clamp(r[0] * sx, 0.0, static_cast<double>(target.width));
                const double top = std::clamp(r[1] * sy, 0.0, static_cast<double>(target.height));
                const double right = std::clamp(r[2] * sx, 0.0, static_cast<double>(target.width));
                const double bottom = std::clamp(r[3] * sy, 0.0, static_cast<double>(target.height));
                if (right <= left || bottom <= top)
                    continue;
                MTLScissorRect scissor = {static_cast<NSUInteger>(left), static_cast<NSUInteger>(top),
                                          static_cast<NSUInteger>(right - left), static_cast<NSUInteger>(bottom - top)};
                [encoder_ setScissorRect:scissor];
                [encoder_ drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:3];
            }
        }
    }

    void draw(const h2_gpu_draw& packet) override {
        ++stats_.draws;
        static int frame_draw = 0;
        static int counted_frame = -1;
        if (counted_frame != presented_) {
            counted_frame = presented_;
            frame_draw = 0;
        }
        ++frame_draw;
        if (tracing_frame())
            std::fprintf(stderr, "h2-frame: #%d\n", frame_draw);
        if (tracing_frame())
            std::fprintf(stderr,
                         "h2-frame: draw rt %08x %ux%u prim %u n %u src %u mode %u vp %u ps %u tex %08x/%08x "
                         "blend %u %u>%u z %u%u f%u cull %u mask %x view %g %g %g %g data %08x bb %08x fb %08x\n",
                         packet.render_target, packet.target_width, packet.target_height, packet.primitive,
                         packet.vertex_count, packet.vertex_source, packet.vertex_mode, packet.vertex_program_start,
                         packet.has_pixel_shader, packet.textures[0].header, packet.textures[0].format,
                         packet.blend_enable, packet.blend_src_color, packet.blend_dst_color,
                         packet.depth_test_enable, packet.depth_write_enable, packet.depth_func, packet.cull,
                         packet.color_write_mask, packet.viewport_x, packet.viewport_y, packet.viewport_width,
                         packet.viewport_height, packet.textures[0].data, packet.back_buffer_data,
                         packet.front_buffer_data);
        /* the frame trace also saves each draw's vertex constants (192 float4s) */
        if (tracing_frame() && packet.vertex_constants) {
            if (const char* dump_dir = std::getenv("H2_TRACE_DUMP")) {
                char path[512];
                std::snprintf(path, sizeof(path), "%s/c-%04d.bin", dump_dir, frame_draw);
                if (FILE* file = std::fopen(path, "wb")) {
                    std::fwrite(guest<float>(packet.vertex_constants), 16, 192, file);
                    std::fclose(file);
                }
                std::snprintf(path, sizeof(path), "%s/p-%04d.bin", dump_dir, frame_draw);
                if (FILE* file = std::fopen(path, "wb")) {
                    std::fwrite(&packet, sizeof(packet), 1, file);
                    std::fclose(file);
                }
            }
        }
        if (tracing_frame() && packet.vertex_source == H2_GPU_VERTICES_INLINE) {
            const auto* v = guest<float>(packet.inline_vertices);
            for (std::uint32_t i = 0; v && i < std::min<std::uint32_t>(packet.inline_vertex_count, 4); ++i) {
                const float* r = v + i * 64;
                std::fprintf(stderr, "h2-frame:   v%u", i);
                for (int reg = 0; reg < 16; ++reg)
                    if (r[reg * 4] || r[reg * 4 + 1] || r[reg * 4 + 2] || r[reg * 4 + 3])
                        std::fprintf(stderr, " r%d (%g %g %g %g)", reg, r[reg * 4], r[reg * 4 + 1], r[reg * 4 + 2],
                                     r[reg * 4 + 3]);
                std::fprintf(stderr, "\n");
            }
        }
        if (tracing_frame()) {
            for (int t = 0; t < 4; ++t)
                std::fprintf(stderr, "h2-frame:   tex%d %08x data %08x fmt %08x pal %08x\n", t,
                             packet.textures[t].header, packet.textures[t].data, packet.textures[t].format,
                             packet.textures[t].palette);
            const std::uint32_t* ps = packet.pixel_shader;
            std::fprintf(stderr, "h2-frame:   ps");
            for (int i = 0; i < 60; ++i)
                std::fprintf(stderr, " %08x", ps[i]);
            std::fprintf(stderr, "\n");
        }
        Target* target = target_for(packet.render_target);
        if (!target) {
            ++stats_.draws_skipped;
            return;
        }
        if (packet.vertex_count == 0)
            return;
        bind(target, depth_for(*target, packet.depth_stencil));
        @autoreleasepool {
            if (!encode_draw(packet))
                ++stats_.draws_failed;
        }
        static const char* dump_draws = std::getenv("H2_TRACE_DUMP_DRAWS");
        const char* dump_dir = std::getenv("H2_TRACE_DUMP");
        if (dump_dir && dump_draws && tracing_frame()) {
            const std::string list = std::string(",") + dump_draws + ",";
            /* "copies": every unblended screen-space draw */
            const bool copy = list == ",copies," && packet.vertex_source == H2_GPU_VERTICES_INLINE &&
                              !packet.blend_enable;
            if (copy || list.find("," + std::to_string(frame_draw) + ",") != std::string::npos)
                dump_targets(dump_dir, "-" + std::to_string(frame_draw));
        }
    }

    /* H2_TRACE_DUMP=dir: every target as a PPM (suffix tells the moment);
       runs the work so far and waits for it */
    void dump_targets(const char* dir, const std::string& suffix) {
        end_encoder();
        begin_command_buffer();
        std::vector<std::pair<std::string, std::pair<id<MTLTexture>, id<MTLBuffer>>>> dumps;
        auto add = [&](const std::string& name, id<MTLTexture> texture) {
            if (!texture)
                return;
            id<MTLBuffer> buffer = [device_ newBufferWithLength:texture.width * texture.height * 4
                                                        options:MTLResourceStorageModeShared];
            id<MTLBlitCommandEncoder> blit = [command_buffer_ blitCommandEncoder];
            [blit copyFromTexture:texture sourceSlice:0 sourceLevel:0 sourceOrigin:MTLOriginMake(0, 0, 0)
                       sourceSize:MTLSizeMake(texture.width, texture.height, 1) toBuffer:buffer
                destinationOffset:0 destinationBytesPerRow:texture.width * 4
                destinationBytesPerImage:texture.width * texture.height * 4];
            [blit endEncoding];
            dumps.push_back({name, {texture, buffer}});
        };
        add("back", back_.color);
        for (auto& [data, target] : targets_) {
            char name[32];
            std::snprintf(name, sizeof(name), "target-%08x", data);
            add(name, target.color);
        }
        [command_buffer_ commit];
        [command_buffer_ waitUntilCompleted];
        command_buffer_ = nil;
        for (auto& [name, pair] : dumps) {
            id<MTLTexture> texture = pair.first;
            const auto* bgra = static_cast<const std::uint8_t*>(pair.second.contents);
            const std::string path = std::string(dir) + "/" + name + suffix + ".ppm";
            if (FILE* file = std::fopen(path.c_str(), "wb")) {
                std::fprintf(file, "P6 %lu %lu 255\n", static_cast<unsigned long>(texture.width),
                             static_cast<unsigned long>(texture.height));
                for (NSUInteger i = 0; i < texture.width * texture.height; ++i) {
                    const std::uint8_t rgb[3] = {bgra[i * 4 + 2], bgra[i * 4 + 1], bgra[i * 4]};
                    std::fwrite(rgb, 1, 3, file);
                }
                std::fclose(file);
            }
        }
    }

    void present(bool keep_pixels) override {
        ++presented_;
        @autoreleasepool {
            begin_command_buffer();
            bind(&back_, depth_for(back_, back_depth_surface_));
            if (!encoder_)
                begin_encoder();   /* flush any pending clear */
            end_encoder();
            if (layer_) {
                id<CAMetalDrawable> drawable = [layer_ nextDrawable];
                if (drawable)
                    encode_present(drawable);
                window_kept_ = false;
                if (drawable && capture_window_) {
                    capture_window_ = false;
                    const NSUInteger w = drawable.texture.width, h = drawable.texture.height;
                    if (!window_readback_ || window_readback_.length < w * h * 4)
                        window_readback_ = [device_ newBufferWithLength:w * h * 4 options:MTLResourceStorageModeShared];
                    id<MTLBlitCommandEncoder> blit = [command_buffer_ blitCommandEncoder];
                    [blit copyFromTexture:drawable.texture sourceSlice:0 sourceLevel:0
                             sourceOrigin:MTLOriginMake(0, 0, 0) sourceSize:MTLSizeMake(w, h, 1)
                                 toBuffer:window_readback_ destinationOffset:0 destinationBytesPerRow:w * 4
                    destinationBytesPerImage:w * h * 4];
                    [blit endEncoding];
                    window_width_ = static_cast<int>(w);
                    window_height_ = static_cast<int>(h);
                    window_kept_ = true;
                }
                if (drawable)
                    [command_buffer_ presentDrawable:drawable];
            }
            if (keep_pixels) {
                if (!readback_ || readback_.length < static_cast<NSUInteger>(width_ * height_ * 4))
                    readback_ = [device_ newBufferWithLength:width_ * height_ * 4
                                                     options:MTLResourceStorageModeShared];
                id<MTLBlitCommandEncoder> blit = [command_buffer_ blitCommandEncoder];
                [blit copyFromTexture:color_
                                 sourceSlice:0
                                 sourceLevel:0
                                sourceOrigin:MTLOriginMake(0, 0, 0)
                                  sourceSize:MTLSizeMake(width_, height_, 1)
                                    toBuffer:readback_
                           destinationOffset:0
                      destinationBytesPerRow:width_ * 4
                    destinationBytesPerImage:width_ * height_ * 4];
                [blit endEncoding];
            }
            const char* dump_dir = std::getenv("H2_TRACE_DUMP");
            if (dump_dir && tracing_frame())
                dump_targets(dump_dir, "");
            [command_buffer_ commit];
            /* one frame in flight: the per-frame buffers are reused next frame */
            [command_buffer_ waitUntilCompleted];
            command_buffer_ = nil;
            frame_offset_ = 0;
            frame_buffer_ = 0;
            ++frame_;
            ++stats_.frames;
            kept_ = keep_pixels;
            if (trace_gpu() && (frame_ <= 4 || frame_ % 120 == 0))
                std::fprintf(stderr, "h2-gpu: frame %llu draws %llu skipped %llu failed %llu pipelines %llu textures %llu\n",
                             static_cast<unsigned long long>(frame_),
                             static_cast<unsigned long long>(stats_.draws),
                             static_cast<unsigned long long>(stats_.draws_skipped),
                             static_cast<unsigned long long>(stats_.draws_failed),
                             static_cast<unsigned long long>(stats_.pipelines),
                             static_cast<unsigned long long>(stats_.textures));
        }
    }

    bool read_pixels(std::vector<std::uint8_t>& bgra, int& width, int& height) override {
        if (!kept_ || !readback_)
            return false;
        width = width_;
        height = height_;
        bgra.assign(static_cast<const std::uint8_t*>(readback_.contents),
                    static_cast<const std::uint8_t*>(readback_.contents) + width_ * height_ * 4);
        return true;
    }

    void capture_window() override { capture_window_ = true; }

    bool read_window_pixels(std::vector<std::uint8_t>& bgra, int& width, int& height) override {
        if (!window_kept_ || !window_readback_)
            return false;
        width = window_width_;
        height = window_height_;
        bgra.assign(static_cast<const std::uint8_t*>(window_readback_.contents),
                    static_cast<const std::uint8_t*>(window_readback_.contents) + width * height * 4);
        return true;
    }

    RendererStats stats() const override { return stats_; }

private:
    /* guest memory */
    template <typename T>
    const T* guest(std::uint32_t address) const {
        return address ? static_cast<const T*>(arena_.pointer_const(address)) : nullptr;
    }

    bool build_placeholders(std::string& error) {
        /* a stage with no texture: the NV2A's disabled texture unit reads
           zero (a shader may still sample it; Halo's 4-tap filters do with
           only stage 0 bound) */
        const std::uint32_t white = 0x00000000u;
        MTLTextureDescriptor* d2 = [MTLTextureDescriptor
            texture2DDescriptorWithPixelFormat:MTLPixelFormatBGRA8Unorm width:1 height:1 mipmapped:NO];
        placeholder_2d_ = [device_ newTextureWithDescriptor:d2];
        [placeholder_2d_ replaceRegion:MTLRegionMake2D(0, 0, 1, 1) mipmapLevel:0 withBytes:&white bytesPerRow:4];
        MTLTextureDescriptor* cube = [MTLTextureDescriptor
            textureCubeDescriptorWithPixelFormat:MTLPixelFormatBGRA8Unorm size:1 mipmapped:NO];
        placeholder_cube_ = [device_ newTextureWithDescriptor:cube];
        for (NSUInteger face = 0; face < 6; ++face)
            [placeholder_cube_ replaceRegion:MTLRegionMake2D(0, 0, 1, 1) mipmapLevel:0 slice:face
                                   withBytes:&white bytesPerRow:4 bytesPerImage:4];
        MTLTextureDescriptor* volume = [[MTLTextureDescriptor alloc] init];
        volume.textureType = MTLTextureType3D;
        volume.pixelFormat = MTLPixelFormatBGRA8Unorm;
        volume.width = volume.height = volume.depth = 1;
        placeholder_3d_ = [device_ newTextureWithDescriptor:volume];
        [placeholder_3d_ replaceRegion:MTLRegionMake3D(0, 0, 0, 1, 1, 1) mipmapLevel:0 slice:0
                             withBytes:&white bytesPerRow:4 bytesPerImage:4];
        MTLSamplerDescriptor* sampler = [[MTLSamplerDescriptor alloc] init];
        placeholder_sampler_ = [device_ newSamplerStateWithDescriptor:sampler];
        if (!placeholder_2d_ || !placeholder_cube_ || !placeholder_3d_) {
            error = "placeholder textures failed";
            return false;
        }
        return true;
    }

    bool build_clear_pipelines(std::string& error) {
        static const char* source = R"msl(
#include <metal_stdlib>
using namespace metal;
struct ClearOut { float4 position [[position]]; };
struct ClearValues { float4 color; float depth; };
struct ClearFragment { float4 color [[color(0)]]; float depth [[depth(any)]]; };
vertex ClearOut h2_clear_vertex(uint vid [[vertex_id]]) {
    float2 corner = float2((vid << 1) & 2, vid & 2);
    ClearOut o;
    o.position = float4(corner * 2.0 - 1.0, 0.0, 1.0);
    return o;
}
fragment ClearFragment h2_clear_fragment(ClearOut in [[stage_in]], constant ClearValues &v [[buffer(0)]]) {
    ClearFragment o;
    o.color = v.color;
    o.depth = v.depth;
    return o;
}
)msl";
        NSError* failure = nil;
        id<MTLLibrary> library = [device_ newLibraryWithSource:@(source) options:nil error:&failure];
        if (!library) {
            error = std::string("clear shader: ") + failure.localizedDescription.UTF8String;
            return false;
        }
        for (int mask = 0; mask < 16; ++mask) {
            MTLRenderPipelineDescriptor* descriptor = [[MTLRenderPipelineDescriptor alloc] init];
            descriptor.vertexFunction = [library newFunctionWithName:@"h2_clear_vertex"];
            descriptor.fragmentFunction = [library newFunctionWithName:@"h2_clear_fragment"];
            descriptor.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
            descriptor.colorAttachments[0].writeMask = static_cast<MTLColorWriteMask>(mask);
            descriptor.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float_Stencil8;
            descriptor.stencilAttachmentPixelFormat = MTLPixelFormatDepth32Float_Stencil8;
            id<MTLRenderPipelineState> state = [device_ newRenderPipelineStateWithDescriptor:descriptor error:&failure];
            if (!state) {
                error = std::string("clear pipeline: ") + failure.localizedDescription.UTF8String;
                return false;
            }
            clear_pipelines_[mask] = state;
        }
        return true;
    }

    bool build_present_pipeline(std::string& error) {
        static const char* source = R"msl(
#include <metal_stdlib>
using namespace metal;
struct PresentOut { float4 position [[position]]; float2 uv; };
vertex PresentOut h2_present_vertex(uint vid [[vertex_id]], constant float4 &rect [[buffer(0)]]) {
    float2 corner = float2((vid << 1) & 2, vid & 2);
    PresentOut o;
    o.uv = corner;
    float2 ndc = rect.xy + corner * rect.zw;
    o.position = float4(ndc, 0.0, 1.0);
    return o;
}
fragment float4 h2_present_fragment(PresentOut in [[stage_in]], texture2d<float> frame [[texture(0)]]) {
    constexpr sampler linear_sampler(filter::linear, address::clamp_to_edge);
    return frame.sample(linear_sampler, in.uv);
}
)msl";
        NSError* failure = nil;
        id<MTLLibrary> library = [device_ newLibraryWithSource:@(source) options:nil error:&failure];
        if (!library) {
            error = std::string("present shader: ") + failure.localizedDescription.UTF8String;
            return false;
        }
        MTLRenderPipelineDescriptor* descriptor = [[MTLRenderPipelineDescriptor alloc] init];
        descriptor.vertexFunction = [library newFunctionWithName:@"h2_present_vertex"];
        descriptor.fragmentFunction = [library newFunctionWithName:@"h2_present_fragment"];
        descriptor.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
        present_pipeline_ = [device_ newRenderPipelineStateWithDescriptor:descriptor error:&failure];
        if (!present_pipeline_) {
            error = std::string("present pipeline: ") + failure.localizedDescription.UTF8String;
            return false;
        }
        return true;
    }

    void encode_present(id<CAMetalDrawable> drawable) {
        MTLRenderPassDescriptor* pass = [MTLRenderPassDescriptor renderPassDescriptor];
        pass.colorAttachments[0].texture = drawable.texture;
        pass.colorAttachments[0].loadAction = MTLLoadActionClear;
        pass.colorAttachments[0].clearColor = MTLClearColorMake(0, 0, 0, 1);
        pass.colorAttachments[0].storeAction = MTLStoreActionStore;
        id<MTLRenderCommandEncoder> encoder = [command_buffer_ renderCommandEncoderWithDescriptor:pass];
        /* letterbox the game's 4:3 */
        const double target_w = drawable.texture.width;
        const double target_h = drawable.texture.height;
        const double aspect = static_cast<double>(kGuestWidth) / kGuestHeight;
        double w = target_w, h = target_w / aspect;
        if (h > target_h) {
            h = target_h;
            w = target_h * aspect;
        }
        /* the full-screen triangle covers 2x the rect: scale the rect to it */
        const float rect[4] = {static_cast<float>(-w / target_w), static_cast<float>(h / target_h),
                               static_cast<float>(2.0 * w / target_w), static_cast<float>(-2.0 * h / target_h)};
        [encoder setRenderPipelineState:present_pipeline_];
        [encoder setVertexBytes:rect length:sizeof(rect) atIndex:0];
        [encoder setFragmentTexture:color_ atIndex:0];
        [encoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:3];
        [encoder endEncoding];
    }

    void begin_command_buffer() {
        if (!command_buffer_)
            command_buffer_ = [queue_ commandBuffer];
    }

    void begin_encoder() {
        begin_command_buffer();
        if (encoder_)
            return;
        Target& t = *current_;
        MTLRenderPassDescriptor* pass = [MTLRenderPassDescriptor renderPassDescriptor];
        pass.colorAttachments[0].texture = t.color;
        pass.colorAttachments[0].loadAction =
            t.pending_color_clear ? MTLLoadActionClear : (t.has_content ? MTLLoadActionLoad : MTLLoadActionClear);
        pass.colorAttachments[0].clearColor = t.pending_color_clear ? t.clear_color : MTLClearColorMake(0, 0, 0, 1);
        pass.colorAttachments[0].storeAction = MTLStoreActionStore;
        DepthBuffer& d = current_depth_ ? *current_depth_ : t.own_depth;
        pass.depthAttachment.texture = d.texture;
        pass.depthAttachment.loadAction =
            d.pending_depth_clear || !d.has_content ? MTLLoadActionClear : MTLLoadActionLoad;
        pass.depthAttachment.clearDepth = d.pending_depth_clear ? d.clear_depth : 1.0;
        pass.depthAttachment.storeAction = MTLStoreActionStore;
        pass.stencilAttachment.texture = d.texture;
        pass.stencilAttachment.loadAction =
            d.pending_stencil_clear || !d.has_content ? MTLLoadActionClear : MTLLoadActionLoad;
        pass.stencilAttachment.clearStencil = d.pending_stencil_clear ? d.clear_stencil : 0;
        pass.stencilAttachment.storeAction = MTLStoreActionStore;
        encoder_ = [command_buffer_ renderCommandEncoderWithDescriptor:pass];
        t.pending_color_clear = d.pending_depth_clear = d.pending_stencil_clear = false;
        t.has_content = d.has_content = true;
    }

    void end_encoder() {
        if (encoder_) {
            [encoder_ endEncoding];
            encoder_ = nil;
        }
    }

    /* per-frame buffer space */
    /* per-frame buffer space: a frame that outgrows one buffer moves on to
       the next (made on first need, at least as large as the request) */
    bool frame_allocate(std::size_t bytes, __strong id<MTLBuffer>& buffer, std::size_t& offset, void*& pointer) {
        bytes = (bytes + 255) & ~std::size_t(255);
        if (frame_offset_ + bytes > frame_buffers_[frame_buffer_].length) {
            ++frame_buffer_;
            frame_offset_ = 0;
            if (frame_buffer_ == frame_buffers_.size() || frame_buffers_[frame_buffer_].length < bytes) {
                id<MTLBuffer> grown = [device_ newBufferWithLength:std::max(kFrameBufferBytes, bytes)
                                                           options:MTLResourceStorageModeShared];
                if (!grown)
                    return false;
                if (frame_buffer_ == frame_buffers_.size())
                    frame_buffers_.push_back(grown);
                else
                    frame_buffers_[frame_buffer_] = grown;
            }
        }
        buffer = frame_buffers_[frame_buffer_];
        offset = frame_offset_;
        pointer = static_cast<std::uint8_t*>(buffer.contents) + offset;
        frame_offset_ += bytes;
        return true;
    }

    /* the vertex indices the draw uses, in the order the primitive gives them */
    bool gather_indices(const h2_gpu_draw& packet, std::vector<std::uint32_t>& order) {
        order.resize(packet.vertex_count);
        if (packet.index_data) {
            const auto* indices = guest<std::uint16_t>(packet.index_data);
            if (!indices)
                return false;
            for (std::uint32_t i = 0; i < packet.vertex_count; ++i)
                order[i] = indices[i] + packet.first_vertex;
        } else {
            for (std::uint32_t i = 0; i < packet.vertex_count; ++i)
                order[i] = packet.first_vertex + i;
        }
        return true;
    }

    /* the primitive Metal draws and the vertex sequence for it */
    MTLPrimitiveType convert_primitive(std::uint32_t primitive, const std::vector<std::uint32_t>& in,
                                       std::vector<std::uint32_t>& out) {
        out.clear();
        const std::size_t n = in.size();
        switch (primitive) {
        case H2_GPU_POINTS:
            out = in;
            return MTLPrimitiveTypePoint;
        case H2_GPU_LINES:
            out = in;
            return MTLPrimitiveTypeLine;
        case H2_GPU_LINE_STRIP:
            out = in;
            return MTLPrimitiveTypeLineStrip;
        case H2_GPU_LINE_LOOP:
            out = in;
            if (n)
                out.push_back(in[0]);
            return MTLPrimitiveTypeLineStrip;
        case H2_GPU_TRIANGLE_STRIP:
            out = in;
            return MTLPrimitiveTypeTriangleStrip;
        case H2_GPU_TRIANGLE_FAN:
        case H2_GPU_POLYGON:
            for (std::size_t i = 1; i + 1 < n; ++i) {
                out.push_back(in[0]);
                out.push_back(in[i]);
                out.push_back(in[i + 1]);
            }
            return MTLPrimitiveTypeTriangle;
        case H2_GPU_QUADS:
            for (std::size_t i = 0; i + 3 < n; i += 4) {
                out.insert(out.end(), {in[i], in[i + 1], in[i + 2], in[i], in[i + 2], in[i + 3]});
            }
            return MTLPrimitiveTypeTriangle;
        case H2_GPU_QUAD_STRIP:
            for (std::size_t i = 0; i + 3 < n; i += 2) {
                out.insert(out.end(), {in[i], in[i + 1], in[i + 3], in[i], in[i + 3], in[i + 2]});
            }
            return MTLPrimitiveTypeTriangle;
        default:
            out = in;
            return MTLPrimitiveTypeTriangle;
        }
    }

    /* the 16 registers of vertices 0..count-1 */
    bool fetch_vertices(const h2_gpu_draw& packet, std::uint32_t count, float* out) {
        if (packet.vertex_source == H2_GPU_VERTICES_INLINE) {
            const auto* source = guest<float>(packet.inline_vertices);
            if (!source || count > packet.inline_vertex_count)
                return false;
            std::memcpy(out, source, static_cast<std::size_t>(count) * 16 * 16);
            return true;
        }
        /* each array's whole span checked once: the guest memory checks lock,
           and a check per element of every vertex dominated the frame */
        enum : std::uint8_t { kDefault, kChecked, kPerElement };
        std::uint8_t mode[16];
        const std::uint8_t* base[16] = {};
        for (int index = 0; index < 16; ++index) {
            const h2_gpu_vertex_array& array = packet.vertex_arrays[index];
            mode[index] = kDefault;
            if ((array.format >> 4) == 0 || !array.address || !count)
                continue;
            const std::size_t span = static_cast<std::size_t>(count - 1) * array.stride + element_bytes(array.format);
            const auto* first = guest<std::uint8_t>(array.address);
            if (first && arena_.host_range_accessible(first, span, false)) {
                mode[index] = kChecked;
                base[index] = first;
            } else {
                mode[index] = kPerElement;
            }
        }
        for (std::uint32_t vertex = 0; vertex < count; ++vertex) {
            float* registers = out + static_cast<std::size_t>(vertex) * 64;
            for (int index = 0; index < 16; ++index) {
                const h2_gpu_vertex_array& array = packet.vertex_arrays[index];
                float* value = registers + index * 4;
                if (mode[index] == kChecked) {
                    decode_attribute(base[index] + static_cast<std::size_t>(vertex) * array.stride, array.format, value);
                    continue;
                }
                if (mode[index] == kDefault) {
                    std::memcpy(value, packet.vertex_defaults[index], 16);
                    continue;
                }
                const std::uint32_t address = array.address + vertex * array.stride;
                const auto* element = guest<std::uint8_t>(address);
                if (!element || !arena_.host_range_accessible(element, element_bytes(array.format), false)) {
                    std::memcpy(value, packet.vertex_defaults[index], 16);
                    continue;
                }
                decode_attribute(element, array.format, value);
            }
        }
        return true;
    }

    std::string vertex_source(const h2_gpu_draw& packet, std::uint64_t& key, bool& ok) {
        ok = true;
        if (packet.vertex_mode != H2_GPU_VERTEX_PROGRAM) {
            key = 1;
            return kScreenVertexProgram;
        }
        const auto* program = guest<std::uint32_t>(packet.vertex_program);
        if (!program || packet.vertex_program_start >= packet.vertex_program_length) {
            ok = false;
            return {};
        }
        /* the program runs from its start to the first instruction flagged
           final: hash the slots from the start (the translator stops at the
           final flag, so later garbage does not matter, but hashing it would
           only cost cache misses, so stop at the final flag here too) */
        const std::size_t start = packet.vertex_program_start;
        std::size_t end = start;
        while (end < packet.vertex_program_length) {
            const std::uint32_t* instruction = program + end * 4;
            ++end;
            if (instruction[3] & 1)
                break;
        }
        key = hash_bytes(program + start * 4, (end - start) * 16, 0x9e3779b97f4a7c15ull);
        auto found = vertex_cache_.find(key);
        if (found == vertex_cache_.end()) {
            VertexProgramInfo info = translate_vertex_program(program, packet.vertex_program_length, start);
            if (!info.error.empty()) {
                std::fprintf(stderr, "h2-gpu: vertex program at %zu: %s\n", start, info.error.c_str());
                ok = false;
                return {};
            }
            found = vertex_cache_.emplace(key, info.source).first;
        }
        /* the frame trace names each draw's vertex program and saves its translation */
        if (tracing_frame()) {
            std::fprintf(stderr, "h2-frame:   vs %016llx (%zu instructions)\n",
                         static_cast<unsigned long long>(key), end - start);
            if (const char* dump_dir = std::getenv("H2_TRACE_DUMP")) {
                char path[512];
                std::snprintf(path, sizeof(path), "%s/vs-%016llx.metal", dump_dir,
                              static_cast<unsigned long long>(key));
                if (FILE* file = std::fopen(path, "w")) {
                    std::fwrite(found->second.data(), 1, found->second.size(), file);
                    std::fclose(file);
                }
            }
        }
        return found->second;
    }

    std::string pixel_source(const h2_gpu_draw& packet, const PixelShaderContext& context, std::uint64_t& key,
                             bool& ok) {
        ok = true;
        if (!packet.has_pixel_shader) {
            const bool texture0 = packet.textures[0].header != 0;
            key = texture0 ? 2 : 3;
            return fixed_function_pixel_shader(texture0);
        }
        key = hash_bytes(packet.pixel_shader, sizeof(packet.pixel_shader));
        key = hash_bytes(&context, sizeof(context), key);
        auto found = pixel_cache_.find(key);
        if (found == pixel_cache_.end()) {
            std::uint32_t definition[60];
            std::memcpy(definition, packet.pixel_shader, sizeof(definition));
            PixelShaderInfo info = translate_pixel_shader(definition, context);
            if (!info.error.empty()) {
                std::fprintf(stderr, "h2-gpu: pixel shader: %s\n", info.error.c_str());
                ok = false;
                return {};
            }
            found = pixel_cache_.emplace(key, info.source).first;
        }
        /* the frame trace names each draw's pixel shader and saves its translation */
        if (tracing_frame()) {
            std::fprintf(stderr, "h2-frame:   fs %016llx\n", static_cast<unsigned long long>(key));
            if (const char* dump_dir = std::getenv("H2_TRACE_DUMP")) {
                char path[512];
                std::snprintf(path, sizeof(path), "%s/fs-%016llx.metal", dump_dir,
                              static_cast<unsigned long long>(key));
                if (FILE* file = std::fopen(path, "w")) {
                    std::fwrite(found->second.data(), 1, found->second.size(), file);
                    std::fclose(file);
                }
            }
        }
        return found->second;
    }

    id<MTLRenderPipelineState> pipeline(const h2_gpu_draw& packet, const PixelShaderContext& context) {
        bool ok = false;
        PipelineKey key;
        const std::string vertex = vertex_source(packet, key.vertex, ok);
        if (!ok)
            return nil;
        const std::string fragment = pixel_source(packet, context, key.fragment, ok);
        if (!ok)
            return nil;
        key.blend = (packet.blend_enable ? 1u : 0u) | (packet.color_write_mask << 1) |
                    (packet.blend_op_color << 8) | (packet.blend_op_alpha << 12);
        key.blend_factors = packet.blend_src_color | (packet.blend_dst_color << 8) |
                            (packet.blend_src_alpha << 16) | (packet.blend_dst_alpha << 24);
        auto found = pipelines_.find(key);
        if (found != pipelines_.end())
            return found->second;

        const std::uint64_t library_key = key.vertex * 1000003ull ^ key.fragment;
        id<MTLLibrary> library = nil;
        auto cached = libraries_.find(library_key);
        if (cached != libraries_.end()) {
            library = cached->second;
        } else {
            std::string source = std::string(kShaderPrelude) + vertex + "\n" + fragment + "\n" + kEntryPoints;
            NSError* failure = nil;
            MTLCompileOptions* options = [[MTLCompileOptions alloc] init];
            options.mathMode = MTLMathModeSafe;
            options.preserveInvariance = YES;
            library = [device_ newLibraryWithSource:@(source.c_str()) options:options error:&failure];
            if (!library) {
                std::fprintf(stderr, "h2-gpu: shader compile failed: %s\n",
                             failure.localizedDescription.UTF8String);
                if (std::getenv("H2_GPU_DUMP_FAILED")) {
                    if (FILE* dump = std::fopen("h2-gpu-failed.metal", "w")) {
                        std::fwrite(source.data(), 1, source.size(), dump);
                        std::fclose(dump);
                    }
                }
                libraries_.emplace(library_key, nil);
                return nil;
            }
            libraries_.emplace(library_key, library);
        }
        if (!library)
            return nil;
        MTLRenderPipelineDescriptor* descriptor = [[MTLRenderPipelineDescriptor alloc] init];
        descriptor.vertexFunction = [library newFunctionWithName:@"h2_vertex_main"];
        descriptor.fragmentFunction = [library newFunctionWithName:@"h2_fragment_main"];
        descriptor.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
        descriptor.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float_Stencil8;
        descriptor.stencilAttachmentPixelFormat = MTLPixelFormatDepth32Float_Stencil8;
        MTLRenderPipelineColorAttachmentDescriptor* attachment = descriptor.colorAttachments[0];
        attachment.blendingEnabled = packet.blend_enable ? YES : NO;
        attachment.sourceRGBBlendFactor = blend_factor(packet.blend_src_color);
        attachment.destinationRGBBlendFactor = blend_factor(packet.blend_dst_color);
        attachment.rgbBlendOperation = blend_operation(packet.blend_op_color);
        attachment.sourceAlphaBlendFactor = blend_factor(packet.blend_src_alpha);
        attachment.destinationAlphaBlendFactor = blend_factor(packet.blend_dst_alpha);
        attachment.alphaBlendOperation = blend_operation(packet.blend_op_alpha);
        MTLColorWriteMask mask = MTLColorWriteMaskNone;
        if (packet.color_write_mask & 1) mask |= MTLColorWriteMaskRed;
        if (packet.color_write_mask & 2) mask |= MTLColorWriteMaskGreen;
        if (packet.color_write_mask & 4) mask |= MTLColorWriteMaskBlue;
        if (packet.color_write_mask & 8) mask |= MTLColorWriteMaskAlpha;
        attachment.writeMask = mask;
        NSError* failure = nil;
        id<MTLRenderPipelineState> state = [device_ newRenderPipelineStateWithDescriptor:descriptor error:&failure];
        if (!state)
            std::fprintf(stderr, "h2-gpu: pipeline failed: %s\n", failure.localizedDescription.UTF8String);
        else
            ++stats_.pipelines;
        pipelines_.emplace(key, state);
        return state;
    }

    id<MTLDepthStencilState> depth_stencil(const h2_gpu_draw& packet) {
        const std::uint64_t key = (packet.depth_test_enable ? 1ull : 0ull) | (packet.depth_write_enable ? 2ull : 0ull) |
                                  (static_cast<std::uint64_t>(packet.depth_func) << 2) |
                                  (packet.stencil_enable ? 1ull << 5 : 0ull) |
                                  (static_cast<std::uint64_t>(packet.stencil_func) << 6) |
                                  (static_cast<std::uint64_t>(packet.stencil_fail) << 9) |
                                  (static_cast<std::uint64_t>(packet.stencil_depth_fail) << 12) |
                                  (static_cast<std::uint64_t>(packet.stencil_pass) << 15) |
                                  (static_cast<std::uint64_t>(packet.stencil_mask & 0xff) << 18) |
                                  (static_cast<std::uint64_t>(packet.stencil_write_mask & 0xff) << 26);
        auto found = depth_states_.find(key);
        if (found != depth_states_.end())
            return found->second;
        MTLDepthStencilDescriptor* descriptor = [[MTLDepthStencilDescriptor alloc] init];
        descriptor.depthCompareFunction =
            packet.depth_test_enable ? compare_function(packet.depth_func) : MTLCompareFunctionAlways;
        descriptor.depthWriteEnabled = packet.depth_test_enable && packet.depth_write_enable;
        if (packet.stencil_enable) {
            MTLStencilDescriptor* stencil = [[MTLStencilDescriptor alloc] init];
            stencil.stencilCompareFunction = compare_function(packet.stencil_func);
            stencil.stencilFailureOperation = stencil_operation(packet.stencil_fail);
            stencil.depthFailureOperation = stencil_operation(packet.stencil_depth_fail);
            stencil.depthStencilPassOperation = stencil_operation(packet.stencil_pass);
            stencil.readMask = packet.stencil_mask & 0xff;
            stencil.writeMask = packet.stencil_write_mask & 0xff;
            descriptor.frontFaceStencil = stencil;
            descriptor.backFaceStencil = stencil;
        }
        id<MTLDepthStencilState> state = [device_ newDepthStencilStateWithDescriptor:descriptor];
        depth_states_.emplace(key, state);
        return state;
    }

    id<MTLSamplerState> sampler(const h2_gpu_texture& texture, bool linear) {
        const std::uint64_t key = texture.address_u | (texture.address_v << 3) | (texture.address_w << 6) |
                                  (texture.mag_filter << 9) | (texture.min_filter << 11) |
                                  (texture.mip_filter << 13) | (linear ? 1u << 15 : 0u) |
                                  (static_cast<std::uint64_t>(texture.border_color) << 16);
        auto found = samplers_.find(key);
        if (found != samplers_.end())
            return found->second;
        MTLSamplerDescriptor* descriptor = [[MTLSamplerDescriptor alloc] init];
        const bool mag_linear = texture.mag_filter == H2_GPU_FILTER_LINEAR;
        const bool min_linear = texture.min_filter == H2_GPU_FILTER_LINEAR;
        if (linear) {
            /* NV2A linear textures take coordinates in texels */
            descriptor.normalizedCoordinates = NO;
            const MTLSamplerMinMagFilter filter = mag_linear ? MTLSamplerMinMagFilterLinear : MTLSamplerMinMagFilterNearest;
            descriptor.minFilter = filter;
            descriptor.magFilter = filter;
            descriptor.mipFilter = MTLSamplerMipFilterNotMipmapped;
        } else {
            descriptor.minFilter = min_linear ? MTLSamplerMinMagFilterLinear : MTLSamplerMinMagFilterNearest;
            descriptor.magFilter = mag_linear ? MTLSamplerMinMagFilterLinear : MTLSamplerMinMagFilterNearest;
            descriptor.mipFilter = texture.mip_filter == H2_GPU_FILTER_NONE ? MTLSamplerMipFilterNotMipmapped
                                   : texture.mip_filter == H2_GPU_FILTER_POINT ? MTLSamplerMipFilterNearest
                                                                               : MTLSamplerMipFilterLinear;
        }
        descriptor.sAddressMode = address_mode(texture.address_u, linear);
        descriptor.tAddressMode = address_mode(texture.address_v, linear);
        descriptor.rAddressMode = address_mode(texture.address_w, linear);
        descriptor.borderColor = (texture.border_color >> 24) ? MTLSamplerBorderColorOpaqueWhite
                                                              : MTLSamplerBorderColorTransparentBlack;
        id<MTLSamplerState> state = [device_ newSamplerStateWithDescriptor:descriptor];
        samplers_.emplace(key, state);
        return state;
    }

    /* a hash of the texels, to notice textures the game rewrites (its text
       cache, render-to-memory effects): every word up to 1 MB, a sample of
       larger ones */
    std::uint64_t content_hash(const std::uint8_t* data, std::size_t size) {
        std::uint64_t hash = size;
        const std::size_t step = size <= (1u << 20) ? 8 : std::max<std::size_t>(64, size / 4096);
        for (std::size_t offset = 0; offset + 8 <= size; offset += step) {
            std::uint64_t word;
            std::memcpy(&word, data + offset, 8);
            hash = (hash ^ word) * 1099511628211ull;
        }
        return hash;
    }

    DepthBuffer make_depth(int width, int height) {
        DepthBuffer buffer;
        MTLTextureDescriptor* descriptor = [MTLTextureDescriptor
            texture2DDescriptorWithPixelFormat:MTLPixelFormatDepth32Float_Stencil8 width:width height:height mipmapped:NO];
        descriptor.usage = MTLTextureUsageRenderTarget;
        descriptor.storageMode = MTLStorageModePrivate;
        buffer.texture = [device_ newTextureWithDescriptor:descriptor];
        buffer.width = width;
        buffer.height = height;
        return buffer;
    }

    /* the depth buffer a pass into `target` uses: the one for the game's depth
       surface (guest address of its header), shared by every target drawn with
       it; the target's own when no surface is bound or the sizes differ */
    DepthBuffer* depth_for(Target& target, std::uint32_t surface) {
        if (!surface)
            return &target.own_depth;
        if (&target == &back_)
            back_depth_surface_ = surface;
        auto found = depths_.find(surface);
        if (found == depths_.end()) {
            DepthBuffer buffer = make_depth(target.width, target.height);
            if (!buffer.texture)
                return &target.own_depth;
            found = depths_.emplace(surface, buffer).first;
        }
        DepthBuffer& shared = found->second;
        if (shared.width != target.width || shared.height != target.height) {
            if (trace_gpu())
                std::fprintf(stderr, "h2-gpu: depth surface %08x is %dx%d, target %dx%d: not shared\n", surface,
                             shared.width, shared.height, target.width, target.height);
            return &target.own_depth;
        }
        return &shared;
    }

    /* makes `target` with `depth` the destination of the next pass */
    void bind(Target* target, DepthBuffer* depth) {
        if (target == current_ && depth == current_depth_)
            return;
        end_encoder();
        current_ = target;
        current_depth_ = depth;
    }

    /* a target's pixels as a texture: its pending clears done, and a copy
       when it is the target being drawn into */
    id<MTLTexture> sampleable(Target& target) {
        if (target.pending_color_clear) {
            Target* drawing = current_;
            DepthBuffer* drawing_depth = current_depth_;
            end_encoder();
            current_ = &target;
            current_depth_ = &target.own_depth;
            begin_encoder();
            end_encoder();
            current_ = drawing;
            current_depth_ = drawing_depth;
        }
        if (&target != current_)
            return target.color;
        end_encoder();
        begin_command_buffer();
        if (!target.snapshot || target.snapshot.width != target.color.width ||
            target.snapshot.height != target.color.height) {
            MTLTextureDescriptor* descriptor = [MTLTextureDescriptor
                texture2DDescriptorWithPixelFormat:target.color.pixelFormat
                                             width:target.color.width
                                            height:target.color.height
                                         mipmapped:NO];
            descriptor.usage = MTLTextureUsageShaderRead;
            descriptor.storageMode = MTLStorageModePrivate;
            target.snapshot = [device_ newTextureWithDescriptor:descriptor];
        }
        id<MTLBlitCommandEncoder> blit = [command_buffer_ blitCommandEncoder];
        [blit copyFromTexture:target.color toTexture:target.snapshot];
        [blit endEncoding];
        return target.snapshot;
    }

    /* H2_TRACE=textures: each texture (data, format, size) once, with what
       became of it, and a tally by outcome every 600 frames */
    static bool trace_textures() {
        static const bool on = [] {
            const char* trace = std::getenv("H2_TRACE");
            return trace && std::strstr(trace, "textures");
        }();
        return on;
    }
    void texture_census(const h2_gpu_texture& stage, const char* outcome, std::uint32_t bytes) {
        if (!trace_textures())
            return;
        const std::uint64_t key = (static_cast<std::uint64_t>(stage.data) << 32) ^ stage.format ^
                                  (static_cast<std::uint64_t>(stage.size) << 7);
        if (!census_seen_.insert(key ^ std::hash<std::string>{}(outcome)).second)
            return;
        ++census_counts_[outcome];
        std::fprintf(stderr, "h2-texture: %08x format %08x size %08x (%u bytes): %s\n", stage.data, stage.format,
                     stage.size, bytes, outcome);
        if (frame_ >= census_reported_ + 600) {
            census_reported_ = frame_;
            std::fprintf(stderr, "h2-texture: tally at frame %llu:", static_cast<unsigned long long>(frame_));
            for (const auto& [name, count] : census_counts_)
                std::fprintf(stderr, " %s %u;", name.c_str(), count);
            std::fprintf(stderr, "\n");
        }
    }
    std::unordered_set<std::uint64_t> census_seen_;
    std::map<std::string, std::uint32_t> census_counts_;
    std::uint64_t census_reported_ = 0;

    CachedTexture* texture(const h2_gpu_texture& stage) {
        if (!stage.header || !stage.data)
            return nullptr;
        auto rendered = targets_.find(stage.data);
        if (rendered != targets_.end() && &rendered->second != current_ &&
            guest_hash(rendered->second) != rendered->second.guest_hash) {
            /* the game wrote a texture over a target's old memory */
            if (trace_gpu())
                std::fprintf(stderr, "h2-gpu: render target %08x replaced by a texture\n", stage.data);
            render_textures_.erase(stage.data);
            targets_.erase(rendered);
            rendered = targets_.end();
        }
        Target* source = rendered != targets_.end() ? &rendered->second : nullptr;
        /* a texture over the back buffer's pixels samples what was drawn there */
        if (!source && stage.data == back_buffer_data_)
            source = &back_;
        if (source) {
            /* a surface the game drew into: sample what the GPU drew */
            CachedTexture& entry = render_textures_[stage.data];
            entry.texture = sampleable(*source);
            entry.kind = 2;
            TextureDescription description;
            entry.linear = describe_texture(stage.format, stage.size, description) ? description.linear : true;
            entry.texel_scale_x = static_cast<float>(source->width) / source->guest_width;
            entry.texel_scale_y = static_cast<float>(source->height) / source->guest_height;
            entry.is_signed = false;
            entry.last_frame = frame_;
            return &entry;
        }
        TextureDescription description;
        if (!describe_texture(stage.format, stage.size, description)) {
            texture_census(stage, "unsupported format", 0);
            return nullptr;
        }
        const auto* data = guest<std::uint8_t>(stage.data);
        if (!data || !arena_.host_range_accessible(data, description.total_size, false)) {
            texture_census(stage, "unreadable memory", description.total_size);
            return nullptr;
        }
        { /* H2_TRACE_TEXTURE=data (hex, or all) with H2_TRACE_DUMP=dir: that texture's raw
             guest bytes in the traced frame, named with its size and pitch, and its
             palette's 256 entries */
            static const char* want = std::getenv("H2_TRACE_TEXTURE");
            static std::set<std::uint32_t> dumped;
            const bool all = want && !std::strcmp(want, "all");
            if (want && tracing_frame() && (all || std::strtoul(want, nullptr, 16) == stage.data) &&
                dumped.insert(stage.data).second) {
                char path[256];
                std::snprintf(path, sizeof(path), "%s/tex-%08x-%ux%u-p%u.raw", std::getenv("H2_TRACE_DUMP"), stage.data,
                              description.width, description.height, description.pitch);
                if (FILE* f = std::fopen(path, "wb")) { std::fwrite(data, 1, description.total_size, f); std::fclose(f); }
                /* and the palette it is drawn with */
                if (const auto* palette = stage.palette ? guest<std::uint8_t>(stage.palette) : nullptr) {
                    std::snprintf(path, sizeof(path), "%s/pal-%08x.raw", std::getenv("H2_TRACE_DUMP"), stage.palette);
                    if (FILE* f = std::fopen(path, "wb")) { std::fwrite(palette, 1, 1024, f); std::fclose(f); }
                }
            }
        }
        const std::uint64_t key = (static_cast<std::uint64_t>(stage.data) << 32) ^ stage.format ^
                                  (static_cast<std::uint64_t>(stage.size) << 7) ^ stage.palette;
        CachedTexture& entry = textures_[key];
        /* a texture unchanged for a while is hashed once a frame: hashing
           every use dominated frames that draw the same textures many times;
           one written recently (glyph caches, dynamic images) still is
           hashed at every use */
        if (entry.texture && entry.hashed_frame == frame_ && frame_ - entry.changed_frame > 30) {
            entry.last_frame = frame_;
            return &entry;
        }
        const std::uint64_t content = content_hash(data, description.total_size) ^ stage.palette;
        entry.last_frame = frame_;
        entry.hashed_frame = frame_;
        if (entry.texture && entry.content == content)
            return &entry;
        entry.changed_frame = frame_;
        entry.content = content;
        entry.linear = description.linear;
        entry.is_signed = description.format && description.format->is_signed;
        entry.kind = description.kind == TextureKind::cube ? 6 : description.kind == TextureKind::volume ? 3 : 2;

        std::vector<std::uint32_t> palette;
        if (description.format && description.format->conversion == Conversion::p8_to_rgba8) {
            const auto* entries = guest<std::uint32_t>(stage.palette);
            if (entries)
                palette.assign(entries, entries + 256);
            else
                palette.assign(256, 0xffffffffu);
        }
        UploadImage first;
        if (!prepare_upload(description, std::span<const std::uint8_t>(data, description.total_size), 0, 0,
                            first, palette)) {
            texture_census(stage, "conversion failed", description.total_size);
            entry.texture = nil;
            return nullptr;
        }
        if (trace_textures()) {
            bool zero = true;
            for (std::uint32_t i = 0; i < description.total_size && zero; ++i)
                zero = data[i] == 0;
            texture_census(stage, zero ? "uploaded, ALL ZERO" : "uploaded", description.total_size);
        }
        const MTLPixelFormat format = static_cast<MTLPixelFormat>(first.format);
        MTLTextureDescriptor* descriptor = [[MTLTextureDescriptor alloc] init];
        descriptor.pixelFormat = format;
        descriptor.width = first.width;
        descriptor.height = first.height;
        const std::uint32_t levels = description.linear ? 1 : std::max<std::uint32_t>(1, description.levels);
        descriptor.mipmapLevelCount = levels;
        if (entry.kind == 6) {
            descriptor.textureType = MTLTextureTypeCube;
        } else if (entry.kind == 3) {
            descriptor.textureType = MTLTextureType3D;
            descriptor.depth = std::max<std::uint32_t>(1, first.depth);
        } else {
            descriptor.textureType = MTLTextureType2D;
        }
        descriptor.usage = MTLTextureUsageShaderRead;
        const SwizzleChannels& sw = first.swizzle;
        descriptor.swizzle = MTLTextureSwizzleChannelsMake(
            static_cast<MTLTextureSwizzle>(sw.red), static_cast<MTLTextureSwizzle>(sw.green),
            static_cast<MTLTextureSwizzle>(sw.blue), static_cast<MTLTextureSwizzle>(sw.alpha));
        id<MTLTexture> created = [device_ newTextureWithDescriptor:descriptor];
        if (!created) {
            entry.texture = nil;
            return nullptr;
        }
        const std::uint32_t faces = entry.kind == 6 ? 6 : 1;
        for (std::uint32_t face = 0; face < faces; ++face) {
            for (std::uint32_t level = 0; level < levels; ++level) {
                UploadImage image;
                if (face == 0 && level == 0) {
                    image = std::move(first);
                } else if (!prepare_upload(description, std::span<const std::uint8_t>(data, description.total_size),
                                           face, level, image, palette)) {
                    break;
                }
                if (image.bytes.empty())
                    break;
                const MTLRegion region = entry.kind == 3
                    ? MTLRegionMake3D(0, 0, 0, image.width, image.height, std::max<std::uint32_t>(1, image.depth))
                    : MTLRegionMake2D(0, 0, image.width, image.height);
                [created replaceRegion:region
                           mipmapLevel:level
                                 slice:face
                             withBytes:image.bytes.data()
                           bytesPerRow:image.bytes_per_row
                         bytesPerImage:entry.kind == 3 ? image.bytes_per_image : 0];
            }
        }
        if (!entry.texture)
            ++stats_.textures;
        entry.texture = created;
        return &entry;
    }

    /* the target behind a D3DSurface header (0 = the back buffer): its
       texels' address keys it, and its Format/Size words size it */
    Target* target_for(std::uint32_t header) {
        if (!header)
            return &back_;
        const auto* words = guest<std::uint32_t>(header);
        if (!words)
            return nullptr;
        const std::uint32_t data = words[1] | 0x80000000u;
        auto found = targets_.find(data);
        if (found != targets_.end()) {
            note_drawn(found->second);
            return &found->second;
        }
        TextureDescription description;
        int width = 0, height = 0;
        if (describe_texture(words[3], words[4], description)) {
            width = static_cast<int>(description.width);
            height = static_cast<int>(description.height);
        }
        if (width <= 0 || height <= 0 || width > 4096 || height > 4096)
            return nullptr;
        Target target;
        /* a screen-sized target is drawn at the back buffer's scale, so that it
           can share the back buffer's depth */
        const int scale = width == kGuestWidth && height == kGuestHeight ? scale_ : 1;
        MTLTextureDescriptor* color = [MTLTextureDescriptor
            texture2DDescriptorWithPixelFormat:MTLPixelFormatBGRA8Unorm width:width * scale height:height * scale
                                     mipmapped:NO];
        color.usage = MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead;
        color.storageMode = MTLStorageModePrivate;
        target.color = [device_ newTextureWithDescriptor:color];
        target.own_depth = make_depth(width * scale, height * scale);
        target.width = width * scale;
        target.height = height * scale;
        target.guest_width = width;
        target.guest_height = height;
        if (!target.color || !target.own_depth.texture)
            return nullptr;
        if (trace_gpu())
            std::fprintf(stderr, "h2-gpu: render target %08x %dx%d format %08x\n", data, width, height, words[3]);
        target.data = data;
        target.guest_bytes = description.total_size;
        Target& made = targets_.emplace(data, target).first->second;
        note_drawn(made);
        return &made;
    }

    std::uint64_t guest_hash(const Target& target) {
        const auto* bytes = guest<std::uint8_t>(target.data);
        if (!bytes || !target.guest_bytes || !arena_.host_range_accessible(bytes, target.guest_bytes, false))
            return 0;
        return content_hash(bytes, target.guest_bytes);
    }

    void note_drawn(Target& target) {
        if (target.hashed_frame == frame_)
            return;
        target.hashed_frame = frame_;
        target.guest_hash = guest_hash(target);
    }

    void trace_draw(const h2_gpu_draw& packet) {
        static int traced = 0;
        static const int limit = [] {
            const char* value = std::getenv("H2_TRACE_DRAWS");
            return value ? std::atoi(value) : 0;
        }();
        if (traced >= limit)
            return;
        ++traced;
        std::fprintf(stderr, "h2-gpu: draw prim %u count %u source %u mode %u start %u ps %u tex0 %08x fmt %08x size %08x\n",
                     packet.primitive, packet.vertex_count, packet.vertex_source, packet.vertex_mode,
                     packet.vertex_program_start, packet.has_pixel_shader, packet.textures[0].header,
                     packet.textures[0].format, packet.textures[0].size);
        if (packet.vertex_source == H2_GPU_VERTICES_INLINE) {
            const auto* v = guest<float>(packet.inline_vertices);
            for (std::uint32_t i = 0; v && i < std::min<std::uint32_t>(packet.inline_vertex_count, 4); ++i) {
                const float* r = v + i * 64;
                std::fprintf(stderr, "h2-gpu:   v%u v0 (%g %g %g %g) v3 (%g %g %g %g) v9 (%g %g %g %g)\n", i,
                             r[0], r[1], r[2], r[3], r[12], r[13], r[14], r[15], r[36], r[37], r[38], r[39]);
            }
        }
        const auto* c = guest<float>(packet.vertex_constants);
        for (int index : {10, 11, 177, 178, 179, 180, 183})
            if (c)
                std::fprintf(stderr, "h2-gpu:   c[%d] (%g %g %g %g)\n", index, c[index * 4], c[index * 4 + 1],
                             c[index * 4 + 2], c[index * 4 + 3]);
        std::fprintf(stderr, "h2-gpu:   viewport %g %g %g %g blend %u %u %u\n", packet.viewport_x, packet.viewport_y,
                     packet.viewport_width, packet.viewport_height, packet.blend_enable, packet.blend_src_color,
                     packet.blend_dst_color);
    }

    bool encode_draw(const h2_gpu_draw& packet) {
        trace_draw(packet);
        back_buffer_data_ = packet.back_buffer_data;
        std::vector<std::uint32_t> order;
        if (!gather_indices(packet, order)) {
            if (tracing_frame())
                std::fprintf(stderr, "h2-frame:   failed: indices\n");
            return false;
        }
        std::vector<std::uint32_t> sequence;
        const MTLPrimitiveType primitive = convert_primitive(packet.primitive, order, sequence);
        if (sequence.empty())
            return true;
        const std::uint32_t vertex_count = *std::max_element(sequence.begin(), sequence.end()) + 1;
        if (vertex_count > 1u << 20) {
            if (tracing_frame())
                std::fprintf(stderr, "h2-frame:   failed: vertex count\n");
            return false;
        }

        /* textures first: the pixel shader's translation depends on them */
        PixelShaderContext context;
        std::array<CachedTexture*, 4> bound{};
        for (int stage = 0; stage < 4; ++stage) {
            bound[stage] = texture(packet.textures[stage]);
            context.texture_dimension[stage] = bound[stage] ? static_cast<std::uint8_t>(bound[stage]->kind) : 0;
            context.texture_signed[stage] = bound[stage] && bound[stage]->is_signed;
        }
        id<MTLRenderPipelineState> state = pipeline(packet, context);
        if (!state) {
            if (tracing_frame())
                std::fprintf(stderr, "h2-frame:   failed: pipeline\n");
            return false;
        }

        id<MTLBuffer> vertex_buffer = nil;
        std::size_t vertex_offset = 0;
        void* vertex_pointer = nullptr;
        if (!frame_allocate(static_cast<std::size_t>(vertex_count) * 256, vertex_buffer, vertex_offset, vertex_pointer)) {
            if (tracing_frame())
                std::fprintf(stderr, "h2-frame:   failed: vertex space\n");
            return false;
        }
        if (!fetch_vertices(packet, vertex_count, static_cast<float*>(vertex_pointer))) {
            if (tracing_frame())
                std::fprintf(stderr, "h2-frame:   failed: fetch vertices\n");
            return false;
        }
        id<MTLBuffer> index_buffer = nil;
        std::size_t index_offset = 0;
        void* index_pointer = nullptr;
        if (!frame_allocate(sequence.size() * 4, index_buffer, index_offset, index_pointer)) {
            if (tracing_frame())
                std::fprintf(stderr, "h2-frame:   failed: index space\n");
            return false;
        }
        std::memcpy(index_pointer, sequence.data(), sequence.size() * 4);

        begin_encoder();
        [encoder_ setRenderPipelineState:state];
        [encoder_ setDepthStencilState:depth_stencil(packet)];
        [encoder_ setStencilReferenceValue:packet.stencil_ref & 0xff];
        [encoder_ setCullMode:packet.cull == H2_GPU_CULL_NONE ? MTLCullModeNone
                              : packet.cull == H2_GPU_CULL_CLOCKWISE ? MTLCullModeBack : MTLCullModeFront];
        /* the screen-space positions keep their winding; the NV2A culls
           clockwise faces for D3DCULL_CW, and Metal's y-up clip space flips
           window winding, so front is clockwise-in-window = counter-clockwise
           in clip space */
        [encoder_ setFrontFacingWinding:MTLWindingCounterClockwise];
        [encoder_ setTriangleFillMode:packet.fill_wireframe ? MTLTriangleFillModeLines : MTLTriangleFillModeFill];
        /* D3DRS_BLENDCOLOR, for the constant blend factors (the atmosphere pass
           adds its fog over dst * the constant) */
        const auto constant = unpack_argb(packet.blend_constant);
        [encoder_ setBlendColorRed:constant[0] green:constant[1] blue:constant[2] alpha:constant[3]];
        if (packet.depth_bias != 0.0f || packet.depth_slope_scale != 0.0f)
            [encoder_ setDepthBias:packet.depth_bias / kDepthUnits slopeScale:packet.depth_slope_scale clamp:0.0f];
        else
            [encoder_ setDepthBias:0.0f slopeScale:0.0f clamp:0.0f];

        const Target& target = *current_;
        float target_width = static_cast<float>(target.guest_width);
        float target_height = static_cast<float>(target.guest_height);
        if (current_ == &back_ && packet.target_width && packet.target_height) {
            target_width = static_cast<float>(packet.target_width);
            target_height = static_cast<float>(packet.target_height);
        }
        const int pass_width = target.width;
        const int pass_height = target.height;
        const float sx = pass_width / target_width;
        const float sy = pass_height / target_height;
        MTLViewport viewport = {0.0, 0.0, static_cast<double>(pass_width), static_cast<double>(pass_height), 0.0, 1.0};
        [encoder_ setViewport:viewport];
        /* the game's viewport bounds what it draws (NV2A clips to it) */
        double left = packet.viewport_x, top = packet.viewport_y;
        double right = packet.viewport_x + packet.viewport_width, bottom = packet.viewport_y + packet.viewport_height;
        if (packet.scissor_enable) {
            left = std::max<double>(left, packet.scissor_left);
            top = std::max<double>(top, packet.scissor_top);
            right = std::min<double>(right, packet.scissor_right);
            bottom = std::min<double>(bottom, packet.scissor_bottom);
        }
        if (right <= left || bottom <= top || packet.viewport_width == 0.0f) {
            left = top = 0;
            right = target_width;
            bottom = target_height;
        }
        MTLScissorRect scissor;
        scissor.x = static_cast<NSUInteger>(std::clamp(left * sx, 0.0, static_cast<double>(pass_width)));
        scissor.y = static_cast<NSUInteger>(std::clamp(top * sy, 0.0, static_cast<double>(pass_height)));
        scissor.width = static_cast<NSUInteger>(std::clamp(right * sx, 0.0, static_cast<double>(pass_width))) - scissor.x;
        scissor.height = static_cast<NSUInteger>(std::clamp(bottom * sy, 0.0, static_cast<double>(pass_height))) - scissor.y;
        if (scissor.width == 0 || scissor.height == 0)
            return true;
        [encoder_ setScissorRect:scissor];

        /* vertex inputs */
        [encoder_ setVertexBuffer:vertex_buffer offset:vertex_offset atIndex:0];
        const auto* constants = guest<float>(packet.vertex_constants);
        if (constants)
            [encoder_ setVertexBytes:constants length:192 * 16 atIndex:1];
        else {
            static const float zero[192 * 4] = {};
            [encoder_ setVertexBytes:zero length:sizeof(zero) atIndex:1];
        }
        struct {
            float window_to_ndc[4];
            float z_scale;
            float pad[3];
        } screen = {{2.0f / target_width, -2.0f / target_height, -1.0f, 1.0f},
                    packet.vertex_mode == H2_GPU_VERTEX_PROGRAM ? 1.0f / kDepthUnits : 1.0f,
                    {0, 0, 0}};
        [encoder_ setVertexBytes:&screen length:sizeof(screen) atIndex:2];

        /* pixel inputs */
        struct PixelConstants {
            float c0[8][4];
            float c1[8][4];
            float final_c0[4];
            float final_c1[4];
            float fog_color[4];
            float bump_env[4][4];
            float bump_luminance[4][4];
            float color_key[4][4];
        } k = {};
        if (packet.has_pixel_shader) {
            /* D3DPIXELSHADERDEF: PSConstant0[8] at word 10, PSConstant1[8] at
               18, PSFinalCombinerConstant0 at 43 and 1 at 44 */
            for (int stage = 0; stage < 8; ++stage) {
                const auto c0 = unpack_argb(packet.pixel_shader[10 + stage]);
                const auto c1 = unpack_argb(packet.pixel_shader[18 + stage]);
                std::memcpy(k.c0[stage], c0.data(), 16);
                std::memcpy(k.c1[stage], c1.data(), 16);
            }
            const auto f0 = unpack_argb(packet.pixel_shader[43]);
            const auto f1 = unpack_argb(packet.pixel_shader[44]);
            std::memcpy(k.final_c0, f0.data(), 16);
            std::memcpy(k.final_c1, f1.data(), 16);
        }
        const auto fog = unpack_argb(packet.fog_color);
        std::memcpy(k.fog_color, fog.data(), 16);
        for (int stage = 0; stage < 4; ++stage) {
            const auto key = unpack_argb(packet.textures[stage].color_key);
            std::memcpy(k.color_key[stage], key.data(), 16);
            k.bump_env[stage][0] = 1.0f;
            k.bump_env[stage][3] = 1.0f;
            k.bump_luminance[stage][0] = 1.0f;
        }
        [encoder_ setFragmentBytes:&k length:sizeof(k) atIndex:0];
        struct {
            std::uint32_t alpha_test;
            std::uint32_t alpha_func;
            float alpha_ref;
            std::uint32_t two_sided;
            float texel_scale[4][4];
        } fragment = {packet.alpha_test_enable, packet.alpha_func, packet.alpha_ref, packet.two_sided_lighting, {}};
        for (int stage = 0; stage < 4; ++stage) {
            const CachedTexture* entry = bound[stage];
            const bool scaled = entry && entry->linear && entry->kind == 2;
            fragment.texel_scale[stage][0] = scaled ? entry->texel_scale_x : 1.0f;
            fragment.texel_scale[stage][1] = scaled ? entry->texel_scale_y : 1.0f;
        }
        [encoder_ setFragmentBytes:&fragment length:sizeof(fragment) atIndex:1];
        for (int stage = 0; stage < 4; ++stage) {
            CachedTexture* entry = bound[stage];
            id<MTLTexture> as2d = placeholder_2d_, ascube = placeholder_cube_, as3d = placeholder_3d_;
            if (entry && entry->texture) {
                if (entry->kind == 6)
                    ascube = entry->texture;
                else if (entry->kind == 3)
                    as3d = entry->texture;
                else
                    as2d = entry->texture;
            }
            [encoder_ setFragmentTexture:as2d atIndex:stage];
            [encoder_ setFragmentTexture:ascube atIndex:4 + stage];
            [encoder_ setFragmentTexture:as3d atIndex:8 + stage];
            [encoder_ setFragmentSamplerState:entry ? sampler(packet.textures[stage], entry->linear) : placeholder_sampler_
                                      atIndex:stage];
        }
        [encoder_ drawIndexedPrimitives:primitive
                             indexCount:sequence.size()
                              indexType:MTLIndexTypeUInt32
                            indexBuffer:index_buffer
                      indexBufferOffset:index_offset];
        return true;
    }

    const h2::port::GuestArena& arena_;
    const int scale_;
    int width_ = 0;
    int height_ = 0;
    id<MTLDevice> device_ = nil;
    id<MTLCommandQueue> queue_ = nil;
    Target back_;
    std::unordered_map<std::uint32_t, Target> targets_;
    Target* current_ = nullptr;
    bool capture_window_ = false;
    bool window_kept_ = false;
    id<MTLBuffer> window_readback_ = nil;
    int window_width_ = 0, window_height_ = 0;
    DepthBuffer* current_depth_ = nullptr;
    std::unordered_map<std::uint32_t, DepthBuffer> depths_;   /* by the game's depth surface */
    std::uint32_t back_depth_surface_ = 0;
    id<MTLTexture> color_ = nil;   /* back_.color */
    id<MTLTexture> placeholder_2d_ = nil;
    id<MTLTexture> placeholder_cube_ = nil;
    id<MTLTexture> placeholder_3d_ = nil;
    id<MTLSamplerState> placeholder_sampler_ = nil;
    id<MTLRenderPipelineState> present_pipeline_ = nil;
    id<MTLRenderPipelineState> clear_pipelines_[16] = {};  /* by colour write mask */
    std::unordered_map<std::uint64_t, id<MTLDepthStencilState>> clear_depth_states_;
    std::vector<id<MTLBuffer>> frame_buffers_;
    std::size_t frame_buffer_ = 0;
    std::size_t frame_offset_ = 0;
    id<MTLBuffer> readback_ = nil;
    CAMetalLayer* layer_ = nil;
    id<MTLCommandBuffer> command_buffer_ = nil;
    id<MTLRenderCommandEncoder> encoder_ = nil;
    bool kept_ = false;
    std::uint64_t frame_ = 0;
    RendererStats stats_;
    int presented_ = 0;
    std::uint32_t back_buffer_data_ = 0;
    std::unordered_map<std::uint64_t, std::string> vertex_cache_;
    std::unordered_map<std::uint64_t, std::string> pixel_cache_;
    std::unordered_map<std::uint64_t, id<MTLLibrary>> libraries_;
    std::unordered_map<PipelineKey, id<MTLRenderPipelineState>, PipelineKeyHash> pipelines_;
    std::unordered_map<std::uint64_t, id<MTLDepthStencilState>> depth_states_;
    std::unordered_map<std::uint64_t, id<MTLSamplerState>> samplers_;
    std::unordered_map<std::uint64_t, CachedTexture> textures_;
    std::unordered_map<std::uint32_t, CachedTexture> render_textures_;
};

} // namespace

std::unique_ptr<Renderer> Renderer::create(const h2::port::GuestArena& arena, int scale, std::string& error) {
    auto renderer = std::make_unique<MetalRenderer>(arena, scale < 1 ? 1 : scale);
    if (!renderer->initialize(error))
        return nullptr;
    return renderer;
}

} // namespace h2::gpu
