/* Standalone tests of the pixel shader translator (port/src/gpu/pixel_shader.cpp).
 *
 * Build and run:
 *   clang++ -std=c++20 -I port/src/gpu -I port/include \
 *       port/src/gpu/pixel_shader.cpp port/tests/gpu/test_pixel_shader.cpp -o $TMP/test_ps
 *   $TMP/test_ps [work-dir]
 *
 * What it checks:
 *   1. Every definition (hand-built feature cases, the game's own definitions
 *      from src/unknown_020560.cpp, and seeded random ones) translates, and
 *      prelude + source + a small fragment entry point compiles with
 *      `xcrun metal -std=metal3.0`.
 *   2. Structure: the generated source contains the expressions an
 *      independent decoding of the definition predicts (texture times
 *      diffuse is a product of t0 and v0, the dot3 stage is a dot product of
 *      expanded inputs, ...).
 *   3. Numbers (when `xcrun swift` is available): combiner-only definitions
 *      (all texture stages in pass-through mode, so t0..t3 are controlled
 *      inputs) run on the GPU in a compute kernel and are compared with a CPU
 *      reference evaluator written here from the NV_register_combiners
 *      specification, independently of the translator. Set
 *      H2_PS_SKIP_GPU=1 to skip.
 *
 * The bit layouts used to build definitions are the public ones cited in
 * pixel_shader.cpp (nouveau nv20_3d.xml.h / envytools rnndb). */
#include "shader_abi.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <random>
#include <sstream>
#include <string>
#include <vector>

#include <unistd.h> /* mkdtemp */

using h2::gpu::PixelShaderContext;
using h2::gpu::translate_pixel_shader;

namespace {

int g_failures = 0;
int g_checks = 0;

void check(bool ok, const std::string& what) {
    ++g_checks;
    if (!ok) {
        ++g_failures;
        std::fprintf(stderr, "FAIL: %s\n", what.c_str());
    }
}

/* ---- definition building -------------------------------------------------- */

struct Def {
    std::uint32_t w[60] = {};
    std::uint32_t& alpha_in(int s) { return w[0 + s]; }
    std::uint32_t& final_abcd() { return w[8]; }
    std::uint32_t& final_efg() { return w[9]; }
    std::uint32_t& c0(int s) { return w[10 + s]; }
    std::uint32_t& c1(int s) { return w[18 + s]; }
    std::uint32_t& alpha_out(int s) { return w[26 + s]; }
    std::uint32_t& rgb_in(int s) { return w[34 + s]; }
    std::uint32_t& compare_mode() { return w[42]; }
    std::uint32_t& final_c0() { return w[43]; }
    std::uint32_t& final_c1() { return w[44]; }
    std::uint32_t& rgb_out(int s) { return w[45 + s]; }
    std::uint32_t& combiner_count() { return w[53]; }
    std::uint32_t& texture_modes() { return w[54]; }
    std::uint32_t& dot_mapping() { return w[55]; }
    std::uint32_t& input_texture() { return w[56]; }
};

/* registers */
enum : unsigned { ZERO = 0, C0 = 1, C1 = 2, FOG = 3, V0 = 4, V1 = 5, T0 = 8, T1 = 9, T2 = 10, T3 = 11,
                  R0 = 12, R1 = 13, V1R0 = 14, EF = 15 };
/* mappings */
enum : unsigned { UID = 0, UINV = 1, EXPN = 2, EXPNEG = 3, HBN = 4, HBNEG = 5, SID = 6, SNEG = 7 };

constexpr std::uint32_t in(unsigned reg, bool alpha = false, unsigned map = UID) {
    return (reg & 0xf) | (alpha ? 0x10u : 0u) | (map << 5);
}
constexpr std::uint32_t inputs(std::uint32_t a, std::uint32_t b, std::uint32_t c, std::uint32_t d) {
    return (a << 24) | (b << 16) | (c << 8) | d;
}
struct Out {
    unsigned ab = 0, cd = 0, sum = 0;
    bool ab_dot = false, cd_dot = false, mux = false, bias = false;
    unsigned scale = 0;
    bool ab_b2a = false, cd_b2a = false;
};
std::uint32_t outputs(const Out& o) {
    return o.cd | (o.ab << 4) | (o.sum << 8) | (o.cd_dot ? 1u << 12 : 0) | (o.ab_dot ? 1u << 13 : 0) |
           (o.mux ? 1u << 14 : 0) | (o.bias ? 1u << 15 : 0) | (o.scale << 16) | (o.cd_b2a ? 1u << 18 : 0) |
           (o.ab_b2a ? 1u << 19 : 0);
}
constexpr std::uint32_t modes(unsigned m0, unsigned m1 = 0, unsigned m2 = 0, unsigned m3 = 0) {
    return m0 | (m1 << 5) | (m2 << 10) | (m3 << 15);
}

/* ---- metal compilation ------------------------------------------------------ */

std::string g_work;

const char* kFragmentWrapper = R"msl(
struct H2TestVaryings {
    float4 position [[position]];
    float4 diffuse;
    float4 specular;
    float4 tc0; float4 tc1; float4 tc2; float4 tc3;
    float fog;
};
fragment float4 h2_test_fragment(H2TestVaryings v [[stage_in]],
                                 bool ff [[front_facing]],
                                 constant H2PixelConstants &k [[buffer(0)]],
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
    in.diffuse = v.diffuse; in.specular = v.specular;
    in.texcoord[0] = v.tc0; in.texcoord[1] = v.tc1; in.texcoord[2] = v.tc2; in.texcoord[3] = v.tc3;
    in.fog = v.fog; in.front_facing = ff; in.position = v.position;
    H2Textures t;
    t.tex2d[0] = a0; t.tex2d[1] = a1; t.tex2d[2] = a2; t.tex2d[3] = a3;
    t.cube[0] = b0; t.cube[1] = b1; t.cube[2] = b2; t.cube[3] = b3;
    t.volume[0] = d0; t.volume[1] = d1; t.volume[2] = d2; t.volume[3] = d3;
    t.samplers[0] = s0; t.samplers[1] = s1; t.samplers[2] = s2; t.samplers[3] = s3;
    return h2_pixel_shader(in, k, t);
}
)msl";

