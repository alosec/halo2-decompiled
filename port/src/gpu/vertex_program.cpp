/* NV2A vertex program microcode -> Metal Shading Language.
 *
 * See shader_abi.hpp for the contract and vertex_program.md for the design
 * notes and the evidence behind each encoding decision.
 *
 * Sources (all public; no GPL emulator code, no XDK material):
 *   [WIKI]  xboxdevwiki, "NV2A/Vertex Shader"
 *           (https://xboxdevwiki.net/NV2A/Vertex_Shader): the field table
 *           (word, bit offset, size of every field), the MAC and ILU opcode
 *           numbers, the swizzle code (0..3 = x..w), the output register
 *           numbers, "o[HPOS] is mirrored as R12", the 192-entry constant
 *           file and the reset values of temporaries (0) and outputs
 *           (xyz 0, w 1).
 *   [MESA]  Mesa, src/gallium/drivers/nouveau/nv30/nv30_vertprog.h and
 *           nvfx_shader.h (MIT): the NV30 vertex program format, whose
 *           source-operand layout is marked "matches NV20 exactly" and
 *           several fields "NV20". Used for the source mux codes (1 temp,
 *           2 input, 3 constant) and the write-mask bit order (x = bit 3 ..
 *           w = bit 0, NVFX_VP_MASK_X = 8), and, as an analogy, for which
 *           operand slots each opcode reads (nvfx_vertprog.c: ADD uses
 *           slots 0 and 2, scalar ops use slot 2).
 *   [SPEC]  NV_vertex_program and NV_vertex_program1_1 (Khronos registry):
 *           the semantics of every opcode.
 *   [DATA]  Facts derived here from the vertex programs stored in the
 *           retail shared.map (statistics over ~530 programs found by a
 *           heuristic scan; see vertex_program.md): confirms the mux codes,
 *           the operand slots per opcode, the mask bit order, the output
 *           select/address meaning, the output mux (1 = ILU) and that the
 *           hardware constant index already includes the D3D +96 bias.
 *           It also showed that the ILU writes r1, not the instruction's
 *           temporary register, whenever the MAC executes an operation.
 */
#include "shader_abi.hpp"

#include <cstdio>
#include <string>

