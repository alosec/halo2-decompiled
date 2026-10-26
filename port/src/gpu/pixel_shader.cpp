/* Translates a D3DPIXELSHADERDEF (NV2A texture shader modes + register
 * combiners) into a Metal Shading Language function
 *
 *   float4 h2_pixel_shader(thread const H2PixelInput &in,
 *                          constant H2PixelConstants &k, H2Textures t)
 *
 * See shader_abi.hpp for the contract and pixel_shader.md for the design.
 *
 * Sources (all public; no SDK headers, no GPL emulator code):
 *   [RC]   OpenGL NV_register_combiners / NV_register_combiners2 extension
 *          specifications (Khronos registry): the combiner math, input
 *          mappings, output scale/bias, mux, final combiner, clamping.
 *   [TS]   OpenGL NV_texture_shader specification (Khronos registry): the
 *          texture shader operations, cull modes, dot-product chains.
 *   [XML]  Mesa classic nouveau `nv20_3d.xml.h` (MIT, generated from
 *          envytools rnndb `graph/nv20_3d.xml` + `graph/nv_3ddefs.xml`, MIT):
 *          RC_IN_ALPHA/RC_IN_RGB/RC_FINAL0/RC_FINAL1 input fields,
 *          RC_OUT_ALPHA/RC_OUT_RGB output fields, TEX_SHADER_OP (5 bits per
 *          stage), TEX_SHADER_CULL_MODE (4 bits per stage, 0 = GEQUAL,
 *          1 = LESS), TEX_SHADER_DOTMAPPING (4 bits per stage),
 *          TEX_SHADER_PREVIOUS (stage 2: bits 17:16, stage 3: bits 21:20).
 *   [WIKI] xboxdevwiki "NV2A/Pixel Combiner": the full texture-mode table
 *          0x00..0x12 (the nouveau headers only name the NV20 subset).
 *   [GAME] facts derived from the game's own definitions
 *          (src/unknown_020560.cpp): PSCombinerCount bit 12 = per-stage c0,
 *          bit 16 = per-stage c1 (function_020f30 writes count | 0x11000 and
 *          fills PSConstant0[1], [4], [7] for the stages that read c0).
 *   [INF]  inferred, not verified from a public register reference; each use
 *          says so.
 *
 * The D3D definition words carry the hardware register values unchanged as
 * far as can be checked (every game-written value decodes to sensible
 * combiner programs under the [XML] layout), so the decoder below reads them
 * as hardware words. */
#include "shader_abi.hpp"

#include <cmath>
#include <cstdio>
#include <sstream>