bool write_file(const std::string& path, const std::string& text) {
    std::ofstream f(path, std::ios::binary);
    f << text;
    return static_cast<bool>(f);
}

std::string shell_quote(const std::string& s) {
    std::string r = "'";
    for (char c : s) {
        if (c == '\'') r += "'\\''";
        else r += c;
    }
    return r + "'";
}

bool compile_metal(const std::string& name, const std::string& source) {
    const std::string base = g_work + "/" + name;
    write_file(base + ".metal", std::string(h2::gpu::kShaderPrelude) + source + kFragmentWrapper);
    const std::string cmd = "xcrun metal -std=metal3.0 -c " + shell_quote(base + ".metal") + " -o " +
                            shell_quote(base + ".air") + " > " + shell_quote(base + ".log") + " 2>&1";
    const int rc = std::system(cmd.c_str());
    if (rc != 0) {
        std::ifstream log(base + ".log");
        std::stringstream ss;
        ss << log.rdbuf();
        std::fprintf(stderr, "metal compile failed for %s:\n%s\n", name.c_str(), ss.str().c_str());
    }
    return rc == 0;
}

bool contains(const std::string& hay, const std::string& needle) {
    return hay.find(needle) != std::string::npos;
}

int g_compiled = 0;

std::string translate_and_compile(const std::string& name, Def& d, const PixelShaderContext& ctx = {}) {
    auto info = translate_pixel_shader(d.w, ctx);
    check(info.error.empty(), name + ": translator error " + info.error);
    const bool ok = compile_metal(name, info.source);
    check(ok, name + ": compiles with xcrun metal");
    if (ok) ++g_compiled;
    return info.source;
}

/* ---- CPU reference evaluator (NV_register_combiners) ----------------------- */

struct V4 { double v[4] = {0, 0, 0, 0}; };

struct RefInputs {
    V4 diffuse, specular, tc[4];
    double fog = 0;
};
struct RefConstants {
    V4 c0[8], c1[8], final_c0, final_c1, fog_color;
};

double sat(double x) { return std::min(1.0, std::max(0.0, x)); }

/* reads one 8-bit input field: channel ch of 0..2 (rgb portion) or alpha portion */
double ref_map(double x, unsigned map) {
    const double p = std::max(0.0, x);
    switch (map) {
    case UID: return p;
    case UINV: return 1.0 - std::min(1.0, p);
    case EXPN: return 2 * p - 1;
    case EXPNEG: return -2 * p + 1;
    case HBN: return p - 0.5;
    case HBNEG: return -p + 0.5;
    case SID: return x;
    default: return -x;
    }
}

struct RefState {
    V4 reg[16];
};

/* Evaluates a definition whose texture stages are all pass-through. */
V4 ref_evaluate(const Def& dd, const RefInputs& in, const RefConstants& k) {
    Def d = dd;
    RefState st;
    for (int i = 0; i < 4; ++i)
        for (int c = 0; c < 4; ++c) st.reg[T0 + i].v[c] = sat(in.tc[i].v[c]);
    for (int c = 0; c < 4; ++c) {
        st.reg[V0].v[c] = sat(in.diffuse.v[c]);
        st.reg[V1].v[c] = sat(in.specular.v[c]);
    }
    for (int c = 0; c < 3; ++c) st.reg[FOG].v[c] = k.fog_color.v[c];
    st.reg[FOG].v[3] = sat(in.fog);
    st.reg[R0].v[3] = st.reg[T0].v[3];
    const std::uint32_t cc = d.combiner_count();
    const unsigned count = std::min(8u, cc & 0xff);
    const bool msb = (cc >> 8) & 1, u0 = (cc >> 12) & 1, u1 = (cc >> 16) & 1;
    for (unsigned s = 0; s < count; ++s) {
        RefState cur = st;
        cur.reg[C0] = k.c0[u0 ? s : 0];
        cur.reg[C1] = k.c1[u1 ? s : 0];
        RefState next = st;
        bool mux_cd = msb ? cur.reg[R0].v[3] >= 0.5
                          : (static_cast<unsigned>(std::lround(sat(cur.reg[R0].v[3]) * 255.0)) & 1) != 0;
        std::vector<std::pair<unsigned, double>> late_alpha;
        for (int portion = 0; portion < 2; ++portion) {
            const std::uint32_t iw = portion ? d.alpha_in(s) : d.rgb_in(s);
            const std::uint32_t ow = portion ? d.alpha_out(s) : d.rgb_out(s);
            double val[4][3];
            for (int j = 0; j < 4; ++j) {
                const unsigned f = (iw >> (24 - 8 * j)) & 0xff;
                const unsigned r = f & 0xf;
                const bool a = (f >> 4) & 1;
                for (int c = 0; c < 3; ++c) {
                    double x;
                    if (r == ZERO || r == 6 || r == 7 || r == V1R0 || r == EF) x = 0;
                    else if (portion) x = cur.reg[r].v[a ? 3 : 2];
                    else x = cur.reg[r].v[a ? 3 : c];
                    val[j][c] = (r == ZERO || r == 6 || r == 7 || r == V1R0 || r == EF)
                                    ? ref_map(0.0, f >> 5) : ref_map(x, f >> 5);
                }
            }
            const int n = portion ? 1 : 3;
            double ab[3], cd[3], sum[3];
            const bool abdot = !portion && ((ow >> 13) & 1), cddot = !portion && ((ow >> 12) & 1);
            double dab = 0, dcd = 0;
            for (int c = 0; c < 3; ++c) { dab += val[0][c] * val[1][c]; dcd += val[2][c] * val[3][c]; }
            for (int c = 0; c < n; ++c) {
                ab[c] = abdot ? dab : val[0][c] * val[1][c];
                cd[c] = cddot ? dcd : val[2][c] * val[3][c];
                sum[c] = ((ow >> 14) & 1) ? (mux_cd ? cd[c] : ab[c]) : ab[c] + cd[c];
            }
            auto omap = [&](double x) {
                if ((ow >> 15) & 1) x -= 0.5;
                switch ((ow >> 16) & 3) {
                case 1: x *= 2; break;
                case 2: x *= 4; break;
                case 3: x *= 0.5; break;
                }
                return std::min(1.0, std::max(-1.0, x));
            };
            const unsigned dst[3] = {(ow >> 4) & 0xf, ow & 0xf, (ow >> 8) & 0xf};
            const double* src[3] = {ab, cd, sum};
            for (int o = 0; o < 3; ++o) {
                const unsigned r = dst[o];
                if (!(r == V0 || r == V1 || (r >= T0 && r <= R1))) continue;
                if (portion) next.reg[r].v[3] = omap(src[o][0]);
                else for (int c = 0; c < 3; ++c) next.reg[r].v[c] = omap(src[o][c]);
            }
            if (!portion) {
                if (((ow >> 19) & 1) && (dst[0] == V0 || dst[0] == V1 || (dst[0] >= T0 && dst[0] <= R1)))
                    late_alpha.push_back({dst[0], omap(ab[2])});
                if (((ow >> 18) & 1) && (dst[1] == V0 || dst[1] == V1 || (dst[1] >= T0 && dst[1] <= R1)))
                    late_alpha.push_back({dst[1], omap(cd[2])});
            }
        }
        for (auto& [r, v] : late_alpha) next.reg[r].v[3] = v;
        st = next;
    }
    /* final combiner */
    RefState cur = st;
    cur.reg[C0] = k.final_c0;
    cur.reg[C1] = k.final_c1;
    const std::uint32_t abcd = d.final_abcd(), efg = d.final_efg();
    const bool clampsum = (efg >> 7) & 1;
    for (int c = 0; c < 3; ++c) {
        const double s = cur.reg[R0].v[c] + cur.reg[V1].v[c];
        cur.reg[V1R0].v[c] = clampsum ? sat(s) : std::max(0.0, s);
    }
    auto fin = [&](unsigned f, int c, bool alpha_portion) {
        const unsigned r = f & 0xf;
        const bool a = (f >> 4) & 1;
        double x;
        if (r == ZERO || r == 6 || r == 7) x = 0;
        else if (r == V1R0 || r == EF) x = (a || alpha_portion) ? 0 : cur.reg[r].v[c];
        else if (alpha_portion) x = cur.reg[r].v[a ? 3 : 2];
        else x = cur.reg[r].v[a ? 3 : c];
        return ref_map(x, f >> 5);
    };
    for (int c = 0; c < 3; ++c) {
        const unsigned fe = (efg >> 24) & 0xff, ff = (efg >> 16) & 0xff;
        const double e = (fe & 0xf) == EF ? 0 : fin(fe, c, false);
        const double f = (ff & 0xf) == EF ? 0 : fin(ff, c, false);
        cur.reg[EF].v[c] = e * f;
    }
    V4 out;
    for (int c = 0; c < 3; ++c) {
        const double A = fin((abcd >> 24) & 0xff, c, false), B = fin((abcd >> 16) & 0xff, c, false);
        const double C = fin((abcd >> 8) & 0xff, c, false), D = fin(abcd & 0xff, c, false);
        out.v[c] = sat(A * B + (1 - A) * C + D);
    }
    out.v[3] = sat(fin((efg >> 8) & 0xff, 3, true));
    return out;
}