namespace h2::gpu {
namespace {

/* ------------------------------------------------------------------ */
/* Decoding                                                            */
/* ------------------------------------------------------------------ */

enum MacOp : unsigned {
    MAC_NOP = 0, MAC_MOV, MAC_MUL, MAC_ADD, MAC_MAD, MAC_DP3, MAC_DPH,
    MAC_DP4, MAC_DST, MAC_MIN, MAC_MAX, MAC_SLT, MAC_SGE, MAC_ARL,
};
enum IluOp : unsigned {
    ILU_NOP = 0, ILU_MOV, ILU_RCP, ILU_RCC, ILU_RSQ, ILU_EXP, ILU_LOG, ILU_LIT,
};

/* Source mux [MESA NV30_VP_SRC_REG_TYPE_*, "matches NV20 exactly"; DATA]:
   0 is not documented. Unused operand slots in game code carry mux 2
   (input), never 0, and 0 shows up almost only in what look like false
   positives of the data scan, so we read it as (0,0,0,0) and note it. */
enum Mux : unsigned { MUX_UNDEFINED = 0, MUX_TEMP = 1, MUX_INPUT = 2, MUX_CONST = 3 };

struct Source {
    unsigned mux = 0;
    unsigned reg = 0;         /* temporary index, used when mux == MUX_TEMP */
    unsigned swizzle[4] = {0, 1, 2, 3};
    bool negate = false;
};

struct Instruction {
    unsigned ilu = 0, mac = 0;
    unsigned const_index = 0;   /* shared by every constant operand */
    unsigned input_index = 0;   /* shared by every input operand */
    Source src[3];              /* A, B, C (the wiki's sources 1, 2, 3) */
    unsigned mac_mask = 0;      /* temporary write mask of the MAC */
    unsigned temp_reg = 0;
    unsigned ilu_mask = 0;      /* temporary write mask of the ILU */
    unsigned out_mask = 0;      /* output (or constant) write mask */
    bool out_is_output = false; /* destination select: 1 output, 0 constant */
    unsigned out_address = 0;
    bool out_from_ilu = false;  /* destination mux: 0 MAC, 1 ILU */
    bool relative = false;      /* constant operand indexed by a0.x */
    bool final = false;
};

inline unsigned bits(std::uint32_t word, unsigned offset, unsigned size) {
    return (word >> offset) & ((1u << size) - 1u);
}

Instruction decode(const std::uint32_t* w) {
    Instruction in;
    /* Word 0 is unused [WIKI]. Every offset/size below is from the [WIKI]
       "Fields" table, written as (word, offset, size). */
    in.ilu = bits(w[1], 25, 3);
    in.mac = bits(w[1], 21, 4);
    in.const_index = bits(w[1], 13, 8);
    in.input_index = bits(w[1], 9, 4);

    Source& a = in.src[0];
    a.negate = bits(w[1], 8, 1);
    a.swizzle[0] = bits(w[1], 6, 2);
    a.swizzle[1] = bits(w[1], 4, 2);
    a.swizzle[2] = bits(w[1], 2, 2);
    a.swizzle[3] = bits(w[1], 0, 2);
    a.reg = bits(w[2], 28, 4);
    a.mux = bits(w[2], 26, 2);

    Source& b = in.src[1];
    b.negate = bits(w[2], 25, 1);
    b.swizzle[0] = bits(w[2], 23, 2);
    b.swizzle[1] = bits(w[2], 21, 2);
    b.swizzle[2] = bits(w[2], 19, 2);
    b.swizzle[3] = bits(w[2], 17, 2);
    b.reg = bits(w[2], 13, 4);
    b.mux = bits(w[2], 11, 2);

    Source& c = in.src[2];
    c.negate = bits(w[2], 10, 1);
    c.swizzle[0] = bits(w[2], 8, 2);
    c.swizzle[1] = bits(w[2], 6, 2);
    c.swizzle[2] = bits(w[2], 4, 2);
    c.swizzle[3] = bits(w[2], 2, 2);
    /* The register is split: high two bits at (2, 0, 2), low two bits at
       (3, 30, 2) [WIKI]. */
    c.reg = (bits(w[2], 0, 2) << 2) | bits(w[3], 30, 2);
    c.mux = bits(w[3], 28, 2);

    /* Write masks: bit 3 = x, bit 2 = y, bit 1 = z, bit 0 = w [MESA
       NVFX_VP_MASK_X = 8 .. W = 1; DATA: oPos.xyz is mask 14, oPts.x 8]. */
    in.mac_mask = bits(w[3], 24, 4);
    in.temp_reg = bits(w[3], 20, 4);
    in.ilu_mask = bits(w[3], 16, 4);
    in.out_mask = bits(w[3], 12, 4);
    /* Destination select: the wiki names the bit without saying what it
       selects. In game data, 1 goes with addresses 0, 3..12 (the output
       register numbers) and 0 is rare and goes with addresses up to 0xEE,
       so we read 1 = output register and 0 = constant register (vertex
       state programs write c[]) [DATA]. */
    in.out_is_output = bits(w[3], 11, 1);
    in.out_address = bits(w[3], 3, 8);
    /* Destination mux: 1 selects the ILU result. Instructions whose MAC is
       NOP and whose ILU runs always have 1 here [DATA]. */
    in.out_from_ilu = bits(w[3], 2, 1);
    in.relative = bits(w[3], 1, 1);
    in.final = bits(w[3], 0, 1);
    return in;
}

/* Which operand slots each MAC operation reads [DATA, matching MESA's
   NV30 usage]: binary operations read A and B, ADD reads A and C, MAD
   reads all three, MOV and ARL read A. The ILU always reads C. */
unsigned mac_slots(unsigned op) {
    switch (op) {
    case MAC_NOP: return 0;
    case MAC_MOV: case MAC_ARL: return 1;
    case MAC_ADD: return 1 | 4;
    case MAC_MAD: return 1 | 2 | 4;
    default: return 1 | 2;
    }
}

/* ------------------------------------------------------------------ */
/* Code generation                                                     */
/* ------------------------------------------------------------------ */

/* Helpers the generated program calls. They touch vectors only through
   .x/.y/.z/.w so the same text also compiles as C++ (the tests run it on
   the CPU). Semantics follow [SPEC] section 2.14.1.10 and, for RCC,
   NV_vertex_program1_1. */
constexpr const char* kHelpers = R"msl(
inline float4 h2vp_dp3(float4 a, float4 b) {
    float d = a.x * b.x + a.y * b.y + a.z * b.z;
    return float4(d, d, d, d);
}
inline float4 h2vp_dph(float4 a, float4 b) {
    float d = a.x * b.x + a.y * b.y + a.z * b.z + b.w;
    return float4(d, d, d, d);
}
inline float4 h2vp_dp4(float4 a, float4 b) {
    float d = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    return float4(d, d, d, d);
}
inline float4 h2vp_dst(float4 a, float4 b) {
    return float4(1.0f, a.y * b.y, a.z, b.w);
}
inline float4 h2vp_min(float4 a, float4 b) {
    return float4(a.x < b.x ? a.x : b.x, a.y < b.y ? a.y : b.y,
                  a.z < b.z ? a.z : b.z, a.w < b.w ? a.w : b.w);
}
inline float4 h2vp_max(float4 a, float4 b) {
    return float4(a.x >= b.x ? a.x : b.x, a.y >= b.y ? a.y : b.y,
                  a.z >= b.z ? a.z : b.z, a.w >= b.w ? a.w : b.w);
}
inline float4 h2vp_slt(float4 a, float4 b) {
    return float4(a.x < b.x ? 1.0f : 0.0f, a.y < b.y ? 1.0f : 0.0f,
                  a.z < b.z ? 1.0f : 0.0f, a.w < b.w ? 1.0f : 0.0f);
}
inline float4 h2vp_sge(float4 a, float4 b) {
    return float4(a.x >= b.x ? 1.0f : 0.0f, a.y >= b.y ? 1.0f : 0.0f,
                  a.z >= b.z ? 1.0f : 0.0f, a.w >= b.w ? 1.0f : 0.0f);
}
/* RCP: 1/x, exactly 1 for 1, +-Inf for +-0 (IEEE division gives those). */
inline float4 h2vp_rcp(float x) {
    float r = (x == 1.0f) ? 1.0f : 1.0f / x;
    return float4(r, r, r, r);
}
/* RCC: RCP clamped to +-[2^-64, 2^64] keeping the sign; +0 counts as
   positive. The bounds are the spec's 0x1F800000 and 0x5F800000. */
inline float4 h2vp_rcc(float x) {
    float r = (x == 1.0f) ? 1.0f : 1.0f / x;
    bool positive = (as_type<uint>(r) & 0x80000000u) == 0u;
    if (positive) {
        r = r > 1.84467440737e19f ? 1.84467440737e19f : r;
        r = r < 5.42101086243e-20f ? 5.42101086243e-20f : r;
    } else {
        r = r < -1.84467440737e19f ? -1.84467440737e19f : r;
        r = r > -5.42101086243e-20f ? -5.42101086243e-20f : r;
    }
    return float4(r, r, r, r);
}
/* RSQ: 1/sqrt(|x|); +Inf for 0, 0 for +-Inf. */
inline float4 h2vp_rsq(float x) {
    float a = abs(x);
    float r = (a == 1.0f) ? 1.0f : 1.0f / sqrt(a);
    return float4(r, r, r, r);
}
/* EXP: (2^floor(x), x - floor(x), 2^x, 1). z is only required to be a
   partial-precision approximation; we give the full-precision value. */
inline float4 h2vp_exp(float x) {
    float f = floor(x);
    return float4(exp2(f), x - f, exp2(x), 1.0f);
}
/* LOG of |x|: (exponent, mantissa in [1,2), log2|x|, 1). Exponent and
   mantissa come from the float's bits so they are exact. 0 (and
   denormals, which the GPU flushes) gives (-Inf, 1, -Inf, 1); Inf gives
   (+Inf, 1, +Inf, 1). */
inline float4 h2vp_log(float x) {
    uint b = as_type<uint>(x) & 0x7fffffffu;
    uint e = b >> 23;
    if (e == 0u) return float4(-INFINITY, 1.0f, -INFINITY, 1.0f);
    if (e == 255u) return float4(INFINITY, 1.0f, INFINITY, 1.0f);
    float ex = float(int(e) - 127);
    float m = as_type<float>((b & 0x007fffffu) | 0x3f800000u);
    return float4(ex, m, ex + log2(m), 1.0f);
}
/* LIT: (1, max(x,0), x > 0 ? max(y,0)^w : 0, 1), the power clamped to
   +-(128 - 1/256). A power of 0 gives 1 even for a base of 0. */
inline float4 h2vp_lit(float4 s) {
    float dx = s.x < 0.0f ? 0.0f : s.x;
    float dy = s.y < 0.0f ? 0.0f : s.y;
    float p = s.w;
    p = p < -127.99609375f ? -127.99609375f : p;
    p = p > 127.99609375f ? 127.99609375f : p;
    float spec;
    if (p == 0.0f) spec = 1.0f;
    else if (dy == 0.0f) spec = p > 0.0f ? 0.0f : INFINITY;
    else spec = exp2(p * log2(dy));
    return float4(1.0f, dx, dx > 0.0f ? spec : 0.0f, 1.0f);
}
)msl";

constexpr const char* kComp = "xyzw";
constexpr const char* kMacNames[16] = {"NOP", "MOV", "MUL", "ADD", "MAD", "DP3", "DPH",
                                       "DP4", "DST", "MIN", "MAX", "SLT", "SGE", "ARL",
                                       "?14", "?15"};
constexpr const char* kIluNames[8] = {"NOP", "MOV", "RCP", "RCC", "RSQ", "EXP", "LOG", "LIT"};

/* The output registers [WIKI "Output registers" table]. */
const char* output_target(unsigned address) {
    switch (address) {
    case 0: return "o.position";      /* oPos, also readable as r12 */
    case 3: return "o.diffuse";       /* oD0 */
    case 4: return "o.specular";      /* oD1 */
    case 5: return "oFog";            /* only .x reaches H2VertexResult */
    case 6: return "oPts";            /* only .x reaches H2VertexResult */
    case 7: return "o.back_diffuse";  /* oB0 */
    case 8: return "o.back_specular"; /* oB1 */
    case 9: return "o.texcoord[0]";
    case 10: return "o.texcoord[1]";
    case 11: return "o.texcoord[2]";
    case 12: return "o.texcoord[3]";
    default: return nullptr;
    }
}

const char* temp_name(unsigned reg) {
    static const char* names[13] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6",
                                    "r7", "r8", "r9", "r10", "r11", "o.position"};
    return reg <= 12 ? names[reg] : nullptr;
}