namespace h2::gpu {
namespace {

/* Word indices of D3DPIXELSHADERDEF (port/include/xdk/d3d8.h). */
constexpr int kAlphaInputs = 0;     /* [8] */
constexpr int kFinalABCD = 8;
constexpr int kFinalEFG = 9;
constexpr int kAlphaOutputs = 26;   /* [8] */
constexpr int kRGBInputs = 34;      /* [8] */
constexpr int kCompareMode = 42;    /* provisional placement */
constexpr int kRGBOutputs = 45;     /* [8] */
constexpr int kCombinerCount = 53;
constexpr int kTextureModes = 54;
constexpr int kDotMapping = 55;     /* provisional placement */
constexpr int kInputTexture = 56;   /* provisional placement */
/* PSConstant0/1 (10, 18), PSFinalCombinerConstant0/1 (43, 44), PSC0Mapping,
   PSC1Mapping and PSFinalCombinerConstants (57..59) are resolved by the
   backend into H2PixelConstants; the shader only chooses the slot. */

/* ---- register combiner encodings [XML] ---------------------------------- */

/* Register (variable) numbers, nv10_rc_variable. */
enum Reg : unsigned {
    R_ZERO = 0x0, R_C0 = 0x1, R_C1 = 0x2, R_FOG = 0x3, R_V0 = 0x4, R_V1 = 0x5,
    R_T0 = 0x8, R_T1 = 0x9, R_T2 = 0xa, R_T3 = 0xb, R_R0 = 0xc, R_R1 = 0xd,
    R_V1R0 = 0xe, /* SPARE0_PLUS_SECONDARY_COLOR, final combiner only */
    R_EF = 0xf,   /* E_TIMES_F, final combiner only */
};

/* Input mappings, nv10_rc_mapping (bits 7:5 of each 8-bit input field). */
enum Mapping : unsigned {
    M_UNSIGNED_IDENTITY = 0, M_UNSIGNED_INVERT = 1, M_EXPAND_NORMAL = 2,
    M_EXPAND_NEGATE = 3, M_HALF_BIAS_NORMAL = 4, M_HALF_BIAS_NEGATE = 5,
    M_SIGNED_IDENTITY = 6, M_SIGNED_NEGATE = 7,
};

/* Each input word (RC_IN_RGB, RC_IN_ALPHA, RC_FINAL0, RC_FINAL1) holds four
   8-bit fields: A (or E) in 31:24, B (F) in 23:16, C (G) in 15:8, D in 7:0.
   Within a field: bits 3:0 register, bit 4 component usage (RGB portion:
   0 = rgb, 1 = alpha; alpha portion and G: 0 = blue, 1 = alpha), bits 7:5
   mapping. [XML] */
struct InputField {
    unsigned reg;
    bool alpha;
    unsigned mapping;
};

InputField input_field(std::uint32_t word, int slot /* 0 = A ... 3 = D */) {
    const unsigned f = (word >> (24 - 8 * slot)) & 0xff;
    return {f & 0xf, ((f >> 4) & 1) != 0, (f >> 5) & 7};
}

/* RC_OUT_RGB / RC_OUT_ALPHA [XML]: CD output 3:0, AB output 7:4, sum output
   11:8, CD dot 12, AB dot 13, mux (instead of sum) 14, bias 15 (-0.5),
   scale 17:16 (1, 2, 4, 1/2).
   Bits 18 and 19 of the RGB word: CD and AB "blue to alpha", NV2A-only and
   absent from the NV20 headers. [INF] position chosen as the next free bits
   (19 = AB, 18 = CD); no game-written value in the repository sets them. */
struct OutputWord {
    unsigned cd, ab, sum;
    bool cd_dot, ab_dot, mux, bias;
    unsigned scale;
    bool cd_blue_to_alpha, ab_blue_to_alpha;
};

OutputWord output_word(std::uint32_t w) {
    OutputWord o;
    o.cd = w & 0xf;
    o.ab = (w >> 4) & 0xf;
    o.sum = (w >> 8) & 0xf;
    o.cd_dot = (w >> 12) & 1;
    o.ab_dot = (w >> 13) & 1;
    o.mux = (w >> 14) & 1;
    o.bias = (w >> 15) & 1;
    o.scale = (w >> 16) & 3;
    o.cd_blue_to_alpha = (w >> 18) & 1;
    o.ab_blue_to_alpha = (w >> 19) & 1;
    return o;
}

/* PSCombinerCount: bits 7:0 stage count (RC_ENABLE NUM_COMBINERS [XML]),
   bit 8 mux select: 1 = MSB of r0.a (spare0.a >= 0.5, the [RC] rule),
   0 = LSB of the 8-bit r0.a [INF]; bit 12 per-stage c0, bit 16 per-stage c1
   [GAME]. */
struct CombinerControl {
    unsigned count;
    bool mux_msb;
    bool unique_c0, unique_c1;
};

CombinerControl combiner_control(std::uint32_t w) {
    CombinerControl c;
    c.count = w & 0xff;
    if (c.count > 8) c.count = 8;
    c.mux_msb = (w >> 8) & 1;
    c.unique_c0 = (w >> 12) & 1;
    c.unique_c1 = (w >> 16) & 1;
    return c;
}

/* ---- texture shader encodings ------------------------------------------- */

/* PSTextureModes: 5 bits per stage, stage n at bits 5n+4:5n [XML]; values
   0x00..0x12 [WIKI] (0x01, 0x04, 0x05, 0x06, 0x09, 0x0a, 0x0f, 0x10, 0x11 also
   [XML]). */
enum TexMode : unsigned {
    TM_NONE = 0x00, TM_PROJECT2D = 0x01, TM_PROJECT3D = 0x02, TM_CUBEMAP = 0x03,
    TM_PASSTHRU = 0x04, TM_CLIPPLANE = 0x05, TM_BUMPENVMAP = 0x06,
    TM_BUMPENVMAP_LUM = 0x07, TM_BRDF = 0x08, TM_DOT_ST = 0x09,
    TM_DOT_ZW = 0x0a, TM_DOT_RFLCT_DIFF = 0x0b, TM_DOT_RFLCT_SPEC = 0x0c,
    TM_DOT_STR_3D = 0x0d, TM_DOT_STR_CUBE = 0x0e, TM_DPNDNT_AR = 0x0f,
    TM_DPNDNT_GB = 0x10, TM_DOTPRODUCT = 0x11, TM_DOT_RFLCT_SPEC_CONST = 0x12,
};

bool is_dot_mode(unsigned m) {
    switch (m) {
    case TM_DOT_ST: case TM_DOT_ZW: case TM_DOT_RFLCT_DIFF:
    case TM_DOT_RFLCT_SPEC: case TM_DOT_STR_3D: case TM_DOT_STR_CUBE:
    case TM_DOTPRODUCT: case TM_DOT_RFLCT_SPEC_CONST:
        return true;
    default:
        return false;
    }
}

/* Lowest stage each mode may occupy ([WIKI] table; [TS] chain rules). */
int min_stage(unsigned m) {
    switch (m) {
    case TM_NONE: case TM_PROJECT2D: case TM_PROJECT3D: case TM_CUBEMAP:
    case TM_PASSTHRU: case TM_CLIPPLANE:
        return 0;
    case TM_BUMPENVMAP: case TM_BUMPENVMAP_LUM: case TM_DPNDNT_AR:
    case TM_DPNDNT_GB: case TM_DOTPRODUCT:
        return 1;
    case TM_BRDF: case TM_DOT_ST: case TM_DOT_ZW: case TM_DOT_RFLCT_DIFF:
        return 2;
    case TM_DOT_RFLCT_SPEC: case TM_DOT_STR_3D: case TM_DOT_STR_CUBE:
    case TM_DOT_RFLCT_SPEC_CONST:
        return 3;
    default:
        return 99;
    }
}

/* ---- expression building with constant folding -------------------------- */

/* An expression of one combiner channel group (float3 for the RGB portion,
   float for the alpha portion). A literal is kept as a number so that
   products with zero or one fold away, which keeps the generated source
   readable and lets the tests check its structure. */
struct Expr {
    std::string code;
    bool literal = false;
    double value = 0.0;
    bool nonneg = false;   /* known >= 0 */
    bool unit = false;     /* known <= 1 */
};

std::string num(double v) {
    char buf[64];
    if (v == std::floor(v) && std::fabs(v) < 1e6)
        std::snprintf(buf, sizeof buf, "%.1f", v);
    else
        std::snprintf(buf, sizeof buf, "%.9g", v);
    return buf;
}

Expr lit(double v) {
    Expr e;
    e.literal = true;
    e.value = v;
    e.code = num(v);
    e.nonneg = v >= 0.0;
    e.unit = v <= 1.0;
    return e;
}

Expr var(std::string code, bool nonneg, bool unit) {
    Expr e;
    e.code = std::move(code);
    e.nonneg = nonneg;
    e.unit = unit;
    return e;
}

Expr mul(const Expr& a, const Expr& b) {
    if (a.literal && b.literal) return lit(a.value * b.value);
    if ((a.literal && a.value == 0.0) || (b.literal && b.value == 0.0)) return lit(0.0);
    if (a.literal && a.value == 1.0) return b;
    if (b.literal && b.value == 1.0) return a;
    Expr e = var("(" + a.code + " * " + b.code + ")", a.nonneg && b.nonneg,
                 a.nonneg && b.nonneg && a.unit && b.unit);
    return e;
}

Expr add(const Expr& a, const Expr& b) {
    if (a.literal && b.literal) return lit(a.value + b.value);
    if (a.literal && a.value == 0.0) return b;
    if (b.literal && b.value == 0.0) return a;
    return var("(" + a.code + " + " + b.code + ")", a.nonneg && b.nonneg, false);
}

Expr one_minus(const Expr& a) {
    if (a.literal) return lit(1.0 - a.value);
    return var("(1.0 - " + a.code + ")", a.unit, a.nonneg);
}

/* Input mapping functions of [RC] ("Table: input mappings"):
   unsigned identity max(0,e), unsigned invert 1-min(max(e,0),1),
   expand normal 2*max(0,e)-1, expand negate -2*max(0,e)+1,
   half bias normal max(0,e)-0.5, half bias negate -max(0,e)+0.5,
   signed identity e, signed negate -e. */
Expr apply_mapping(const Expr& x, unsigned mapping) {
    if (x.literal) {
        const double v = x.value;
        const double p = v > 0.0 ? v : 0.0;
        const double c = p > 1.0 ? 1.0 : p;
        switch (mapping) {
        case M_UNSIGNED_IDENTITY: return lit(p);
        case M_UNSIGNED_INVERT: return lit(1.0 - c);
        case M_EXPAND_NORMAL: return lit(2.0 * p - 1.0);
        case M_EXPAND_NEGATE: return lit(-2.0 * p + 1.0);
        case M_HALF_BIAS_NORMAL: return lit(p - 0.5);
        case M_HALF_BIAS_NEGATE: return lit(-p + 0.5);
        case M_SIGNED_IDENTITY: return lit(v);
        default: return lit(-v);
        }
    }
    const std::string p = x.nonneg ? x.code : "max(" + x.code + ", 0.0)";
    switch (mapping) {
    case M_UNSIGNED_IDENTITY:
        return var(p, true, x.unit);
    case M_UNSIGNED_INVERT:
        if (x.nonneg && x.unit) return var("(1.0 - " + x.code + ")", true, true);
        return var("(1.0 - saturate(" + x.code + "))", true, true);
    case M_EXPAND_NORMAL:
        return var("(2.0 * " + p + " - 1.0)", false, x.unit);
    case M_EXPAND_NEGATE:
        return var("(1.0 - 2.0 * " + p + ")", false, true);
    case M_HALF_BIAS_NORMAL:
        return var("(" + p + " - 0.5)", false, x.unit);
    case M_HALF_BIAS_NEGATE:
        return var("(0.5 - " + p + ")", false, true);
    case M_SIGNED_IDENTITY:
        return x;
    default:
        return var("(-" + x.code + ")", false, true);
    }
}

/* ---- the translator ------------------------------------------------------ */

class Translator {
public:
    Translator(const std::uint32_t (&d)[60], const PixelShaderContext& ctx)
        : d_(d), ctx_(ctx) {}

