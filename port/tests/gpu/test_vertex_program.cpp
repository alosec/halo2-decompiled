/* Tests for the NV2A vertex program translator (port/src/gpu/vertex_program.cpp).
 *
 * Build:
 *   clang++ -std=c++20 -I port/src/gpu -I port/include \
 *       port/src/gpu/vertex_program.cpp port/tests/gpu/test_vertex_program.cpp -o $TMP/test
 * Run:
 *   $TMP/test [work-dir]        (work dir defaults to $TMPDIR/h2_vp_test)
 *
 * What it does:
 *   - builds programs with its own encoder (the inverse of the decoder, written
 *     from the same public field table);
 *   - runs them in a CPU interpreter written straight from the
 *     NV_vertex_program semantics and checks hand-computed expectations;
 *   - compiles the generated MSL as C++ (a tiny shim stands in for
 *     metal_stdlib; the generator only touches vectors through .x/.y/.z/.w)
 *     into a dylib, runs it on the same inputs and compares with the
 *     interpreter - for the hand-written programs and for a few hundred random
 *     ones;
 *   - compiles prelude + generated source + a wrapper vertex function with
 *     `xcrun metal -std=metal3.0` for every program.
 */
#include "shader_abi.hpp"

#include <dlfcn.h>

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <functional>
#include <random>
#include <sstream>
#include <string>
#include <vector>

using h2::gpu::translate_vertex_program;
using h2::gpu::VertexProgramInfo;