std::string mask_string(unsigned mask) {
    std::string s;
    for (unsigned i = 0; i < 4; ++i)
        if (mask & (8u >> i)) s += kComp[i];
    return s;
}

struct Generator {
    std::string out;
    std::uint32_t attributes = 0;
    bool fog_written = false;
    bool pts_written = false;

    void line(const std::string& s) { out += "    "; out += s; out += '\n'; }

    std::string source_base(const Instruction& in, const Source& s, std::string& note) {
        switch (s.mux) {
        case MUX_TEMP:
            if (const char* t = temp_name(s.reg)) return t;
            note = " /* r" + std::to_string(s.reg) + " does not exist: reads 0 */";
            return "float4(0.0f)";
        case MUX_INPUT:
            attributes |= 1u << in.input_index;
            return "v[" + std::to_string(in.input_index) + "]";
        case MUX_CONST:
            if (in.relative)
                /* a0.x plus the base index, clamped to the register file.
                   (NV_vertex_program reads 0 out of range; the hardware's
                   behaviour is not documented, clamping is our choice.) */
                return "c[clamp(a0 + " + std::to_string(in.const_index) + ", 0, 191)]";
            if (in.const_index > 191) {
                note = " /* c[" + std::to_string(in.const_index) + "] is past the file: clamped */";
                return "c[191]";
            }
            return "c[" + std::to_string(in.const_index) + "]";
        default:
            note = " /* source mux 0 is undocumented: reads 0 */";
            return "float4(0.0f)";
        }
    }