/* ---- GPU numeric comparison (via a Swift + Metal runner) ------------------- */

struct GpuJob {
    std::string name;
    Def def;
    std::vector<RefInputs> inputs;
    RefConstants k;
};

const char* kKernelWrapper = R"msl(
struct H2TestIn { float4 diffuse; float4 specular; float4 tc[4]; float4 fog; };
kernel void h2_test_kernel(device const H2TestIn *ins [[buffer(0)]],
                           constant H2PixelConstants &k [[buffer(1)]],
                           device float4 *outs [[buffer(2)]],
                           uint id [[thread_position_in_grid]])
{
    H2PixelInput in;
    in.diffuse = ins[id].diffuse; in.specular = ins[id].specular;
    for (int i = 0; i < 4; ++i) in.texcoord[i] = ins[id].tc[i];
    in.fog = ins[id].fog.x; in.front_facing = true; in.position = float4(0.0);
    H2Textures t;
    outs[id] = h2_pixel_shader(in, k, t);
}
)msl";

const char* kSwiftRunner = R"swift(
import Metal
import Foundation
let manifest = try! String(contentsOfFile: CommandLine.arguments[1], encoding: .utf8)
let device = MTLCreateSystemDefaultDevice()!
let queue = device.makeCommandQueue()!
for line in manifest.split(separator: "\n") {
    let f = line.split(separator: " ").map(String.init)
    let (src, inPath, kPath, outPath, n) = (f[0], f[1], f[2], f[3], Int(f[4])!)
    do {
        let lib = try device.makeLibrary(source: try String(contentsOfFile: src, encoding: .utf8), options: nil)
        let pso = try device.makeComputePipelineState(function: lib.makeFunction(name: "h2_test_kernel")!)
        let inData = try Data(contentsOf: URL(fileURLWithPath: inPath))
        let kData = try Data(contentsOf: URL(fileURLWithPath: kPath))
        let inBuf = inData.withUnsafeBytes { device.makeBuffer(bytes: $0.baseAddress!, length: inData.count)! }
        let kBuf = kData.withUnsafeBytes { device.makeBuffer(bytes: $0.baseAddress!, length: kData.count)! }
        let outBuf = device.makeBuffer(length: n * 16)!
        let cb = queue.makeCommandBuffer()!
        let enc = cb.makeComputeCommandEncoder()!
        enc.setComputePipelineState(pso)
        enc.setBuffer(inBuf, offset: 0, index: 0)
        enc.setBuffer(kBuf, offset: 0, index: 1)
        enc.setBuffer(outBuf, offset: 0, index: 2)
        enc.dispatchThreads(MTLSize(width: n, height: 1, depth: 1), threadsPerThreadgroup: MTLSize(width: 1, height: 1, depth: 1))
        enc.endEncoding()
        cb.commit()
        cb.waitUntilCompleted()
        try Data(bytes: outBuf.contents(), count: n * 16).write(to: URL(fileURLWithPath: outPath))
    } catch {
        print("runner error for \(src): \(error)")
    }
}
)swift";