namespace {

int g_failures = 0;
int g_checks = 0;

#define CHECK(cond, ...)                                                       \
    do {                                                                       \
        ++g_checks;                                                            \
        if (!(cond)) {                                                         \
            ++g_failures;                                                      \
            std::printf("FAIL %s:%d: %s: ", __FILE__, __LINE__, #cond);        \
            std::printf(__VA_ARGS__);                                          \
            std::printf("\n");                                                 \
        }                                                                      \
    } while (0)

/* ------------------------------------------------------------------ */
/* Encoder                                                             */
/* ------------------------------------------------------------------ */

enum { NOP = 0, MOV, MUL, ADD, MAD, DP3, DPH, DP4, DST, MIN, MAX, SLT, SGE, ARL };
enum { INOP = 0, IMOV, RCP, RCC, RSQ, EXP, LOG, LIT };
enum { MTEMP = 1, MINPUT = 2, MCONST = 3 };
enum { WX = 8, WY = 4, WZ = 2, WW = 1, WALL = 15 };
enum { O_POS = 0, O_D0 = 3, O_D1 = 4, O_FOG = 5, O_PTS = 6, O_B0 = 7, O_B1 = 8, O_T0 = 9 };

struct Src {
    unsigned mux = MINPUT, reg = 0;
    unsigned swz[4] = {0, 1, 2, 3};
    bool neg = false;
};

struct Ins {
    unsigned mac = NOP, ilu = INOP;
    unsigned cidx = 0, vidx = 0;
    Src s[3];
    unsigned mac_mask = 0, treg = 0, ilu_mask = 0;
    unsigned out_mask = 0, out_addr = 0;
    bool out_output = true, out_ilu = false, rel = false, final = false;
};

Src temp(unsigned r, const char* sw = "xyzw", bool neg = false) {
    Src s; s.mux = MTEMP; s.reg = r; s.neg = neg;
    for (int i = 0; i < 4; ++i) s.swz[i] = std::strchr("xyzw", sw[i]) - "xyzw";
    return s;
}
Src input(const char* sw = "xyzw", bool neg = false) { Src s = temp(0, sw, neg); s.mux = MINPUT; return s; }
Src cnst(const char* sw = "xyzw", bool neg = false) { Src s = temp(0, sw, neg); s.mux = MCONST; return s; }

void put(std::uint32_t& w, unsigned off, unsigned size, unsigned value) {
    std::uint32_t m = ((1u << size) - 1u) << off;
    w = (w & ~m) | ((value << off) & m);
}

/* Field positions from xboxdevwiki "NV2A/Vertex Shader" (word, offset, size). */
void encode(const Ins& in, std::uint32_t* w) {
    w[0] = w[1] = w[2] = w[3] = 0;
    put(w[1], 25, 3, in.ilu);
    put(w[1], 21, 4, in.mac);
    put(w[1], 13, 8, in.cidx);
    put(w[1], 9, 4, in.vidx);
    put(w[1], 8, 1, in.s[0].neg);
    put(w[1], 6, 2, in.s[0].swz[0]); put(w[1], 4, 2, in.s[0].swz[1]);
    put(w[1], 2, 2, in.s[0].swz[2]); put(w[1], 0, 2, in.s[0].swz[3]);
    put(w[2], 28, 4, in.s[0].reg);
    put(w[2], 26, 2, in.s[0].mux);
    put(w[2], 25, 1, in.s[1].neg);
    put(w[2], 23, 2, in.s[1].swz[0]); put(w[2], 21, 2, in.s[1].swz[1]);
    put(w[2], 19, 2, in.s[1].swz[2]); put(w[2], 17, 2, in.s[1].swz[3]);
    put(w[2], 13, 4, in.s[1].reg);
    put(w[2], 11, 2, in.s[1].mux);
    put(w[2], 10, 1, in.s[2].neg);
    put(w[2], 8, 2, in.s[2].swz[0]); put(w[2], 6, 2, in.s[2].swz[1]);
    put(w[2], 4, 2, in.s[2].swz[2]); put(w[2], 2, 2, in.s[2].swz[3]);
    put(w[2], 0, 2, in.s[2].reg >> 2);
    put(w[3], 30, 2, in.s[2].reg & 3);
    put(w[3], 28, 2, in.s[2].mux);
    put(w[3], 24, 4, in.mac_mask);
    put(w[3], 20, 4, in.treg);
    put(w[3], 16, 4, in.ilu_mask);
    put(w[3], 12, 4, in.out_mask);
    put(w[3], 11, 1, in.out_output);
    put(w[3], 3, 8, in.out_addr);
    put(w[3], 2, 1, in.out_ilu);
    put(w[3], 1, 1, in.rel);
    put(w[3], 0, 1, in.final);
}

std::vector<std::uint32_t> encode_all(std::vector<Ins> prog, bool mark_final = true) {
    if (mark_final && !prog.empty()) prog.back().final = true;
    std::vector<std::uint32_t> words(prog.size() * 4);
    for (size_t i = 0; i < prog.size(); ++i) encode(prog[i], &words[4 * i]);
    return words;
}

/* ------------------------------------------------------------------ */
/* CPU interpreter                                                     */
/* ------------------------------------------------------------------ */

struct V4 { float v[4] = {0, 0, 0, 0}; };
V4 vec(float x, float y, float z, float w) { V4 r; r.v[0] = x; r.v[1] = y; r.v[2] = z; r.v[3] = w; return r; }
V4 splat(float s) { return vec(s, s, s, s); }

struct Result {
    V4 pos, d0, d1, b0, b1, t[4];
    float fog = 0, pts = 1;
};

struct Inputs {
    V4 v[16];
    V4 c[192];
};

/* decoded form = Ins; decode words back with the same table for the
   interpreter, so the interpreter runs on what the hardware would see */
unsigned get(std::uint32_t w, unsigned off, unsigned size) { return (w >> off) & ((1u << size) - 1u); }
Ins decode(const std::uint32_t* w) {
    Ins in;
    in.ilu = get(w[1], 25, 3); in.mac = get(w[1], 21, 4);
    in.cidx = get(w[1], 13, 8); in.vidx = get(w[1], 9, 4);
    in.s[0].neg = get(w[1], 8, 1);
    for (int i = 0; i < 4; ++i) in.s[0].swz[i] = get(w[1], 6 - 2 * i, 2);
    in.s[0].reg = get(w[2], 28, 4); in.s[0].mux = get(w[2], 26, 2);
    in.s[1].neg = get(w[2], 25, 1);
    for (int i = 0; i < 4; ++i) in.s[1].swz[i] = get(w[2], 23 - 2 * i, 2);
    in.s[1].reg = get(w[2], 13, 4); in.s[1].mux = get(w[2], 11, 2);
    in.s[2].neg = get(w[2], 10, 1);
    for (int i = 0; i < 4; ++i) in.s[2].swz[i] = get(w[2], 8 - 2 * i, 2);
    in.s[2].reg = (get(w[2], 0, 2) << 2) | get(w[3], 30, 2); in.s[2].mux = get(w[3], 28, 2);
    in.mac_mask = get(w[3], 24, 4); in.treg = get(w[3], 20, 4); in.ilu_mask = get(w[3], 16, 4);
    in.out_mask = get(w[3], 12, 4); in.out_output = get(w[3], 11, 1);
    in.out_addr = get(w[3], 3, 8); in.out_ilu = get(w[3], 2, 1);
    in.rel = get(w[3], 1, 1); in.final = get(w[3], 0, 1);
    return in;
}

float spec_rcp(float x) { return x == 1.0f ? 1.0f : 1.0f / x; }

Result interpret(const std::uint32_t* words, size_t count, size_t start, const Inputs& io) {
    V4 r[13];          /* r12 = oPos */
    r[12] = vec(0, 0, 0, 1);
    V4 outs[13];
    for (auto& o : outs) o = vec(0, 0, 0, 1);
    bool fog_w = false, pts_w = false;
    int a0 = 0;
    for (size_t pc = start; pc < count; ++pc) {
        Ins in = decode(words + 4 * pc);
        auto read = [&](int slot) {
            const Src& s = in.s[slot];
            V4 base;
            if (s.mux == MTEMP) base = s.reg <= 12 ? r[s.reg] : V4{};
            else if (s.mux == MINPUT) base = io.v[in.vidx];
            else if (s.mux == MCONST) {
                int idx = in.rel ? a0 + int(in.cidx) : int(in.cidx);
                idx = idx < 0 ? 0 : (idx > 191 ? 191 : idx);
                base = io.c[idx];
            }
            V4 out;
            for (int i = 0; i < 4; ++i) out.v[i] = s.neg ? -base.v[s.swz[i]] : base.v[s.swz[i]];
            return out;
        };
        V4 A = read(0), B = read(1), C = read(2);
        bool have_mac = true, have_ilu = true;
        V4 m, u;
        auto dot = [](const V4& a, const V4& b, int n) { float d = 0; for (int i = 0; i < n; ++i) d = d + a.v[i] * b.v[i]; return d; };
        switch (in.mac) {
        case MOV: m = A; break;
        case MUL: for (int i = 0; i < 4; ++i) m.v[i] = A.v[i] * B.v[i]; break;
        case ADD: for (int i = 0; i < 4; ++i) m.v[i] = A.v[i] + C.v[i]; break;
        case MAD: for (int i = 0; i < 4; ++i) m.v[i] = A.v[i] * B.v[i] + C.v[i]; break;
        case DP3: m = splat(dot(A, B, 3)); break;
        case DPH: m = splat(dot(A, B, 3) + B.v[3]); break;
        case DP4: m = splat(dot(A, B, 4)); break;
        case DST: m = vec(1.0f, A.v[1] * B.v[1], A.v[2], B.v[3]); break;
        case MIN: for (int i = 0; i < 4; ++i) m.v[i] = A.v[i] < B.v[i] ? A.v[i] : B.v[i]; break;
        case MAX: for (int i = 0; i < 4; ++i) m.v[i] = A.v[i] >= B.v[i] ? A.v[i] : B.v[i]; break;
        case SLT: for (int i = 0; i < 4; ++i) m.v[i] = A.v[i] < B.v[i] ? 1.0f : 0.0f; break;
        case SGE: for (int i = 0; i < 4; ++i) m.v[i] = A.v[i] >= B.v[i] ? 1.0f : 0.0f; break;
        default: have_mac = false; break;
        }
        float x = C.v[0];
        switch (in.ilu) {
        case IMOV: u = C; break;
        case RCP: u = splat(spec_rcp(x)); break;
        case RCC: {
            float q = spec_rcp(x);
            /* written as the spec's pseudo-code (NaN passes through) */
            if (!std::signbit(q)) {
                if (q > 1.84467440737e19f) q = 1.84467440737e19f;
                else if (q < 5.42101086243e-20f) q = 5.42101086243e-20f;
            } else {
                if (q < -1.84467440737e19f) q = -1.84467440737e19f;
                else if (q > -5.42101086243e-20f) q = -5.42101086243e-20f;
            }
            u = splat(q);
            break;
        }
        case RSQ: { float a = std::fabs(x); u = splat(a == 1.0f ? 1.0f : 1.0f / std::sqrt(a)); break; }
        case EXP: u = vec(std::exp2(std::floor(x)), x - std::floor(x), std::exp2(x), 1.0f); break;
        case LOG: {
            float a = std::fabs(x);
            if (a == 0.0f || !std::isnormal(a)) {
                if (std::isinf(a)) u = vec(INFINITY, 1, INFINITY, 1);
                else u = vec(-INFINITY, 1, -INFINITY, 1);
            } else {
                int e; float f = std::frexp(a, &e);   /* a = f * 2^e, f in [0.5,1) */
                float ex = float(e - 1), mant = f * 2.0f;
                u = vec(ex, mant, ex + std::log2(mant), 1.0f);
            }
            break;
        }
        case LIT: {
            float dx = C.v[0] < 0.0f ? 0.0f : C.v[0], dy = C.v[1] < 0.0f ? 0.0f : C.v[1];
            float p = C.v[3];
            if (p < -127.99609375f) p = -127.99609375f;
            else if (p > 127.99609375f) p = 127.99609375f;
            float s = p == 0.0f ? 1.0f : (dy == 0.0f ? (p > 0 ? 0.0f : INFINITY) : std::exp2(p * std::log2(dy)));
            u = vec(1.0f, dx, dx > 0.0f ? s : 0.0f, 1.0f);
            break;
        }
        default: have_ilu = false; break;
        }
        auto wr = [](V4& dst, unsigned mask, const V4& val) {
            for (int i = 0; i < 4; ++i) if (mask & (8u >> i)) dst.v[i] = val.v[i];
        };
        if (have_mac && in.mac_mask && in.treg <= 12) wr(r[in.treg], in.mac_mask, m);
        if (have_ilu && in.ilu_mask) {
            unsigned t = in.mac != NOP ? 1u : in.treg;
            if (t <= 12) wr(r[t], in.ilu_mask, u);
        }
        if (in.out_mask && in.out_output && (in.out_ilu ? have_ilu : have_mac)) {
            const V4& val = in.out_ilu ? u : m;
            unsigned a = in.out_addr;
            if (a == 0) wr(r[12], in.out_mask, val);
            else if (a >= 3 && a <= 12) {
                wr(outs[a], in.out_mask, val);
                if (a == O_FOG && (in.out_mask & 8)) fog_w = true;
                if (a == O_PTS && (in.out_mask & 8)) pts_w = true;
            }
        }
        if (in.mac == ARL) a0 = int(std::floor(A.v[0]));
        if (in.final) break;
    }
    Result res;
    res.pos = r[12];
    res.d0 = outs[3]; res.d1 = outs[4]; res.b0 = outs[7]; res.b1 = outs[8];
    for (int i = 0; i < 4; ++i) res.t[i] = outs[9 + i];
    if (fog_w) res.fog = outs[5].v[0];
    if (pts_w) res.pts = outs[6].v[0];
    return res;
}

/* ------------------------------------------------------------------ */
/* Running the generated source on the CPU and compiling it for Metal  */
/* ------------------------------------------------------------------ */

std::string g_work;

int run(const std::string& cmd) {
    int rc = std::system(cmd.c_str());
    return rc;
}

constexpr const char* kCpuShim = R"cpp(
#include <cmath>
#include <cstdint>
#include <cstring>
#define thread
#define constant const
typedef std::uint32_t uint;
struct float4 {
    float x, y, z, w;
    float4() = default;
    float4(float s) : x(s), y(s), z(s), w(s) {}
    float4(float a, float b, float c, float d) : x(a), y(b), z(c), w(d) {}
};
inline float4 operator+(float4 a, float4 b) { return float4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w); }
inline float4 operator*(float4 a, float4 b) { return float4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w); }
inline float4 operator-(float4 a) { return float4(-a.x, -a.y, -a.z, -a.w); }
template <typename T, typename F> inline T as_type(F f) { static_assert(sizeof(T) == sizeof(F)); T t; std::memcpy(&t, &f, sizeof t); return t; }
struct H2VertexResult {
    float4 position, diffuse, specular, back_diffuse, back_specular;
    float4 texcoord[4];
    float fog, point_size;
};
#define H2_SHIM_FUNCS \
    inline float floor(float x) { return std::floor(x); } \
    inline float abs(float x) { return std::fabs(x); } \
    inline float sqrt(float x) { return std::sqrt(x); } \
    inline float exp2(float x) { return std::exp2(x); } \
    inline float log2(float x) { return std::log2(x); } \
    inline int clamp(int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); }
)cpp";