    std::string source_expr(const Instruction& in, const Source& s, std::string& note) {
        std::string base = source_base(in, s, note);
        bool identity = s.swizzle[0] == 0 && s.swizzle[1] == 1 && s.swizzle[2] == 2 &&
                        s.swizzle[3] == 3;
        std::string e;
        if (identity) {
            e = base;
        } else {
            e = "float4(";
            for (unsigned i = 0; i < 4; ++i) {
                if (i) e += ", ";
                e += base + "." + kComp[s.swizzle[i]];
            }
            e += ")";
        }
        return s.negate ? "-(" + e + ")" : e;
    }

    void write_masked(const std::string& target, unsigned mask, const char* value) {
        for (unsigned i = 0; i < 4; ++i)
            if (mask & (8u >> i))
                line(target + "." + kComp[i] + " = " + value + "." + kComp[i] + ";");
    }

    std::string describe(const Instruction& in, std::size_t index) {
        char buf[256];
        std::snprintf(buf, sizeof buf,
                      "/* %zu: MAC %s, ILU %s; r%u mac.%s ilu.%s; %s %u.%s from %s%s%s */",
                      index, kMacNames[in.mac], kIluNames[in.ilu], in.temp_reg,
                      mask_string(in.mac_mask).c_str(), mask_string(in.ilu_mask).c_str(),
                      in.out_is_output ? "o" : "c", in.out_address,
                      mask_string(in.out_mask).c_str(), in.out_from_ilu ? "ILU" : "MAC",
                      in.relative ? "; c[a0+]" : "", in.final ? "; final" : "");
        return buf;
    }