void put4(std::vector<float>& b, const V4& v) {
    for (double x : v.v) b.push_back(static_cast<float>(x));
}

void run_gpu_jobs(std::vector<GpuJob>& jobs) {
    if (const char* skip = std::getenv("H2_PS_SKIP_GPU"); skip && *skip == '1') {
        std::printf("GPU numeric comparison skipped (H2_PS_SKIP_GPU=1)\n");
        return;
    }
    std::string manifest;
    for (auto& j : jobs) {
        const std::string base = g_work + "/gpu_" + j.name;
        auto info = translate_pixel_shader(j.def.w, PixelShaderContext{});
        write_file(base + ".metal", std::string(h2::gpu::kShaderPrelude) + info.source + kKernelWrapper);
        std::vector<float> ib;
        for (auto& x : j.inputs) {
            put4(ib, x.diffuse); put4(ib, x.specular);
            for (auto& t : x.tc) put4(ib, t);
            V4 f; f.v[0] = x.fog; put4(ib, f);
        }
        std::vector<float> kb;
        for (auto& c : j.k.c0) put4(kb, c);
        for (auto& c : j.k.c1) put4(kb, c);
        put4(kb, j.k.final_c0); put4(kb, j.k.final_c1); put4(kb, j.k.fog_color);
        for (int i = 0; i < 12; ++i) put4(kb, V4{}); /* bump_env, bump_luminance, color_key */
        write_file(base + ".in", std::string(reinterpret_cast<char*>(ib.data()), ib.size() * 4));
        write_file(base + ".k", std::string(reinterpret_cast<char*>(kb.data()), kb.size() * 4));
        std::remove((base + ".out").c_str());
        manifest += base + ".metal " + base + ".in " + base + ".k " + base + ".out " +
                    std::to_string(j.inputs.size()) + "\n";
    }
    write_file(g_work + "/manifest.txt", manifest);
    write_file(g_work + "/runner.swift", kSwiftRunner);
    const std::string cmd = "xcrun swift " + shell_quote(g_work + "/runner.swift") + " " +
                            shell_quote(g_work + "/manifest.txt") + " > " + shell_quote(g_work + "/runner.log") +
                            " 2>&1";
    if (std::system(cmd.c_str()) != 0) {
        std::printf("GPU numeric comparison unavailable (xcrun swift failed; see %s/runner.log)\n",
                    g_work.c_str());
        return;
    }
    int compared = 0;
    for (auto& j : jobs) {
        std::ifstream f(g_work + "/gpu_" + j.name + ".out", std::ios::binary);
        std::vector<float> out(j.inputs.size() * 4);
        f.read(reinterpret_cast<char*>(out.data()), static_cast<std::streamsize>(out.size() * 4));
        check(static_cast<bool>(f), j.name + ": GPU result present");
        if (!f) continue;
        int bad = 0;
        for (std::size_t n = 0; n < j.inputs.size(); ++n) {
            const V4 ref = ref_evaluate(j.def, j.inputs[n], j.k);
            for (int c = 0; c < 4; ++c) {
                if (std::fabs(ref.v[c] - out[n * 4 + c]) > 2e-4) {
                    if (bad++ < 3)
                        std::fprintf(stderr, "%s: sample %zu channel %d: gpu %.6f ref %.6f\n", j.name.c_str(), n,
                                     c, out[n * 4 + c], ref.v[c]);
                }
            }
        }
        check(bad == 0, j.name + ": GPU result matches the CPU reference");
        ++compared;
    }
    std::printf("GPU numeric comparison: %d definitions x %zu samples\n", compared,
                jobs.empty() ? 0 : jobs[0].inputs.size());
}

/* ---- the cases -------------------------------------------------------------- */

constexpr std::uint32_t kPassAll = modes(4, 4, 4, 4);

std::mt19937 g_rng(0x48324732);

double rnd(double lo, double hi) { return std::uniform_real_distribution<double>(lo, hi)(g_rng); }
unsigned rndu(unsigned n) { return std::uniform_int_distribution<unsigned>(0, n - 1)(g_rng); }

RefInputs random_inputs() {
    RefInputs x;
    for (int c = 0; c < 4; ++c) {
        x.diffuse.v[c] = rnd(0, 1);
        x.specular.v[c] = rnd(0, 1);
        for (auto& t : x.tc) t.v[c] = rnd(-0.2, 1.2);
    }
    x.fog = rnd(0, 1);
    return x;
}

RefConstants random_constants() {
    RefConstants k;
    auto r = [] { V4 v; for (auto& x : v.v) x = std::round(rnd(0, 255)) / 255.0; return v; };
    for (auto& c : k.c0) c = r();
    for (auto& c : k.c1) c = r();
    k.final_c0 = r(); k.final_c1 = r(); k.fog_color = r();
    return k;
}