struct Program {
    std::string name;
    std::vector<std::uint32_t> words;
    size_t start = 0;
    VertexProgramInfo info;
};

typedef void (*RunFn)(const float*, const float*, float*);

/* Compiles every program's source as C++ in one dylib; returns the handle. */
void* build_cpu(const std::vector<Program>& progs, const std::string& tag) {
    std::string cpp = g_work + "/cpu_" + tag + ".cpp";
    std::string lib = g_work + "/cpu_" + tag + ".dylib";
    std::ofstream f(cpp);
    f << kCpuShim;
    for (size_t i = 0; i < progs.size(); ++i) {
        f << "namespace p" << i << " {\nH2_SHIM_FUNCS\n" << progs[i].info.source << "\n}\n";
        f << "extern \"C\" void run_p" << i << "(const float* vin, const float* cin, float* out) {\n"
          << "    float4 v[16]; for (int i = 0; i < 16; ++i) v[i] = float4(vin[4*i], vin[4*i+1], vin[4*i+2], vin[4*i+3]);\n"
          << "    const float4* c = reinterpret_cast<const float4*>(cin);\n"
          << "    H2VertexResult r = p" << i << "::h2_vertex_program(v, c);\n"
          << "    std::memcpy(out, &r, sizeof r);\n}\n";
    }
    f.close();
    std::string cmd = "xcrun clang++ -std=c++20 -O1 -ffp-contract=off -shared -fPIC -w '" + cpp +
                      "' -o '" + lib + "'";
    if (run(cmd) != 0) { CHECK(false, "CPU build of %s failed", cpp.c_str()); return nullptr; }
    void* h = dlopen(lib.c_str(), RTLD_NOW | RTLD_LOCAL);
    CHECK(h != nullptr, "dlopen %s: %s", lib.c_str(), dlerror());
    return h;
}

Result run_cpu(void* lib, size_t index, const Inputs& io) {
    char sym[32]; std::snprintf(sym, sizeof sym, "run_p%zu", index);
    RunFn fn = reinterpret_cast<RunFn>(dlsym(lib, sym));
    float out[4 * 9 + 2];
    fn(&io.v[0].v[0], &io.c[0].v[0], out);
    Result r;
    V4* dst[9] = {&r.pos, &r.d0, &r.d1, &r.b0, &r.b1, &r.t[0], &r.t[1], &r.t[2], &r.t[3]};
    for (int k = 0; k < 9; ++k) std::memcpy(dst[k]->v, out + 4 * k, 16);
    r.fog = out[36]; r.pts = out[37];
    return r;
}