    bool instruction(const Instruction& in, std::size_t index, std::string& error) {
        if (in.mac > MAC_ARL) {
            error = "instruction " + std::to_string(index) + ": undefined MAC opcode " +
                    std::to_string(in.mac);
            return false;
        }
        line(describe(in, index));
        line("{");
        std::string mac;
        switch (in.mac) {
        case MAC_MOV: mac = "sA"; break;
        case MAC_MUL: mac = "sA * sB"; break;
        case MAC_ADD: mac = "sA + sC"; break;
        case MAC_MAD: mac = "sA * sB + sC"; break;
        case MAC_DP3: mac = "h2vp_dp3(sA, sB)"; break;
        case MAC_DPH: mac = "h2vp_dph(sA, sB)"; break;
        case MAC_DP4: mac = "h2vp_dp4(sA, sB)"; break;
        case MAC_DST: mac = "h2vp_dst(sA, sB)"; break;
        case MAC_MIN: mac = "h2vp_min(sA, sB)"; break;
        case MAC_MAX: mac = "h2vp_max(sA, sB)"; break;
        case MAC_SLT: mac = "h2vp_slt(sA, sB)"; break;
        case MAC_SGE: mac = "h2vp_sge(sA, sB)"; break;
        default: break;
        }
        /* Scalar ILU operations take the first swizzled component of C
           (the spec's "source0.c"). Game code always uses a replicated
           swizzle for them, so which slot the hardware really reads is
           not observable there. */
        std::string ilu;
        switch (in.ilu) {
        case ILU_MOV: ilu = "sC"; break;
        case ILU_RCP: ilu = "h2vp_rcp(sC.x)"; break;
        case ILU_RCC: ilu = "h2vp_rcc(sC.x)"; break;
        case ILU_RSQ: ilu = "h2vp_rsq(sC.x)"; break;
        case ILU_EXP: ilu = "h2vp_exp(sC.x)"; break;
        case ILU_LOG: ilu = "h2vp_log(sC.x)"; break;
        case ILU_LIT: ilu = "h2vp_lit(sC)"; break;
        default: break;
        }
        /* Only results that reach a register are computed, and only the
           operands they need are read (so attributes_read stays exact). */
        bool output_ok = in.out_mask && in.out_is_output && output_target(in.out_address);
        bool mac_value = !mac.empty() &&
                         ((in.mac_mask && temp_name(in.temp_reg)) || (output_ok && !in.out_from_ilu));
        unsigned ilu_reg = in.mac != MAC_NOP ? 1u : in.temp_reg;
        bool ilu_value = !ilu.empty() &&
                         ((in.ilu_mask && temp_name(ilu_reg)) || (output_ok && in.out_from_ilu));

        /* All operands are read before anything is written. */
        unsigned slots = (mac_value ? mac_slots(in.mac) : 0u) | (in.mac == MAC_ARL ? 1u : 0u) |
                         (ilu_value ? 4u : 0u);
        static const char* names[3] = {"sA", "sB", "sC"};
        for (unsigned i = 0; i < 3; ++i) {
            if (!(slots & (1u << i))) continue;
            std::string note;
            std::string e = source_expr(in, in.src[i], note);
            line(std::string("    const float4 ") + names[i] + " = " + e + ";" + note);
        }
        if (mac_value) line("    const float4 mac = " + mac + ";");
        if (ilu_value) line("    const float4 ilu = " + ilu + ";");

        /* MAC result into the temporary register. r12 is oPos. */
        if (mac_value && in.mac_mask) {
            if (const char* t = temp_name(in.temp_reg))
                write_masked(std::string("    ") + t, in.mac_mask, "mac");
        } else if (!mac.empty() && in.mac_mask) {
            line("    /* MAC write to r" + std::to_string(in.temp_reg) + " dropped */");
        }
        /* ILU result into a temporary: r1 whenever the MAC executes an
           operation in the same instruction, else the temporary register
           field [DATA: liveness of temporaries over the game's programs is
           only consistent with this rule; it is also the only reading under
           which instructions whose MAC and ILU masks overlap make sense]. */
        if (ilu_value && in.ilu_mask) {
            if (const char* t = temp_name(ilu_reg))
                write_masked(std::string("    ") + t, in.ilu_mask, "ilu");
        }
        /* Output register. */
        if (in.out_mask) {
            bool have = in.out_from_ilu ? ilu_value : mac_value;
            const char* value = in.out_from_ilu ? "ilu" : "mac";
            if (!in.out_is_output) {
                line("    /* constant-register write c[" + std::to_string(in.out_address) +
                     "] (vertex state programs only): not supported, dropped */");
            } else if (!output_target(in.out_address)) {
                line("    /* write to unknown output " + std::to_string(in.out_address) +
                     " dropped */");
            } else if (!have) {
                line("    /* output from an idle unit: no write */");
            } else {
                write_masked(std::string("    ") + output_target(in.out_address), in.out_mask, value);
                if (in.out_address == 5 && (in.out_mask & 8)) fog_written = true;
                if (in.out_address == 6 && (in.out_mask & 8)) pts_written = true;
            }
        }
        /* ARL: a0.x = floor(A.x) [SPEC 2.14.1.10.1], after the reads. */
        if (in.mac == MAC_ARL) line("    a0 = int(floor(sA.x));");
        line("}");
        return true;
    }
};

} // namespace