std::uint32_t random_general_inputs() {
    static const unsigned regs[] = {ZERO, C0, C1, FOG, V0, V1, T0, T1, T2, T3, R0, R1};
    std::uint32_t w = 0;
    for (int j = 0; j < 4; ++j) w = (w << 8) | in(regs[rndu(12)], rndu(2), rndu(8));
    return w;
}
std::uint32_t random_outputs(bool rgb) {
    static const unsigned dsts[] = {ZERO, ZERO, V0, V1, T0, T1, T2, T3, R0, R0, R1, R1};
    Out o;
    o.ab = dsts[rndu(12)];
    o.cd = dsts[rndu(12)];
    o.sum = dsts[rndu(12)];
    o.ab_dot = rgb && rndu(4) == 0;
    o.cd_dot = rgb && rndu(4) == 0;
    o.mux = rndu(3) == 0;
    o.bias = rndu(3) == 0;
    o.scale = rndu(4);
    o.ab_b2a = rgb && rndu(5) == 0;
    o.cd_b2a = rgb && rndu(5) == 0;
    return outputs(o);
}
Def random_combiner_def() {
    Def d;
    d.texture_modes() = kPassAll;
    const unsigned count = 1 + rndu(8);
    d.combiner_count() = count | (rndu(2) << 8) | (rndu(2) << 12) | (rndu(2) << 16);
    for (unsigned s = 0; s < count; ++s) {
        d.rgb_in(s) = random_general_inputs();
        d.alpha_in(s) = random_general_inputs();
        d.rgb_out(s) = random_outputs(true);
        d.alpha_out(s) = random_outputs(false);
    }
    static const unsigned fregs[] = {ZERO, C0, C1, FOG, V0, V1, T0, T1, T2, T3, R0, R1, V1R0, EF};
    std::uint32_t abcd = 0;
    for (int j = 0; j < 4; ++j) abcd = (abcd << 8) | in(fregs[rndu(14)], rndu(4) == 0, rndu(2));
    d.final_abcd() = abcd;
    d.final_efg() = inputs(in(fregs[rndu(13)], rndu(4) == 0, rndu(2)), in(fregs[rndu(13)], rndu(4) == 0, rndu(2)),
                           in(fregs[rndu(12)], rndu(2), rndu(2)), 0) |
                    (rndu(2) << 7);
    return d;
}

} // namespace