constexpr const char* kMetalWrapper = R"msl(
struct H2TestOut {
    float4 position [[position]];
    float4 d0; float4 d1; float4 b0; float4 b1;
    float4 t0; float4 t1; float4 t2; float4 t3;
    float fog;
    float psize [[point_size]];
};
vertex H2TestOut NAME(uint vid [[vertex_id]],
                      constant float4 *attrs [[buffer(0)]],
                      constant float4 *c [[buffer(1)]]) {
    float4 v[16];
    for (int i = 0; i < 16; ++i) v[i] = attrs[vid * 16 + i];
    H2VertexResult r = NS h2_vertex_program(v, c);
    H2TestOut o;
    o.position = r.position;
    o.d0 = r.diffuse; o.d1 = r.specular; o.b0 = r.back_diffuse; o.b1 = r.back_specular;
    o.t0 = r.texcoord[0]; o.t1 = r.texcoord[1]; o.t2 = r.texcoord[2]; o.t3 = r.texcoord[3];
    o.fog = r.fog; o.psize = r.point_size;
    return o;
}
)msl";

std::string replace_all(std::string s, const std::string& a, const std::string& b) {
    for (size_t p = 0; (p = s.find(a, p)) != std::string::npos; p += b.size()) s.replace(p, a.size(), b);
    return s;
}

bool metal_compile(const std::string& file, const std::string& text) {
    std::string path = g_work + "/" + file + ".metal";
    std::ofstream(path) << text;
    std::string cmd = "xcrun metal -std=metal3.0 -c '" + path + "' -o '" + g_work + "/" + file +
                      ".air' 2> '" + g_work + "/" + file + ".log'";
    bool ok = run(cmd) == 0;
    CHECK(ok, "metal compile of %s failed, see %s.log", path.c_str(), (g_work + "/" + file).c_str());
    return ok;
}

/* One .metal per program, exactly as the backend would assemble it. */
void metal_single(const Program& p) {
    std::string text = std::string(h2::gpu::kShaderPrelude) + p.info.source +
                       replace_all(replace_all(kMetalWrapper, "NAME", "h2_test_vs"), "NS ", "");
    metal_compile("msl_" + p.name, text);
}

/* Many programs in one file, each in a namespace (used for the fuzz set). */
void metal_batch(const std::vector<Program>& progs, const std::string& tag) {
    std::string text = h2::gpu::kShaderPrelude;
    for (size_t i = 0; i < progs.size(); ++i) {
        std::string ns = "p" + std::to_string(i);
        text += "namespace " + ns + " {\n" + progs[i].info.source + "\n}\n";
        text += replace_all(replace_all(replace_all(kMetalWrapper, "H2TestOut", "H2TestOut_" + ns), "NAME", "vs_" + ns), "NS ", ns + "::");
    }
    metal_compile("msl_batch_" + tag, text);
}

bool same(float a, float b) {
    if (std::isnan(a) || std::isnan(b)) return std::isnan(a) && std::isnan(b);
    if (std::isinf(a) || std::isinf(b)) return a == b;
    float d = std::fabs(a - b);
    return d <= 1e-6f || d <= 1e-5f * std::fmax(std::fabs(a), std::fabs(b));
}

bool same(const V4& a, const V4& b) {
    for (int i = 0; i < 4; ++i) if (!same(a.v[i], b.v[i])) return false;
    return true;
}

std::string str(const V4& a) {
    char b[128]; std::snprintf(b, sizeof b, "(%g, %g, %g, %g)", a.v[0], a.v[1], a.v[2], a.v[3]);
    return b;
}

bool same(const Result& a, const Result& b, std::string& why) {
    const V4* x[9] = {&a.pos, &a.d0, &a.d1, &a.b0, &a.b1, &a.t[0], &a.t[1], &a.t[2], &a.t[3]};
    const V4* y[9] = {&b.pos, &b.d0, &b.d1, &b.b0, &b.b1, &b.t[0], &b.t[1], &b.t[2], &b.t[3]};
    static const char* n[9] = {"pos", "d0", "d1", "b0", "b1", "t0", "t1", "t2", "t3"};
    for (int i = 0; i < 9; ++i)
        if (!same(*x[i], *y[i])) { why = std::string(n[i]) + " " + str(*x[i]) + " vs " + str(*y[i]); return false; }
    if (!same(a.fog, b.fog)) { why = "fog"; return false; }
    if (!same(a.pts, b.pts)) { why = "pts"; return false; }
    return true;
}

/* ------------------------------------------------------------------ */
/* Hand-written cases                                                  */
/* ------------------------------------------------------------------ */

struct Case {
    Program prog;
    Inputs in;
    std::function<void(const Result&, const char* who)> expect;
};

Inputs base_inputs() {
    Inputs io;
    for (int i = 0; i < 16; ++i) io.v[i] = vec(i + 0.5f, -(i + 0.25f), 2.0f * i, 1.0f + i);
    for (int i = 0; i < 192; ++i) io.c[i] = vec(0.01f * i, 1.0f + 0.5f * (i % 7), -0.25f * (i % 5), 3.0f - i * 0.125f);
    return io;
}

#define EXPECT_V4(got, want) CHECK(same(got, want), "%s: %s = %s, want %s", who, #got, str(got).c_str(), str(want).c_str())
#define EXPECT_F(got, want) CHECK(same(got, want), "%s: %s = %g, want %g", who, #got, (double)(got), (double)(want))

/* single instruction: MAC op on v1 (A), c[100] (B), v1 (C) -> oD0 */
Ins mac_to_d0(unsigned op, Src a, Src b, Src c) {
    Ins i; i.mac = op; i.vidx = 1; i.cidx = 100; i.s[0] = a; i.s[1] = b; i.s[2] = c;
    i.out_mask = WALL; i.out_addr = O_D0;
    return i;
}
Ins ilu_to_d0(unsigned op, Src c) {
    Ins i; i.ilu = op; i.vidx = 2; i.cidx = 10; i.s[2] = c;
    i.out_mask = WALL; i.out_addr = O_D0; i.out_ilu = true;
    return i;
}