VertexProgramInfo translate_vertex_program(const std::uint32_t* instructions,
                                           std::size_t instruction_count,
                                           std::size_t start) {
    VertexProgramInfo info;
    if (!instructions || start >= instruction_count) {
        info.error = "vertex program start " + std::to_string(start) + " is outside the " +
                     std::to_string(instruction_count) + " instructions";
        return info;
    }

    Generator g;
    for (std::size_t i = start; i < instruction_count; ++i) {
        Instruction in = decode(instructions + 4 * i);
        if (!g.instruction(in, i, info.error)) return info;
        if (in.final) break;
    }

    std::string s = kHelpers;
    s += "\nH2VertexResult h2_vertex_program(thread const float4 (&v)[16], constant float4 *c)\n{\n";
    s += "    H2VertexResult o;\n";
    s += "    o.position = float4(0.0f, 0.0f, 0.0f, 1.0f);\n";
    s += "    o.diffuse = float4(0.0f, 0.0f, 0.0f, 1.0f);\n";
    s += "    o.specular = float4(0.0f, 0.0f, 0.0f, 1.0f);\n";
    s += "    o.back_diffuse = float4(0.0f, 0.0f, 0.0f, 1.0f);\n";
    s += "    o.back_specular = float4(0.0f, 0.0f, 0.0f, 1.0f);\n";
    s += "    for (int i = 0; i < 4; ++i) o.texcoord[i] = float4(0.0f, 0.0f, 0.0f, 1.0f);\n";
    s += "    o.fog = 0.0f;\n";
    s += "    o.point_size = 1.0f;\n";
    s += "    float4 oFog = float4(0.0f, 0.0f, 0.0f, 1.0f);\n";
    s += "    float4 oPts = float4(0.0f, 0.0f, 0.0f, 1.0f);\n";
    for (int r = 0; r < 12; ++r)
        s += "    float4 r" + std::to_string(r) + " = float4(0.0f);\n";
    s += "    int a0 = 0;\n";
    s += "    (void)v; (void)c; (void)a0; (void)oFog; (void)oPts;\n";
    for (int r = 0; r < 12; ++r) s += "    (void)r" + std::to_string(r) + ";\n";
    s += g.out;
    if (g.fog_written) s += "    o.fog = oFog.x;\n";
    if (g.pts_written) s += "    o.point_size = oPts.x;\n";
    s += "    return o;\n}\n";

    info.source = std::move(s);
    info.attributes_read = g.attributes;
    return info;
}

} // namespace h2::gpu