int main(int argc, char** argv) {
    if (argc > 1) g_work = argv[1];
    else {
        char tmpl[] = "/tmp/h2_pixel_shader_test_XXXXXX";
        const char* dir = mkdtemp(tmpl);
        if (!dir) { std::perror("mkdtemp"); return 2; }
        g_work = dir;
    }
    std::printf("work dir: %s\n", g_work.c_str());
    std::vector<GpuJob> jobs;

    /* 1. texture times diffuse, final combiner only */
    {
        Def d;
        d.texture_modes() = modes(1);
        d.final_abcd() = inputs(in(T0), in(V0), in(ZERO), in(ZERO));
        d.final_efg() = inputs(0, 0, in(T0, true), 0);
        auto s = translate_and_compile("tex_times_diffuse_final", d);
        check(contains(s, "t0 = t.tex2d[0].sample(t.samplers[0], in.texcoord[0].xy / in.texcoord[0].w)"),
              "tex_times_diffuse_final: projected 2D sample of stage 0");
        check(contains(s, "float3 out_rgb = float3((t0.rgb * v0.rgb));"), "tex_times_diffuse_final: rgb = t0 * v0");
        check(contains(s, "float out_a = float(t0.a);"), "tex_times_diffuse_final: alpha = t0.a");
        /* numeric: same with pass-through in place of the sample */
        GpuJob j{"tex_times_diffuse", d, {}, random_constants()};
        j.def.texture_modes() = kPassAll;
        for (int n = 0; n < 32; ++n) j.inputs.push_back(random_inputs());
        jobs.push_back(j);
    }
    /* 2. texture times diffuse in a general combiner stage, final outputs r0 */
    {
        Def d;
        d.texture_modes() = modes(1);
        d.combiner_count() = 1;
        d.rgb_in(0) = inputs(in(T0), in(V0), 0, 0);
        d.alpha_in(0) = inputs(in(T0, true), in(V0, true), 0, 0);
        d.rgb_out(0) = outputs({.ab = R0});
        d.alpha_out(0) = outputs({.ab = R0});
        d.final_abcd() = inputs(0, 0, 0, in(R0));
        d.final_efg() = inputs(0, 0, in(R0, true), 0);
        auto s = translate_and_compile("tex_times_diffuse_stage", d);
        check(contains(s, "float3 s0c_ab = float3((t0.rgb * v0.rgb));"), "tex_times_diffuse_stage: rgb AB = t0*v0");
        check(contains(s, "float s0a_ab = float((t0.a * v0.a));"), "tex_times_diffuse_stage: alpha AB = t0.a*v0.a");
        check(contains(s, "r0.rgb = s0c_ab_o;") && contains(s, "r0.a = s0a_ab_o;"),
              "tex_times_diffuse_stage: AB written to r0");
        check(contains(s, "float3 out_rgb = float3(max(r0.rgb, 0.0));"), "tex_times_diffuse_stage: final D = r0");
        GpuJob j{"tex_times_diffuse_stage", d, {}, random_constants()};
        j.def.texture_modes() = kPassAll;
        for (int n = 0; n < 32; ++n) j.inputs.push_back(random_inputs());
        jobs.push_back(j);
    }
    /* 3. two-stage lerp: r0 = lerp(t0, t1, v0.a); r0 = r0 * c0 (per-stage constants) */
    {
        Def d;
        d.texture_modes() = modes(1, 1);
        d.combiner_count() = 2 | 0x1000;
        d.rgb_in(0) = inputs(in(T0), in(V0, true, UINV), in(T1), in(V0, true));
        d.rgb_out(0) = outputs({.sum = R0});
        d.rgb_in(1) = inputs(in(R0), in(C0), 0, 0);
        d.rgb_out(1) = outputs({.ab = R0});
        d.final_abcd() = inputs(0, 0, 0, in(R0));
        d.final_efg() = inputs(0, 0, in(ZERO, false, UINV), 0);
        auto s = translate_and_compile("two_stage_lerp", d);
        check(contains(s, "s0c_ab = float3((t0.rgb * (1.0 - v0.aaa)))"), "two_stage_lerp: t0 * (1 - v0.a)");
        check(contains(s, "s0c_cd = float3((t1.rgb * v0.aaa))"), "two_stage_lerp: t1 * v0.a");
        check(contains(s, "s0c_sum = s0c_ab + s0c_cd"), "two_stage_lerp: sum");
        check(contains(s, "k.c0[1].rgb"), "two_stage_lerp: stage 1 reads its own c0");
        check(contains(s, "float out_a = float(1.0);"), "two_stage_lerp: alpha = 1");
        GpuJob j{"two_stage_lerp", d, {}, random_constants()};
        j.def.texture_modes() = kPassAll;
        for (int n = 0; n < 32; ++n) j.inputs.push_back(random_inputs());
        jobs.push_back(j);
    }
    /* 4. dot3 bump: r0 = dot(expand(t0), expand(v0)) with blue to alpha, times t1 */
    {
        Def d;
        d.texture_modes() = modes(1, 1);
        d.combiner_count() = 2;
        d.rgb_in(0) = inputs(in(T0, false, EXPN), in(V0, false, EXPN), 0, 0);
        d.rgb_out(0) = outputs({.ab = R0, .ab_dot = true, .ab_b2a = true});
        d.rgb_in(1) = inputs(in(R0), in(T1), 0, 0);
        d.rgb_out(1) = outputs({.ab = R0, .scale = 1});
        d.final_abcd() = inputs(0, 0, 0, in(R0));
        d.final_efg() = inputs(0, 0, in(R0, true), 0);
        auto s = translate_and_compile("dot3_bump", d);
        check(contains(s, "float3(dot(float3((2.0 * t0.rgb - 1.0)), float3((2.0 * v0.rgb - 1.0))))"),
              "dot3_bump: dot of expanded t0 and v0");
        check(contains(s, "r0.a = s0c_ab_o.b;"), "dot3_bump: blue to alpha");
        check(contains(s, "* 2.0)"), "dot3_bump: scale by two");
        GpuJob j{"dot3_bump", d, {}, random_constants()};
        j.def.texture_modes() = kPassAll;
        for (int n = 0; n < 32; ++n) j.inputs.push_back(random_inputs());
        jobs.push_back(j);
    }
    /* 5. cubemap reflection (texm3x3vspec): t0 normal map, 1-2 dot, 3 reflect */
    {
        Def d;
        d.texture_modes() = modes(1, 0x11, 0x11, 0x0c);
        d.dot_mapping() = (3 << 4) | (3 << 8) | (3 << 12);
        d.final_abcd() = inputs(0, 0, 0, in(T3));
        d.final_efg() = inputs(0, 0, in(T3, true), 0);
        PixelShaderContext ctx;
        ctx.texture_dimension[0] = 2;
        ctx.texture_dimension[3] = 6;
        auto s = translate_and_compile("cubemap_reflection", d, ctx);
        check(contains(s, "dp1 = dot(in.texcoord[1].xyz, (2.0 * t0.rgb - 1.0));"), "cubemap_reflection: dp1");
        check(contains(s, "dp3 = dot(in.texcoord[3].xyz, (2.0 * t0.rgb - 1.0));"), "cubemap_reflection: dp3");
        check(contains(s, "float3 eye = float3(in.texcoord[1].w, in.texcoord[2].w, in.texcoord[3].w);"),
              "cubemap_reflection: eye from q");
        check(contains(s, "t3 = t.cube[3].sample(t.samplers[3], rfl);"), "cubemap_reflection: cube sample");
    }
    /* 6. plain cube map and volume texture modes */
    {
        Def d;
        d.texture_modes() = modes(3, 2);
        d.final_abcd() = inputs(in(T0), in(T1), 0, 0);
        auto s = translate_and_compile("cube_and_volume", d);
        check(contains(s, "t0 = t.cube[0].sample(t.samplers[0], in.texcoord[0].xyz);"), "cube_and_volume: cube");
        check(contains(s, "t1 = t.volume[1].sample(t.samplers[1], in.texcoord[1].xyz / in.texcoord[1].w);"),
              "cube_and_volume: volume");
    }
    /* 7. final-combiner fog: rgb = lerp(fog.rgb, r0, fog.a); plus the v1+r0 sum and E*F */
    {
        Def d;
        d.texture_modes() = modes(1);
        d.combiner_count() = 1;
        d.rgb_in(0) = inputs(in(T0), in(V0), 0, 0);
        d.rgb_out(0) = outputs({.ab = R0});
        d.final_abcd() = inputs(in(FOG, true), in(R0), in(FOG), 0);
        d.final_efg() = inputs(0, 0, in(T0, true), 0);
        auto s = translate_and_compile("final_fog", d);
        check(contains(s, "float4 fog = float4(k.fog_color.rgb, saturate(in.fog));"), "final_fog: fog register");
        check(contains(s, "((fog.aaa * max(r0.rgb, 0.0)) + ((1.0 - fog.aaa) * fog.rgb))"),
              "final_fog: fog.a * r0 + (1 - fog.a) * fog.rgb");
        GpuJob j{"final_fog", d, {}, random_constants()};
        j.def.texture_modes() = kPassAll;
        for (int n = 0; n < 32; ++n) j.inputs.push_back(random_inputs());
        jobs.push_back(j);

        Def e = d;
        e.final_abcd() = inputs(in(EF), in(V1R0), 0, in(V1R0));
        e.final_efg() = inputs(in(T0), in(C0), in(C1, true), 0) | 0x80;
        auto s2 = translate_and_compile("final_ef_v1r0", e);
        check(contains(s2, "float4 v1r0 = float4(saturate(r0.rgb + v1.rgb), 0.0);"), "final_ef_v1r0: clamped sum");
        check(contains(s2, "float4 ef = float4(float3((t0.rgb * k.final_c0.rgb)), 0.0);"), "final_ef_v1r0: E*F");
        GpuJob j2{"final_ef_v1r0", e, {}, random_constants()};
        j2.def.texture_modes() = kPassAll;
        for (int n = 0; n < 32; ++n) j2.inputs.push_back(random_inputs());
        jobs.push_back(j2);
    }
    /* 8. mux, MSB and LSB */
    for (int msb = 0; msb < 2; ++msb) {
        Def d;
        d.texture_modes() = kPassAll;
        d.combiner_count() = 1 | (msb << 8);
        d.rgb_in(0) = inputs(in(T1), in(ZERO, false, UINV), in(T2), in(ZERO, false, UINV));
        d.rgb_out(0) = outputs({.sum = R0, .mux = true});
        d.alpha_in(0) = inputs(in(T1, true), in(ZERO, false, UINV), in(T2, true), in(ZERO, false, UINV));
        d.alpha_out(0) = outputs({.sum = R0, .mux = true});
        d.final_abcd() = inputs(0, 0, 0, in(R0));
        d.final_efg() = inputs(0, 0, in(R0, true), 0);
        const std::string name = msb ? "mux_msb" : "mux_lsb";
        auto s = translate_and_compile(name, d);
        check(contains(s, msb ? "bool mux = r0.a >= 0.5;" : "& 1u) != 0u"), name + ": mux condition");
        GpuJob j{name, d, {}, random_constants()};
        for (int n = 0; n < 32; ++n) j.inputs.push_back(random_inputs());
        jobs.push_back(j);
    }
    /* 9. the remaining texture modes */
    {
        struct M { const char* name; std::uint32_t modes; std::uint32_t extra; const char* expect; };
        const M ms[] = {
            {"passthrough", modes(4), 0, "t0 = saturate(in.texcoord[0]);"},
            {"clipplane", modes(5, 1), 0x0005,
             "if (in.texcoord[0].x >= 0.0 || in.texcoord[0].y < 0.0 || in.texcoord[0].z >= 0.0 || in.texcoord[0].w < 0.0) discard_fragment();"},
            {"bumpenvmap", modes(1, 6), 0, "float2 uv = in.texcoord[1].xy + float2(bm.x * duv.x + bm.z * duv.y, bm.y * duv.x + bm.w * duv.y);"},
            {"bumpenvmap_lum", modes(1, 7), 0, "t1.rgb *= saturate(k.bump_luminance[1].x * t0.b + k.bump_luminance[1].y);"},
            {"brdf", modes(1, 1, 8), 0, "BRDF"},
            {"dot_st", modes(1, 0x11, 9), 0, "t2 = t.tex2d[2].sample(t.samplers[2], float2(dp1, dp2));"},
            {"dot_zw", modes(1, 0x11, 0xa), 0, "depth replace"},
            {"dot_rflct_diff", modes(1, 0x11, 0xb, 0x0c), 0, "t2 = t.cube[2].sample(t.samplers[2], float3(dp1, dp2, dp3));"},
            {"dot_str_3d", modes(1, 0x11, 0x11, 0xd), 0, "t3 = t.volume[3].sample(t.samplers[3], float3(dp1, dp2, dp3));"},
            {"dot_str_cube", modes(1, 0x11, 0x11, 0xe), 0, "t3 = t.cube[3].sample(t.samplers[3], float3(dp1, dp2, dp3));"},
            {"dot_rflct_spec_const", modes(1, 0x11, 0x11, 0x12), 0, "constant eye: fallback"},
            {"dependent_ar", modes(1, 0xf), 0, "t1 = t.tex2d[1].sample(t.samplers[1], t0.ar);"},
            {"dependent_gb", modes(1, 1, 0x10), 0, "t2 = t.tex2d[2].sample(t.samplers[2], t1.gb);"},
            {"invalid_stage0_bump", modes(6), 0, "is not valid in this stage"},
        };
        for (const M& m : ms) {
            Def d;
            d.texture_modes() = m.modes;
            if (std::string(m.name) == "clipplane") d.compare_mode() = m.extra;
            if (std::string(m.name) == "dependent_gb") d.input_texture() = 1u << 16; /* stage 2 reads t1 */
            d.final_abcd() = inputs(in(T0), in(T1), in(T2), in(T3));
            d.final_efg() = inputs(0, 0, in(T3, true), 0);
            auto s = translate_and_compile(m.name, d);
            check(contains(s, m.expect), std::string(m.name) + ": expected code");
        }
        /* each dot mapping value */
        for (unsigned map = 0; map < 8; ++map) {
            Def d;
            d.texture_modes() = modes(1, 0x11, 9);
            d.dot_mapping() = (map << 4) | (map << 8);
            d.final_abcd() = inputs(0, 0, 0, in(T2));
            translate_and_compile("dot_mapping_" + std::to_string(map), d);
        }
        /* signed texture context */
        Def d;
        d.texture_modes() = modes(1, 0x11, 9);
        d.dot_mapping() = (1 << 4) | (1 << 8);
        d.final_abcd() = inputs(in(T0), in(T2), 0, 0);
        PixelShaderContext ctx;
        ctx.texture_signed[0] = true;
        auto s = translate_and_compile("signed_texture", d, ctx);
        check(contains(s, "dp1 = dot(in.texcoord[1].xyz, t0.rgb);"), "signed_texture: no remap of signed data");
        check(contains(s, "max(t0.rgb, 0.0)"), "signed_texture: unsigned identity clamps signed t0");
    }
    /* 10. the game's own definitions (src/unknown_020560.cpp) */
    {
        /* function_020f30 with b > 0 and a > 0 */
        Def d;
        d.texture_modes() = 0x8421;
        d.alpha_in(0) = 0x18201920; d.alpha_out(0) = 0x30c00;
        d.alpha_in(1) = 0x1a201b20; d.alpha_out(1) = 0x30d00;
        d.alpha_in(2) = 0x1c121d12; d.alpha_out(2) = 0xc00;
        d.final_efg() = 0x1c00;
        d.c1(2) = 0x80000000u;
        const std::uint32_t rgbi[8] = {0x8200940, 0xcc012020, 0xc082c09, 0xa200b40,
                                       0xcd012020, 0xd0a2d0b, 0xc200d40, 0xc4012020};
        const std::uint32_t rgbo[8] = {0x20c00, 0x30c00, 0xc00, 0x20d00, 0x30d00, 0xd00, 0x20400, 0x30400};
        for (int s = 0; s < 8; ++s) { d.rgb_in(s) = rgbi[s]; d.rgb_out(s) = rgbo[s]; }
        d.final_abcd() = 0x40c0d00;
        d.combiner_count() = 8 | 0x11000;
        auto s = translate_and_compile("game_020f30_full", d);
        check(contains(s, "k.c0[1].rgb") && contains(s, "k.c0[4].rgb") && contains(s, "k.c0[7].rgb"),
              "game_020f30_full: per-stage c0 at stages 1, 4, 7");
        check(contains(s, "k.c1[2].a"), "game_020f30_full: per-stage c1 at stage 2");
        check(contains(s, "float3((t0.rgb + (t1.rgb * -1.0)))") || contains(s, "(t1.rgb * -1.0)"),
              "game_020f30_full: stage 0 is t0 - t1");
        /* v0 is written by stages 6 and 7, so it is no longer known non-negative */
        check(contains(s, "((max(v0.rgb, 0.0) * max(r0.rgb, 0.0)) + ((1.0 - max(v0.rgb, 0.0)) * max(r1.rgb, 0.0)))"),
              "game_020f30_full: final lerp(r1, r0, v0)");
        check(contains(s, "clamp(float3((s0c_sum * 4.0)), -1.0, 1.0)") && contains(s, "clamp(float((s0a_sum * 0.5)), -1.0, 1.0)"),
              "game_020f30_full: output scales (x4 rgb, x1/2 alpha)");
        GpuJob j{"game_020f30_full", d, {}, random_constants()};
        j.def.texture_modes() = kPassAll;
        for (int n = 0; n < 32; ++n) j.inputs.push_back(random_inputs());
        jobs.push_back(j);

        Def e;
        e.texture_modes() = 0x8421;
        e.alpha_in(0) = 0x18201920; e.alpha_out(0) = 0x30c00;
        e.alpha_in(1) = 0x1a201b20; e.alpha_out(1) = 0x30d00;
        e.alpha_in(2) = 0x1c201d20; e.alpha_out(2) = 0x30c00;
        e.final_efg() = 0x1c00;
        e.rgb_in(0) = 0x8200920; e.rgb_out(0) = 0x30c00;
        e.rgb_in(1) = 0xa200b20; e.rgb_out(1) = 0x30d00;
        e.rgb_in(2) = 0xc200d20; e.rgb_out(2) = 0x30c00;
        e.final_abcd() = 0xc;
        e.combiner_count() = 3 | 0x11000;
        translate_and_compile("game_020f30_short", e);
        GpuJob j2{"game_020f30_short", e, {}, random_constants()};
        j2.def.texture_modes() = kPassAll;
        for (int n = 0; n < 32; ++n) j2.inputs.push_back(random_inputs());
        jobs.push_back(j2);

        /* function_39780, both branches */
        for (int alt = 0; alt < 2; ++alt) {
            Def f;
            f.combiner_count() = 0x11101;
            f.texture_modes() = (!alt) << 15;
            f.alpha_out(0) = 0xc0;
            f.rgb_out(0) = 0xc0;
            f.rgb_in(0) = alt ? 0xc4200000 : 0xc4cb0000;
            f.alpha_in(0) = alt ? 0xd4301010 : 0xd4db1010;
            f.final_abcd() = 0x0c200000;
            f.final_efg() = 0x1c80;
            f.w[57] = f.w[58] = 0xffffffff; /* PSC0Mapping, PSC1Mapping */
            f.w[59] = 0x1ff;                /* PSFinalCombinerConstants */
            auto s3 = translate_and_compile(alt ? "game_39780_alt" : "game_39780", f);
            if (!alt)
                check(contains(s3, "float3((v0.rgb * t3.rgb))"), "game_39780: r0 = v0 * t3 (signed identity)");
        }
        /* luminance: dot(c0, t0) into r0 (PSCombinerCount 0x1001) */
        Def g;
        g.texture_modes() = 1;
        g.combiner_count() = 0x1001;
        g.c0(0) = 0xff4c961c;
        g.rgb_in(0) = 0x1080000;
        g.rgb_out(0) = 0x20c0;
        g.final_abcd() = 0xc;
        g.final_efg() = 0x2000;
        auto s4 = translate_and_compile("game_luminance", g);
        check(contains(s4, "float3(dot(float3(k.c0[0].rgb), float3(t0.rgb)))"), "game_luminance: dot(c0, t0)");
        /* filter copy and constant alpha */
        Def h;
        h.texture_modes() = 1;
        h.combiner_count() = 0x11001;
        h.final_abcd() = 8;
        h.final_efg() = 0x1800;
        auto s5 = translate_and_compile("game_filter_copy", h);
        check(contains(s5, "float3 out_rgb = float3(t0.rgb);") && contains(s5, "float out_a = float(t0.a);"),
              "game_filter_copy: output t0");
        Def m;
        m.texture_modes() = 1;
        m.combiner_count() = 0x11001;
        m.final_abcd() = 0x8110000;
        m.final_efg() = 0x1100;
        auto s6 = translate_and_compile("game_constant_alpha", m);
        check(contains(s6, "(t0.rgb * k.final_c0.aaa)") && contains(s6, "out_a = float(k.final_c0.a)"),
              "game_constant_alpha: t0 * final c0 alpha");
    }
    /* 11. seeded random combiner programs (compile + GPU vs CPU reference) */
    {
        for (int n = 0; n < 40; ++n) {
            Def d = random_combiner_def();
            const std::string name = "random_" + std::to_string(n);
            translate_and_compile(name, d);
            GpuJob j{name, d, {}, random_constants()};
            for (int i = 0; i < 64; ++i) j.inputs.push_back(random_inputs());
            jobs.push_back(j);
        }
        /* random texture-mode words, compile only */
        for (int n = 0; n < 12; ++n) {
            Def d = random_combiner_def();
            d.texture_modes() = rndu(0x13) | (rndu(0x13) << 5) | (rndu(0x13) << 10) | (rndu(0x13) << 15);
            d.dot_mapping() = g_rng() & 0xffff;
            d.input_texture() = g_rng() & 0x330000;
            d.compare_mode() = g_rng() & 0xffff;
            PixelShaderContext ctx;
            for (int i = 0; i < 4; ++i) {
                static const std::uint8_t dims[] = {0, 2, 3, 6};
                ctx.texture_dimension[i] = dims[rndu(4)];
                ctx.texture_signed[i] = rndu(2);
            }
            translate_and_compile("random_modes_" + std::to_string(n), d, ctx);
        }
    }

    run_gpu_jobs(jobs);

    std::printf("%d definitions compiled with xcrun metal; %d checks, %d failures\n", g_compiled, g_checks,
                g_failures);
    return g_failures ? 1 : 0;
}