std::vector<Case> make_cases() {
    std::vector<Case> cases;
    auto add = [&](const std::string& name, std::vector<Ins> code, Inputs in,
                   std::function<void(const Result&, const char*)> expect, size_t start = 0,
                   bool mark_final = true) {
        Case c;
        c.prog.name = name;
        c.prog.words = encode_all(code, mark_final);
        c.prog.start = start;
        c.in = in;
        c.expect = std::move(expect);
        cases.push_back(std::move(c));
    };
    Inputs io = base_inputs();
    io.v[1] = vec(1.5f, -2.0f, 3.0f, 0.5f);
    io.c[100] = vec(2.0f, 0.25f, -1.0f, 4.0f);
    const V4 a = io.v[1];

    /* MAC opcodes */
    add("mac_mov", {mac_to_d0(MOV, input(), cnst(), input())}, io,
        [=](const Result& r, const char* who) { EXPECT_V4(r.d0, a); });
    add("mac_mul", {mac_to_d0(MUL, input(), cnst(), input())}, io,
        [=](const Result& r, const char* who) { EXPECT_V4(r.d0, vec(3.0f, -0.5f, -3.0f, 2.0f)); });
    add("mac_add", {mac_to_d0(ADD, input(), cnst(), cnst())}, io,   /* A + C */
        [=](const Result& r, const char* who) { EXPECT_V4(r.d0, vec(3.5f, -1.75f, 2.0f, 4.5f)); });
    add("mac_mad", {mac_to_d0(MAD, input(), cnst(), input())}, io,  /* A*B + C */
        [=](const Result& r, const char* who) { EXPECT_V4(r.d0, vec(4.5f, -2.5f, 0.0f, 2.5f)); });
    add("mac_dp3", {mac_to_d0(DP3, input(), cnst(), input())}, io,
        [=](const Result& r, const char* who) { EXPECT_V4(r.d0, splat(3.0f - 0.5f - 3.0f)); });
    add("mac_dph", {mac_to_d0(DPH, input(), cnst(), input())}, io,
        [=](const Result& r, const char* who) { EXPECT_V4(r.d0, splat(3.0f - 0.5f - 3.0f + 4.0f)); });
    add("mac_dp4", {mac_to_d0(DP4, input(), cnst(), input())}, io,
        [=](const Result& r, const char* who) { EXPECT_V4(r.d0, splat(3.0f - 0.5f - 3.0f + 2.0f)); });
    add("mac_dst", {mac_to_d0(DST, input(), cnst(), input())}, io,
        [=](const Result& r, const char* who) { EXPECT_V4(r.d0, vec(1.0f, -0.5f, 3.0f, 4.0f)); });
    add("mac_min", {mac_to_d0(MIN, input(), cnst(), input())}, io,
        [=](const Result& r, const char* who) { EXPECT_V4(r.d0, vec(1.5f, -2.0f, -1.0f, 0.5f)); });
    add("mac_max", {mac_to_d0(MAX, input(), cnst(), input())}, io,
        [=](const Result& r, const char* who) { EXPECT_V4(r.d0, vec(2.0f, 0.25f, 3.0f, 4.0f)); });
    add("mac_slt", {mac_to_d0(SLT, input(), cnst(), input())}, io,
        [=](const Result& r, const char* who) { EXPECT_V4(r.d0, vec(1, 1, 0, 1)); });
    {
        Inputs e = io; e.c[100] = vec(1.5f, -3.0f, 3.5f, 0.5f);   /* equal x and w */
        add("mac_sge", {mac_to_d0(SGE, input(), cnst(), input())}, e,
            [=](const Result& r, const char* who) { EXPECT_V4(r.d0, vec(1, 1, 0, 1)); });
    }

    /* ILU opcodes, operand C, with special values from the specs */
    auto ilu_case = [&](const std::string& name, unsigned op, V4 cval, V4 want) {
        Inputs e = io; e.v[2] = cval;
        add(name, {ilu_to_d0(op, input())}, e,
            [=](const Result& r, const char* who) { EXPECT_V4(r.d0, want); });
    };
    ilu_case("ilu_mov", IMOV, vec(1, 2, 3, 4), vec(1, 2, 3, 4));
    ilu_case("ilu_rcp", RCP, vec(4, 9, 9, 9), splat(0.25f));
    ilu_case("ilu_rcp_one", RCP, vec(1, 9, 9, 9), splat(1.0f));
    ilu_case("ilu_rcp_zero", RCP, vec(0, 9, 9, 9), splat(INFINITY));
    ilu_case("ilu_rcp_negzero", RCP, vec(-0.0f, 9, 9, 9), splat(-INFINITY));
    ilu_case("ilu_rcp_inf", RCP, vec(INFINITY, 9, 9, 9), splat(0.0f));
    ilu_case("ilu_rcc", RCC, vec(-8, 9, 9, 9), splat(-0.125f));
    ilu_case("ilu_rcc_zero", RCC, vec(0, 9, 9, 9), splat(1.84467440737e19f));
    ilu_case("ilu_rcc_negzero", RCC, vec(-0.0f, 9, 9, 9), splat(-1.84467440737e19f));
    ilu_case("ilu_rcc_big", RCC, vec(1e30f, 9, 9, 9), splat(5.42101086243e-20f));
    ilu_case("ilu_rcc_negbig", RCC, vec(-1e30f, 9, 9, 9), splat(-5.42101086243e-20f));
    ilu_case("ilu_rsq", RSQ, vec(16, 9, 9, 9), splat(0.25f));
    ilu_case("ilu_rsq_neg", RSQ, vec(-4, 9, 9, 9), splat(0.5f));
    ilu_case("ilu_rsq_zero", RSQ, vec(0, 9, 9, 9), splat(INFINITY));
    ilu_case("ilu_exp", EXP, vec(2.5f, 9, 9, 9), vec(4.0f, 0.5f, std::exp2(2.5f), 1.0f));
    ilu_case("ilu_exp_neg", EXP, vec(-1.25f, 9, 9, 9), vec(0.25f, 0.75f, std::exp2(-1.25f), 1.0f));
    ilu_case("ilu_log", LOG, vec(8, 9, 9, 9), vec(3, 1, 3, 1));
    ilu_case("ilu_log_neg", LOG, vec(-6, 9, 9, 9), vec(2, 1.5f, std::log2(6.0f), 1));
    ilu_case("ilu_log_frac", LOG, vec(0.375f, 9, 9, 9), vec(-2, 1.5f, std::log2(0.375f), 1));
    ilu_case("ilu_log_zero", LOG, vec(0, 9, 9, 9), vec(-INFINITY, 1, -INFINITY, 1));
    ilu_case("ilu_log_inf", LOG, vec(-INFINITY, 9, 9, 9), vec(INFINITY, 1, INFINITY, 1));
    ilu_case("ilu_lit", LIT, vec(0.5f, 0.25f, 7, 2), vec(1, 0.5f, 0.0625f, 1));
    ilu_case("ilu_lit_back", LIT, vec(-0.5f, 0.25f, 7, 2), vec(1, 0, 0, 1));
    ilu_case("ilu_lit_negspec", LIT, vec(0.5f, -0.25f, 7, 2), vec(1, 0.5f, 0, 1));
    ilu_case("ilu_lit_pow0", LIT, vec(0.5f, 0.0f, 7, 0), vec(1, 0.5f, 1, 1));
    ilu_case("ilu_lit_clamp", LIT, vec(0.5f, 0.5f, 7, 200), vec(1, 0.5f, std::exp2(-127.99609375f), 1));
    /* scalar ILU ops take the first swizzled component of C */
    {
        Inputs e = io; e.v[2] = vec(9, 4, 9, 9);
        add("ilu_scalar_swizzle", {ilu_to_d0(RCP, input("yyyy"))}, e,
            [=](const Result& r, const char* who) { EXPECT_V4(r.d0, splat(0.25f)); });
    }

    /* swizzle and negation on each slot */
    add("swizzle_neg_a", {mac_to_d0(MOV, input("wzyx", true), cnst(), input())}, io,
        [=](const Result& r, const char* who) { EXPECT_V4(r.d0, vec(-0.5f, -3.0f, 2.0f, -1.5f)); });
    add("swizzle_b", {mac_to_d0(MUL, input("xxxx"), cnst("yzwx"), input())}, io,
        [=](const Result& r, const char* who) { EXPECT_V4(r.d0, vec(0.375f, -1.5f, 6.0f, 3.0f)); });
    add("swizzle_neg_c", {mac_to_d0(ADD, input(), cnst(), cnst("zzwy", true))}, io,
        [=](const Result& r, const char* who) { EXPECT_V4(r.d0, vec(2.5f, -1.0f, -1.0f, 0.25f)); });

    /* write masks: temporaries (MAC) and outputs */
    {
        Ins i0 = mac_to_d0(MOV, input(), cnst(), input()); i0.out_mask = 0; i0.treg = 5; i0.mac_mask = WX | WZ;
        Ins i1 = mac_to_d0(MOV, temp(5), cnst(), input()); i1.out_mask = WY | WW;
        add("masks", {i0, i1}, io, [=](const Result& r, const char* who) {
            EXPECT_V4(r.d0, vec(0, 0, 0, 0));   /* r5.yw were 0, d0.xz keep 0 */
        });
        Ins j1 = mac_to_d0(MOV, temp(5, "xxzz"), cnst(), input()); j1.out_mask = WALL;
        add("masks2", {i0, j1}, io, [=](const Result& r, const char* who) {
            EXPECT_V4(r.d0, vec(1.5f, 1.5f, 3.0f, 3.0f));
        });
    }

    /* relative addressing: ARL then c[a0 + base] */
    auto rel_case = [&](const std::string& name, float a0src, unsigned base, int expect_index) {
        Inputs e = io; e.v[1] = vec(a0src, 0, 0, 0);
        for (int i = 0; i < 192; ++i) e.c[i] = vec(float(i), float(i) + 0.5f, -float(i), 1.0f);
        Ins arl; arl.mac = ARL; arl.vidx = 1; arl.s[0] = input();
        Ins mov; mov.mac = MOV; mov.cidx = base; mov.rel = true; mov.s[0] = cnst(); mov.out_mask = WALL; mov.out_addr = O_T0 + 1;
        V4 want = e.c[expect_index];
        add(name, {arl, mov}, e, [=](const Result& r, const char* who) { EXPECT_V4(r.t[1], want); });
    };
    rel_case("rel_basic", 2.7f, 50, 52);
    rel_case("rel_floor_negative", -1.5f, 50, 48);
    rel_case("rel_clamp_low", -10.0f, 3, 0);
    rel_case("rel_clamp_high", 300.0f, 100, 191);
    {
        /* ARL reads a0 after the instruction's own operand reads; the
           next instruction sees the new value. Pair ARL with an ILU. */
        Inputs e = io; e.v[1] = vec(3.0f, 0, 0, 0);
        for (int i = 0; i < 192; ++i) e.c[i] = vec(float(i), 0, 0, 1);
        Ins arl; arl.mac = ARL; arl.ilu = IMOV; arl.vidx = 1; arl.cidx = 10; arl.rel = true;
        arl.s[0] = input(); arl.s[2] = cnst(); arl.out_mask = WALL; arl.out_addr = O_D1; arl.out_ilu = true;
        Ins mov; mov.mac = MOV; mov.cidx = 10; mov.rel = true; mov.s[0] = cnst(); mov.out_mask = WALL; mov.out_addr = O_D0;
        add("rel_arl_timing", {arl, mov}, e, [=](const Result& r, const char* who) {
            EXPECT_V4(r.d1, vec(10, 0, 0, 1));   /* a0 was still 0 */
            EXPECT_V4(r.d0, vec(13, 0, 0, 1));
        });
    }

    /* paired MAC + ILU: MAC writes r3, the ILU writes r1 (not r3) */
    {
        Inputs e = io; e.v[1] = vec(1, 2, 3, 4); e.c[100] = vec(16, 0, 0, 0);
        Ins p; p.mac = MAD; p.ilu = RSQ; p.vidx = 1; p.cidx = 100;
        p.s[0] = input(); p.s[1] = input(); p.s[2] = cnst("xxxx");
        p.treg = 3; p.mac_mask = WALL; p.ilu_mask = WW;
        p.out_mask = WX; p.out_addr = O_FOG; p.out_ilu = true;    /* oFog = rsq */
        Ins o1; o1.mac = MOV; o1.s[0] = temp(3); o1.out_mask = WALL; o1.out_addr = O_D0;
        Ins o2; o2.mac = MOV; o2.s[0] = temp(1); o2.out_mask = WALL; o2.out_addr = O_D1;
        add("paired_mac_ilu", {p, o1, o2}, e, [=](const Result& r, const char* who) {
            EXPECT_V4(r.d0, vec(17, 20, 25, 32));   /* v*v + c.xxxx */
            EXPECT_V4(r.d1, vec(0, 0, 0, 0.25f));
            EXPECT_F(r.fog, 0.25f);
        });
        /* ILU alone writes the temporary register field */
        Ins q; q.ilu = RCP; q.cidx = 100; q.s[2] = cnst(); q.treg = 7; q.ilu_mask = WY;
        Ins o3; o3.mac = MOV; o3.s[0] = temp(7); o3.out_mask = WALL; o3.out_addr = O_D0;
        add("ilu_alone_temp", {q, o3}, e, [=](const Result& r, const char* who) {
            EXPECT_V4(r.d0, vec(0, 0.0625f, 0, 0));
        });
        /* MAC and ILU in one instruction write two different outputs:
           MAC -> temp, ILU -> output */
        Ins m; m.mac = DP4; m.ilu = EXP; m.vidx = 1; m.cidx = 100;
        m.s[0] = input(); m.s[1] = input(); m.s[2] = input("yyyy");
        m.treg = 0; m.mac_mask = WX; m.out_mask = WALL; m.out_addr = O_T0 + 3; m.out_ilu = true;
        Ins o4; o4.mac = MOV; o4.s[0] = temp(0); o4.out_mask = WALL; o4.out_addr = O_B0;
        add("paired_outputs", {m, o4}, e, [=](const Result& r, const char* who) {
            EXPECT_V4(r.t[3], vec(4, 0, 4, 1));
            EXPECT_V4(r.b0, vec(30, 0, 0, 0));
        });
        /* operands are read before the instruction writes: MAC writes r1
           while the ILU reads r1 */
        Ins w0; w0.mac = MOV; w0.vidx = 1; w0.s[0] = input(); w0.treg = 1; w0.mac_mask = WALL;
        Ins w1; w1.mac = MUL; w1.ilu = IMOV; w1.s[0] = temp(1); w1.s[1] = temp(1); w1.s[2] = temp(1);
        w1.treg = 1; w1.mac_mask = WALL; w1.out_mask = WALL; w1.out_addr = O_B1; w1.out_ilu = true;
        Ins w2; w2.mac = MOV; w2.s[0] = temp(1); w2.out_mask = WALL; w2.out_addr = O_D0;
        add("read_before_write", {w0, w1, w2}, e, [=](const Result& r, const char* who) {
            EXPECT_V4(r.b1, vec(1, 2, 3, 4));
            EXPECT_V4(r.d0, vec(1, 4, 9, 16));
        });
    }

    /* r12 aliases oPos, both for reads and for temporary writes */
    {
        Inputs e = io; e.v[0] = vec(10, 20, 30, 2);
        Ins p; p.mac = MOV; p.vidx = 0; p.s[0] = input(); p.out_mask = WX | WY | WZ; p.out_addr = O_POS;
        Ins q; q.mac = ADD; q.s[0] = temp(12); q.s[2] = temp(12, "wwww"); q.out_mask = WALL; q.out_addr = O_D0;
        Ins t; t.mac = MUL; t.s[0] = temp(12); t.s[1] = temp(12); t.treg = 12; t.mac_mask = WW;
        add("r12_alias", {p, q, t}, e, [=](const Result& r, const char* who) {
            EXPECT_V4(r.d0, vec(11, 21, 31, 2));   /* oPos.w was the default 1 */
            EXPECT_V4(r.pos, vec(10, 20, 30, 1));  /* r12.w = 1*1 */
        });
    }

    /* fog and point size, defaults of untouched outputs */
    {
        Inputs e = io; e.v[3] = vec(0.75f, 5, 6, 7);
        Ins f; f.mac = MOV; f.vidx = 3; f.s[0] = input(); f.out_mask = WX; f.out_addr = O_FOG;
        Ins s; s.mac = MOV; s.vidx = 3; s.s[0] = input("yyyy"); s.out_mask = WX; s.out_addr = O_PTS;
        add("fog_pts", {f, s}, e, [=](const Result& r, const char* who) {
            EXPECT_F(r.fog, 0.75f);
            EXPECT_F(r.pts, 5.0f);
            EXPECT_V4(r.pos, vec(0, 0, 0, 1));
            EXPECT_V4(r.d0, vec(0, 0, 0, 1));
            EXPECT_V4(r.b1, vec(0, 0, 0, 1));
            EXPECT_V4(r.t[2], vec(0, 0, 0, 1));
        });
        Ins n; n.mac = MOV; n.vidx = 3; n.s[0] = input(); n.out_mask = WY; n.out_addr = O_FOG;
        add("fog_untouched", {n}, e, [=](const Result& r, const char* who) {
            EXPECT_F(r.fog, 0.0f);
            EXPECT_F(r.pts, 1.0f);
        });
    }

    /* every output register */
    {
        Inputs e = io;
        std::vector<Ins> code;
        unsigned addrs[] = {O_POS, O_D0, O_D1, O_B0, O_B1, 9, 10, 11, 12};
        for (unsigned k = 0; k < 9; ++k) {
            Ins i; i.mac = MOV; i.vidx = k; i.s[0] = input(); i.out_mask = WALL; i.out_addr = addrs[k];
            code.push_back(i);
        }
        add("all_outputs", code, e, [=](const Result& r, const char* who) {
            EXPECT_V4(r.pos, e.v[0]); EXPECT_V4(r.d0, e.v[1]); EXPECT_V4(r.d1, e.v[2]);
            EXPECT_V4(r.b0, e.v[3]); EXPECT_V4(r.b1, e.v[4]);
            for (int t = 0; t < 4; ++t) EXPECT_V4(r.t[t], e.v[5 + t]);
        });
    }

    /* the final flag stops translation; start skips earlier instructions */
    {
        Inputs e = io;
        Ins a0; a0.mac = MOV; a0.vidx = 4; a0.s[0] = input(); a0.out_mask = WALL; a0.out_addr = O_D0; a0.final = true;
        Ins a1; a1.mac = MOV; a1.vidx = 5; a1.s[0] = input(); a1.out_mask = WALL; a1.out_addr = O_D1; a1.final = true;
        Ins a2; a2.mac = MOV; a2.vidx = 6; a2.s[0] = input(); a2.out_mask = WALL; a2.out_addr = O_D0;
        add("final_stops", {a0, a1, a2}, e, [=](const Result& r, const char* who) {
            EXPECT_V4(r.d0, e.v[4]);
            EXPECT_V4(r.d1, vec(0, 0, 0, 1));
        }, 0, false);
        add("start_offset", {a0, a1, a2}, e, [=](const Result& r, const char* who) {
            EXPECT_V4(r.d0, vec(0, 0, 0, 1));
            EXPECT_V4(r.d1, e.v[5]);
        }, 1, false);
        add("no_final", {a2}, e, [=](const Result& r, const char* who) {
            EXPECT_V4(r.d0, e.v[6]);
        }, 0, false);
    }

    /* a transform: oPos = M * v0 with M in c[96..99] (D3D c0..c3) */
    {
        Inputs e = io; e.v[0] = vec(1, 2, 3, 1);
        e.c[96] = vec(1, 0, 0, 5); e.c[97] = vec(0, 2, 0, 6); e.c[98] = vec(0, 0, 3, 7); e.c[99] = vec(0, 0, 0, 1);
        std::vector<Ins> code;
        for (unsigned k = 0; k < 4; ++k) {
            Ins i; i.mac = DP4; i.vidx = 0; i.cidx = 96 + k; i.s[0] = input(); i.s[1] = cnst();
            i.out_mask = 8u >> k; i.out_addr = O_POS;
            code.push_back(i);
        }
        add("transform", code, e, [=](const Result& r, const char* who) {
            EXPECT_V4(r.pos, vec(6, 10, 16, 1));
        });
    }

    /* constant-register writes (state programs) are dropped */
    {
        Inputs e = io;
        Ins i; i.mac = MOV; i.vidx = 1; i.s[0] = input(); i.out_mask = WALL; i.out_addr = 3; i.out_output = false;
        add("const_write_dropped", {i}, e, [=](const Result& r, const char* who) {
            EXPECT_V4(r.d0, vec(0, 0, 0, 1));
        });
    }
    return cases;
}