    PixelShaderInfo run();

private:
    const std::uint32_t (&d_)[60];
    const PixelShaderContext& ctx_;
    std::ostringstream o_;
    unsigned mode_[4] = {};
    bool tex_nonneg_[4] = {};
    bool reg_nonneg_[16] = {};   /* by register number, for the combiners */

    unsigned input_stage(int stage) const;
    std::string dot_source(int stage);
    void texture_stages();
    void sample_plain(int i, unsigned mode);
    void general_stage(int s, const CombinerControl& cc);
    void final_combiner();
    Expr read(unsigned reg, bool alpha_usage, bool alpha_portion, int stage,
              bool final, bool unique_c0, bool unique_c1);
};

/* Which earlier stage's texture a stage reads (bump env map, dependent
   reads, dot products): TEX_SHADER_PREVIOUS [XML]. Stage 1 always reads t0;
   stage 2 reads the stage in bits 17:16, stage 3 the one in bits 21:20.
   The game's definitions store this register as PSInputTexture
   (src/unknown_020560.cpp writes it as 0: every stage reads t0). */
unsigned Translator::input_stage(int stage) const {
    const std::uint32_t w = d_[kInputTexture];
    unsigned in = 0;
    if (stage == 2) in = (w >> 16) & 3;
    if (stage == 3) in = (w >> 20) & 3;
    if (in >= static_cast<unsigned>(stage)) in = static_cast<unsigned>(stage) - 1;
    return in;
}

/* The vector a dot-product stage multiplies its texture coordinate with:
   the input texture's rgb, remapped per TEX_SHADER_DOTMAPPING (4 bits per
   stage, stage n at 4n+3:4n [XML]). The meaning of the values is [INF],
   following the D3D/GL naming of the eight NV2A dot mappings:
     0 unsigned 0..1 (as read)
     1 "-1..1, D3D": byte with a 128 bias, (b - 128) / 127
     2 "-1..1, GL": byte two's complement, b < 128 ? b : b - 256, / 127
     3 "-1..1": 2x - 1
     4 HILO "1": (x, y, 1)
     5..7 HILO hemisphere: (x, y, sqrt(1 - x^2 - y^2))
   When the context says the texture is a signed format the value is used as
   read (it is already -1..1). */
std::string Translator::dot_source(int stage) {
    const unsigned in = input_stage(stage);
    const std::string t = "t" + std::to_string(in);
    const unsigned m = (d_[kDotMapping] >> (4 * stage)) & 0xf;
    if (ctx_.texture_signed[in] && m <= 3) return t + ".rgb";
    switch (m & 7) {
    case 0: return t + ".rgb";
    case 1: return "((round(" + t + ".rgb * 255.0) - 128.0) / 127.0)";
    case 2: {
        const std::string b = "round(" + t + ".rgb * 255.0)";
        return "(select(" + b + ", " + b + " - 256.0, " + b + " >= 128.0) / 127.0)";
    }
    case 3: return "(2.0 * " + t + ".rgb - 1.0)";
    case 4: return "float3(" + t + ".rg, 1.0)";
    default:
        return "float3(" + t + ".rg, sqrt(saturate(1.0 - dot(" + t + ".rg, " + t + ".rg))))";
    }
}

void Translator::sample_plain(int i, unsigned mode) {
    /* [TS] TEXTURE_2D: (s/q, t/q); TEXTURE_3D: (s/q, t/q, r/q); CUBE_MAP:
       (s, t, r). The kind of texture actually bound wins over the mode when
       the context knows it (the D3D layer chooses the mode from the bound
       texture, so the two normally agree). */
    unsigned kind = mode == TM_PROJECT3D ? 3 : mode == TM_CUBEMAP ? 6 : 2;
    if (ctx_.texture_dimension[i] == 2 || ctx_.texture_dimension[i] == 3 ||
        ctx_.texture_dimension[i] == 6)
        kind = ctx_.texture_dimension[i];
    const std::string tc = "in.texcoord[" + std::to_string(i) + "]";
    const std::string n = std::to_string(i);
    if (kind == 6)
        o_ << "    t" << n << " = t.cube[" << n << "].sample(t.samplers[" << n << "], " << tc << ".xyz);\n";
    else if (kind == 3 && mode == TM_PROJECT2D)
        /* a volume under the 2D projective mode: (s/q, t/q) on its first
           slice, r = 0, as xemu's NV2A model samples it (the flashlight's
           falloff pass binds a 4x4x4 volume so) */
        o_ << "    t" << n << " = t.volume[" << n << "].sample(t.samplers[" << n << "], float3(" << tc << ".xy / " << tc
           << ".w, 0.0));\n";
    else if (kind == 3)
        o_ << "    t" << n << " = t.volume[" << n << "].sample(t.samplers[" << n << "], " << tc << ".xyz / " << tc << ".w);\n";
    else
        o_ << "    t" << n << " = t.tex2d[" << n << "].sample(t.samplers[" << n << "], " << tc << ".xy / " << tc << ".w);\n";
}

void Translator::texture_stages() {
    for (int i = 0; i < 4; ++i) {
        mode_[i] = (d_[kTextureModes] >> (5 * i)) & 0x1f;
        if (static_cast<int>(min_stage(mode_[i])) > i) {
            o_ << "    /* stage " << i << ": texture mode 0x" << std::hex << mode_[i] << std::dec
               << " is not valid in this stage; treated as none */\n";
            mode_[i] = TM_NONE;
        }
    }
    o_ << "    float4 t0 = float4(0.0), t1 = float4(0.0), t2 = float4(0.0), t3 = float4(0.0);\n";
    bool any_dot = false;
    for (unsigned m : mode_) any_dot = any_dot || is_dot_mode(m);
    if (any_dot) o_ << "    float dp1 = 0.0, dp2 = 0.0, dp3 = 0.0;\n";

    for (int i = 0; i < 4; ++i) {
        const std::string n = std::to_string(i);
        const std::string tc = "in.texcoord[" + n + "]";
        const unsigned m = mode_[i];
        const bool sign = ctx_.texture_signed[i];
        tex_nonneg_[i] = !sign;
        /* every dot mode first forms this stage's dot product [TS] */
        if (is_dot_mode(m))
            o_ << "    dp" << n << " = dot(" << tc << ".xyz, " << dot_source(i) << ");\n";
        switch (m) {
        case TM_NONE:
            break;
        case TM_PROJECT2D: case TM_PROJECT3D: case TM_CUBEMAP:
            o_ << "    /* stage " << i << ": texture */\n";
            sample_plain(i, m);
            break;
        case TM_BRDF:
            o_ << "    /* stage " << i << ": BRDF (Xbox-only, unsupported): fallback to a plain sample */\n";
            sample_plain(i, TM_PROJECT2D);
            break;
        case TM_PASSTHRU:
            /* [TS] PASS_THROUGH_NV: (s, t, r, q) clamped to [0, 1] */
            o_ << "    t" << n << " = saturate(" << tc << ");\n";
            tex_nonneg_[i] = true;
            break;
        case TM_CLIPPLANE: {
            /* [TS] CULL_FRAGMENT_NV; TEX_SHADER_CULL_MODE [XML]: 4 bits per
               stage (s, t, r, q at 4n..4n+3), 0 = GEQUAL (keep when >= 0),
               1 = LESS (keep when < 0). The result is (0, 0, 0, 0). */
            const unsigned cm = (d_[kCompareMode] >> (4 * i)) & 0xf;
            static const char* comp[4] = {"x", "y", "z", "w"};
            o_ << "    /* stage " << i << ": clip plane */\n    if (";
            for (int c = 0; c < 4; ++c) {
                if (c) o_ << " || ";
                o_ << tc << "." << comp[c] << ((cm >> c) & 1 ? " >= 0.0" : " < 0.0");
            }
            o_ << ") discard_fragment();\n";
            tex_nonneg_[i] = true;
            break;
        }
        case TM_BUMPENVMAP: case TM_BUMPENVMAP_LUM: {
            /* [TS] OFFSET_TEXTURE_2D_NV: (s + a1 ds + a3 dt, t + a2 ds + a4 dt)
               with (ds, dt) the input texture's first two components and the
               stage's matrix; k.bump_env[n] = (m00, m01, m10, m11), so
               u' = u + m00 du + m10 dv, v' = v + m01 du + m11 dv.
               OFFSET_TEXTURE_2D_SCALE_NV additionally scales rgb by
               saturate(scale * mag + offset), mag = third component. */
            const std::string p = "t" + std::to_string(input_stage(i));
            o_ << "    /* stage " << i << ": bump env map" << (m == TM_BUMPENVMAP_LUM ? " + luminance" : "") << " */\n";
            o_ << "    {\n        float2 duv = " << p << ".rg;\n"
               << "        float4 bm = k.bump_env[" << n << "];\n"
               << "        float2 uv = " << tc << ".xy + float2(bm.x * duv.x + bm.z * duv.y, bm.y * duv.x + bm.w * duv.y);\n"
               << "        t" << n << " = t.tex2d[" << n << "].sample(t.samplers[" << n << "], uv);\n";
            if (m == TM_BUMPENVMAP_LUM)
                o_ << "        t" << n << ".rgb *= saturate(k.bump_luminance[" << n << "].x * " << p
                   << ".b + k.bump_luminance[" << n << "].y);\n";
            o_ << "    }\n";
            break;
        }
        case TM_DPNDNT_AR: case TM_DPNDNT_GB: {
            /* [TS] DEPENDENT_AR_TEXTURE_2D_NV: (a, r); _GB_: (g, b) */
            const std::string p = "t" + std::to_string(input_stage(i));
            o_ << "    /* stage " << i << ": dependent " << (m == TM_DPNDNT_AR ? "AR" : "GB") << " read */\n";
            o_ << "    t" << n << " = t.tex2d[" << n << "].sample(t.samplers[" << n << "], "
               << (m == TM_DPNDNT_AR ? p + ".ar" : p + ".gb") << ");\n";
            break;
        }
        case TM_DOTPRODUCT:
            /* [TS] DOT_PRODUCT_NV: only feeds the next stage; result 0 */
            break;
        case TM_DOT_ST:
            /* [TS] DOT_PRODUCT_TEXTURE_2D_NV: 2D at (previous dot, this dot) */
            o_ << "    t" << n << " = t.tex2d[" << n << "].sample(t.samplers[" << n << "], float2(dp"
               << (i - 1) << ", dp" << n << "));\n";
            break;
        case TM_DOT_ZW:
            /* [TS] DOT_PRODUCT_DEPTH_REPLACE_NV: depth = dp(i-1) / dp(i).
               h2_pixel_shader returns colour only, so the depth is not
               replaced (fallback); the texture result is 0. */
            o_ << "    /* stage " << i << ": depth replace (dp" << (i - 1) << " / dp" << n
               << ") not supported: depth left unchanged */\n";
            break;
        case TM_DOT_RFLCT_DIFF: {
            /* [TS] DOT_PRODUCT_DIFFUSE_CUBE_MAP_NV (stage 2): cube map at
               N = (dp1, dp2, dp3), looking ahead at stage 3's dot product. */
            o_ << "    dp3 = dot(in.texcoord[3].xyz, " << dot_source(3) << ");\n";
            o_ << "    t" << n << " = t.cube[" << n << "].sample(t.samplers[" << n << "], float3(dp1, dp2, dp3));\n";
            break;
        }
        case TM_DOT_RFLCT_SPEC: case TM_DOT_RFLCT_SPEC_CONST: {
            /* [TS] DOT_PRODUCT_REFLECT_CUBE_MAP_NV: N = (dp1, dp2, dp3),
               E = (q1, q2, q3); R = 2 N (N.E) / (N.N) - E.
               ..._CONST_EYE_: E is a constant eye vector. H2PixelConstants
               has no eye vector, so the constant-eye mode uses (0, 0, 1)
               (fallback). */
            o_ << "    {\n        float3 nrm = float3(dp1, dp2, dp3);\n";
            if (m == TM_DOT_RFLCT_SPEC)
                o_ << "        float3 eye = float3(in.texcoord[1].w, in.texcoord[2].w, in.texcoord[3].w);\n";
            else
                o_ << "        float3 eye = float3(0.0, 0.0, 1.0); /* constant eye: fallback */\n";
            o_ << "        float3 rfl = 2.0 * nrm * dot(nrm, eye) / max(dot(nrm, nrm), 1e-20) - eye;\n"
               << "        t" << n << " = t.cube[" << n << "].sample(t.samplers[" << n << "], rfl);\n    }\n";
            break;
        }
        case TM_DOT_STR_3D:
            /* [TS] DOT_PRODUCT_TEXTURE_3D_NV */
            o_ << "    t" << n << " = t.volume[" << n << "].sample(t.samplers[" << n << "], float3(dp1, dp2, dp3));\n";
            break;
        case TM_DOT_STR_CUBE:
            /* [TS] DOT_PRODUCT_TEXTURE_CUBE_MAP_NV */
            o_ << "    t" << n << " = t.cube[" << n << "].sample(t.samplers[" << n << "], float3(dp1, dp2, dp3));\n";
            break;
        default:
            o_ << "    /* stage " << i << ": unknown texture mode 0x" << std::hex << m << std::dec
               << ": treated as none */\n";
            break;
        }
        if (m == TM_NONE || m == TM_DOTPRODUCT || m == TM_DOT_ZW) tex_nonneg_[i] = true;
    }
}

/* A register as read by a combiner input (before the mapping). Register
   values: v0/v1 interpolated colours clamped to [0, 1]; fog = (fog colour,
   fog factor) [RC]; c0/c1 the stage's (or final combiner's) constants;
   t0..t3 the texture results; r0/r1 the spares. */
Expr Translator::read(unsigned reg, bool alpha_usage, bool alpha_portion, int stage,
                      bool final, bool unique_c0, bool unique_c1) {
    const std::string sw = alpha_portion ? (alpha_usage ? ".a" : ".b")
                                         : (alpha_usage ? ".aaa" : ".rgb");
    std::string base;
    switch (reg) {
    case R_ZERO: return lit(0.0);
    case R_C0:
        base = final ? "k.final_c0" : "k.c0[" + std::to_string(unique_c0 ? stage : 0) + "]";
        break;
    case R_C1:
        base = final ? "k.final_c1" : "k.c1[" + std::to_string(unique_c1 ? stage : 0) + "]";
        break;
    case R_FOG: base = "fog"; break;
    case R_V0: base = "v0"; break;
    case R_V1: base = "v1"; break;
    case R_T0: case R_T1: case R_T2: case R_T3:
        base = "t" + std::to_string(reg - R_T0);
        break;
    case R_R0: base = "r0"; break;
    case R_R1: base = "r1"; break;
    case R_V1R0: case R_EF:
        if (!final) return lit(0.0);  /* only defined in the final combiner [RC] */
        base = reg == R_V1R0 ? "v1r0" : "ef";
        if (alpha_usage || alpha_portion) return lit(0.0); /* rgb only [RC] */
        return var(base + ".rgb", true, reg == R_EF);
    default:
        return lit(0.0); /* 6, 7: reserved */
    }
    /* every register holds values in [-1, 1] (or [0, 1]) */
    return var(base + sw, reg_nonneg_[reg], true);
}

static const char* kRegName[16] = {
    nullptr, nullptr, nullptr, nullptr, "v0", "v1", nullptr, nullptr,
    "t0", "t1", "t2", "t3", "r0", "r1", nullptr, nullptr};

/* Output mapping [RC]: (x + bias) * scale, then clamp to [-1, 1]. */
std::string out_map(const std::string& x, const OutputWord& o, bool vec) {
    std::string v = x;
    if (o.bias) v = "(" + v + " - 0.5)";
    switch (o.scale) {
    case 1: v = "(" + v + " * 2.0)"; break;
    case 2: v = "(" + v + " * 4.0)"; break;
    case 3: v = "(" + v + " * 0.5)"; break;
    default: break;
    }
    return std::string("clamp(") + (vec ? "float3(" : "float(") + v + "), -1.0, 1.0)";
}

void Translator::general_stage(int s, const CombinerControl& cc) {
    const std::string S = std::to_string(s);
    o_ << "    /* general combiner stage " << s << " */\n    {\n";
    const OutputWord ro = output_word(d_[kRGBOutputs + s]);
    const OutputWord ao = output_word(d_[kAlphaOutputs + s]);
    const std::uint32_t ri = d_[kRGBInputs + s];
    const std::uint32_t ai = d_[kAlphaInputs + s];

    /* mux condition, read from r0 as the stage receives it [RC] */
    const bool need_mux = (ro.mux && ro.sum) || (ao.mux && ao.sum);
    if (need_mux) {
        if (cc.mux_msb)
            o_ << "        bool mux = r0.a >= 0.5;\n";
        else
            o_ << "        bool mux = (uint(round(saturate(r0.a) * 255.0)) & 1u) != 0u;\n";
    }

    struct Pending { unsigned reg; std::string comp; std::string value; };
    std::vector<Pending> writes;

    for (int portion = 0; portion < 2; ++portion) {
        const bool alpha = portion == 1;
        const OutputWord& ow = alpha ? ao : ro;
        const std::uint32_t iw = alpha ? ai : ri;
        if (!ow.ab && !ow.cd && !ow.sum) continue;
        Expr in[4];
        for (int j = 0; j < 4; ++j) {
            const InputField f = input_field(iw, j);
            in[j] = apply_mapping(read(f.reg, f.alpha, alpha, s, false, cc.unique_c0, cc.unique_c1),
                                  f.mapping);
        }
        const std::string ty = alpha ? "float" : "float3";
        const std::string pre = "s" + S + (alpha ? "a" : "c");
        auto wrap = [&](const Expr& e) { return ty + "(" + e.code + ")"; };
        auto dotp = [&](const Expr& a, const Expr& b) {
            return "float3(dot(float3(" + a.code + "), float3(" + b.code + ")))";
        };
        /* AB and CD products, or dot products (RGB portion only) [RC] */
        const bool abdot = !alpha && ow.ab_dot;
        const bool cddot = !alpha && ow.cd_dot;
        o_ << "        " << ty << " " << pre << "_ab = "
           << (abdot ? dotp(in[0], in[1]) : wrap(mul(in[0], in[1]))) << ";\n";
        o_ << "        " << ty << " " << pre << "_cd = "
           << (cddot ? dotp(in[2], in[3]) : wrap(mul(in[2], in[3]))) << ";\n";
        if (ow.sum) {
            if (ow.mux)
                o_ << "        " << ty << " " << pre << "_sum = mux ? " << pre << "_cd : " << pre << "_ab;\n";
            else
                o_ << "        " << ty << " " << pre << "_sum = " << pre << "_ab + " << pre << "_cd;\n";
        }
        const std::string comp = alpha ? ".a" : ".rgb";
        auto emit = [&](unsigned reg, const std::string& name) {
            if (!reg || !kRegName[reg]) return; /* discard (or not writable) */
            o_ << "        " << ty << " " << name << "_o = " << out_map(name, ow, !alpha) << ";\n";
            writes.push_back({reg, comp, name + "_o"});
        };
        emit(ow.ab, pre + "_ab");
        emit(ow.cd, pre + "_cd");
        emit(ow.sum, pre + "_sum");
        if (!alpha) {
            /* blue to alpha [INF bit positions]: the output's blue also goes to
               the register's alpha, after the alpha portion's writes */
            if (ow.ab_blue_to_alpha && ow.ab && kRegName[ow.ab])
                writes.push_back({ow.ab, ".a#", pre + "_ab_o.b"});
            if (ow.cd_blue_to_alpha && ow.cd && kRegName[ow.cd])
                writes.push_back({ow.cd, ".a#", pre + "_cd_o.b"});
        }
    }
    /* all inputs were read before any register is written */
    for (int pass = 0; pass < 2; ++pass)
        for (const Pending& w : writes) {
            const bool late = w.comp == ".a#";
            if (late != (pass == 1)) continue;
            o_ << "        " << kRegName[w.reg] << (late ? ".a" : w.comp) << " = " << w.value << ";\n";
            reg_nonneg_[w.reg] = false;
        }
    o_ << "    }\n";
}

void Translator::final_combiner() {
    const std::uint32_t abcd = d_[kFinalABCD];
    const std::uint32_t efg = d_[kFinalEFG];
    /* RC_FINAL1 bit 7: COLOR_SUM_CLAMP [XML]. Bits 6:0 carry nothing in the
       NV20 headers; NV2A-only uses of them are not decoded. */
    const bool sum_clamp = (efg >> 7) & 1;
    o_ << "    /* final combiner */\n";

    bool uses_v1r0 = false, uses_ef = false;
    for (int j = 0; j < 4; ++j) {
        const unsigned r = input_field(abcd, j).reg;
        uses_v1r0 = uses_v1r0 || r == R_V1R0;
        uses_ef = uses_ef || r == R_EF;
    }
    for (int j = 0; j < 2; ++j)
        uses_v1r0 = uses_v1r0 || input_field(efg, j).reg == R_V1R0;
    if (uses_v1r0)
        o_ << "    float4 v1r0 = float4(" << (sum_clamp ? "saturate(" : "max(")
           << "r0.rgb + v1.rgb" << (sum_clamp ? ")" : ", 0.0)") << ", 0.0);\n";
    if (uses_ef) {
        Expr e[2];
        for (int j = 0; j < 2; ++j) {
            const InputField f = input_field(efg, j);
            e[j] = apply_mapping(read(f.reg, f.alpha, false, 0, true, false, false), f.mapping);
            if (f.reg == R_EF) e[j] = lit(0.0); /* E and F cannot read E*F */
        }
        o_ << "    float4 ef = float4(float3(" << mul(e[0], e[1]).code << "), 0.0);\n";
    }
    /* A..D: rgb = A*B + (1-A)*C + D; G: alpha [RC]. Only the unsigned
       mappings are valid in the final combiner; others are applied anyway. */
    Expr in[4];
    for (int j = 0; j < 4; ++j) {
        const InputField f = input_field(abcd, j);
        in[j] = apply_mapping(read(f.reg, f.alpha, false, 0, true, false, false), f.mapping);
    }
    const InputField g = input_field(efg, 2);
    Expr ga = apply_mapping(read(g.reg, g.alpha, true, 0, true, false, false), g.mapping);
    Expr rgb = add(add(mul(in[0], in[1]), mul(one_minus(in[0]), in[2])), in[3]);
    o_ << "    float3 out_rgb = float3(" << rgb.code << ");\n";
    o_ << "    float out_a = float(" << ga.code << ");\n";
    /* the final combiner's output is clamped to [0, 1] [RC] */
    o_ << "    return saturate(float4(out_rgb, out_a));\n";
}

PixelShaderInfo Translator::run() {
    PixelShaderInfo info;
    const CombinerControl cc = combiner_control(d_[kCombinerCount]);
    char hdr[160];
    std::snprintf(hdr, sizeof hdr,
                  "/* translated D3DPIXELSHADERDEF: texture modes 0x%05x, combiner count 0x%05x */\n",
                  d_[kTextureModes], d_[kCombinerCount]);
    o_ << hdr;
    o_ << "float4 h2_pixel_shader(thread const H2PixelInput &in, constant H2PixelConstants &k, H2Textures t)\n{\n";
    texture_stages();
    for (int i = 0; i < 4; ++i) reg_nonneg_[R_T0 + i] = tex_nonneg_[i];
    reg_nonneg_[R_V0] = reg_nonneg_[R_V1] = reg_nonneg_[R_FOG] = true;
    reg_nonneg_[R_C0] = reg_nonneg_[R_C1] = true;
    reg_nonneg_[R_R0] = reg_nonneg_[R_R1] = false;
    o_ << "    float4 v0 = saturate(in.diffuse);\n"
          "    float4 v1 = saturate(in.specular);\n"
          "    float4 fog = float4(k.fog_color.rgb, saturate(in.fog));\n"
          /* [RC]: spare0's alpha starts as texture 0's alpha; the rest is
             undefined, here 0 */
          "    float4 r0 = float4(0.0, 0.0, 0.0, t0.a);\n"
          "    float4 r1 = float4(0.0);\n";
    reg_nonneg_[R_R0] = tex_nonneg_[0];
    reg_nonneg_[R_R1] = true;
    for (unsigned s = 0; s < cc.count; ++s) general_stage(static_cast<int>(s), cc);
    final_combiner();
    o_ << "}\n";
    info.source = o_.str();
    return info;
}

} // namespace

PixelShaderInfo translate_pixel_shader(const std::uint32_t (&definition)[60],
                                       const PixelShaderContext& context) {
    Translator t(definition, context);
    return t.run();
}

} // namespace h2::gpu