/* ------------------------------------------------------------------ */
/* Random programs                                                     */
/* ------------------------------------------------------------------ */

std::vector<Ins> random_program(std::mt19937& rng) {
    auto U = [&](unsigned n) { return unsigned(rng() % n); };
    std::vector<Ins> code;
    unsigned len = 1 + U(14);
    for (unsigned k = 0; k < len; ++k) {
        Ins i;
        i.mac = U(14);
        i.ilu = U(3) == 0 ? 0 : U(8);
        i.vidx = U(16);
        i.cidx = U(10) == 0 ? U(256) : 90 + U(20);
        for (auto& s : i.s) {
            s.mux = U(20) == 0 ? 0 : 1 + U(3);
            s.reg = U(15) == 0 ? 13 + U(3) : U(13);
            for (auto& w : s.swz) w = U(4);
            s.neg = U(2);
        }
        i.treg = U(13);
        i.mac_mask = U(16);
        i.ilu_mask = U(16);
        i.out_mask = U(16);
        static const unsigned outs[] = {0, 0, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 1, 40};
        i.out_addr = outs[U(14)];
        i.out_output = U(10) != 0;
        i.out_ilu = U(2);
        i.rel = U(4) == 0;
        code.push_back(i);
    }
    return code;
}

} // namespace

int main(int argc, char** argv) {
    const char* tmp = std::getenv("TMPDIR");
    g_work = argc > 1 ? argv[1] : std::string(tmp ? tmp : "/tmp") + "/h2_vp_test";
    run("mkdir -p '" + g_work + "'");

    /* error cases */
    {
        std::uint32_t w[4] = {0, 0, 0, 1};
        CHECK(!translate_vertex_program(w, 1, 1).error.empty(), "start past the end");
        CHECK(!translate_vertex_program(nullptr, 0, 0).error.empty(), "empty program");
        Ins bad; bad.mac = 14; bad.final = true;
        auto words = encode_all({bad});
        CHECK(!translate_vertex_program(words.data(), 1, 0).error.empty(), "MAC opcode 14");
    }

    /* encoder/decoder round trip on random field values */
    {
        std::mt19937 rng(7);
        for (int n = 0; n < 2000; ++n) {
            Ins i = random_program(rng)[0];
            i.final = rng() & 1;
            std::uint32_t w[4]; encode(i, w);
            std::uint32_t w2[4]; encode(decode(w), w2);
            CHECK(std::memcmp(w, w2, 16) == 0, "round trip %d", n);
        }
    }

    /* attributes_read: only operands the opcodes actually use count */
    {
        Ins a; a.mac = MUL; a.vidx = 3; a.s[0] = input(); a.s[1] = cnst(); a.s[2] = input(); a.out_mask = WALL;
        Ins b; b.mac = MOV; b.vidx = 7; b.s[0] = temp(0); b.s[1] = input(); b.s[2] = input(); b.out_mask = WALL;
        Ins c; c.ilu = RCP; c.vidx = 9; c.s[0] = input(); c.s[2] = input(); c.out_mask = WALL; c.out_ilu = true;
        auto words = encode_all({a, b, c});
        auto info = translate_vertex_program(words.data(), 3, 0);
        CHECK(info.attributes_read == ((1u << 3) | (1u << 9)), "attributes_read = 0x%x", info.attributes_read);
    }

    /* hand-written cases */
    std::vector<Case> cases = make_cases();
    std::vector<Program> progs;
    for (auto& c : cases) {
        c.prog.info = translate_vertex_program(c.prog.words.data(), c.prog.words.size() / 4, c.prog.start);
        CHECK(c.prog.info.error.empty(), "%s: %s", c.prog.name.c_str(), c.prog.info.error.c_str());
        progs.push_back(c.prog);
    }
    void* lib = build_cpu(progs, "cases");
    for (size_t i = 0; i < cases.size(); ++i) {
        auto& c = cases[i];
        Result ri = interpret(c.prog.words.data(), c.prog.words.size() / 4, c.prog.start, c.in);
        c.expect(ri, (c.prog.name + " [interpreter]").c_str());
        if (lib) {
            Result rg = run_cpu(lib, i, c.in);
            c.expect(rg, (c.prog.name + " [generated]").c_str());
            std::string why;
            CHECK(same(ri, rg, why), "%s: interpreter vs generated: %s", c.prog.name.c_str(), why.c_str());
        }
        metal_single(c.prog);
    }
    std::printf("%zu hand-written programs checked\n", cases.size());

    /* random programs: interpreter vs generated code */
    {
        std::mt19937 rng(12345);
        std::vector<Program> fuzz;
        for (int n = 0; n < 300; ++n) {
            Program p;
            p.name = "fuzz" + std::to_string(n);
            p.words = encode_all(random_program(rng));
            p.info = translate_vertex_program(p.words.data(), p.words.size() / 4, 0);
            if (!p.info.error.empty()) continue;   /* MAC 14/15 never generated */
            fuzz.push_back(std::move(p));
        }
        void* flib = build_cpu(fuzz, "fuzz");
        int mismatches = 0;
        if (flib) {
            std::mt19937 vr(99);
            std::uniform_real_distribution<float> d(-3.0f, 3.0f);
            for (size_t i = 0; i < fuzz.size(); ++i) {
                for (int trial = 0; trial < 4; ++trial) {
                    Inputs io;
                    for (auto& v : io.v) for (auto& x : v.v) x = d(vr);
                    for (auto& c : io.c) for (auto& x : c.v) x = d(vr);
                    if (trial == 3) io.v[0].v[0] = 0.0f;   /* special values */
                    Result a = interpret(fuzz[i].words.data(), fuzz[i].words.size() / 4, 0, io);
                    Result b = run_cpu(flib, i, io);
                    std::string why;
                    if (!same(a, b, why)) {
                        ++mismatches;
                        CHECK(false, "%s trial %d: %s", fuzz[i].name.c_str(), trial, why.c_str());
                    }
                }
            }
        }
        metal_batch(fuzz, "fuzz");
        std::printf("%zu random programs checked (%d mismatches)\n", fuzz.size(), mismatches);
    }

    std::printf("%d checks, %d failures\n", g_checks, g_failures);
    return g_failures ? 1 : 0;
}
