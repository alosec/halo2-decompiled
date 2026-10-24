/* Standalone tests for port/src/gpu/texture_formats.cpp.
 *
 *   clang++ -std=c++20 -I port/src/gpu -I port/include \
 *     port/src/gpu/texture_formats.cpp port/src/texture_decode.cpp \
 *     port/tests/gpu/test_texture_formats.cpp -o /tmp/test && /tmp/test [maps dir]
 *
 * With a maps directory (default orig/game/maps, skipped if missing) the
 * layout is also checked against every bitmap data block in the retail
 * shared.map: their level sizes must equal the described chain sizes. */
#include "texture_formats.hpp"

#include "h2/texture_decode.hpp"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iterator>
#include <numeric>
#include <string>
#include <vector>

using namespace h2::gpu;

namespace {

int failures = 0;
int checks = 0;

#define CHECK(condition)                                                              \
    do {                                                                              \
        ++checks;                                                                     \
        if (!(condition)) {                                                           \
            ++failures;                                                               \
            std::fprintf(stderr, "%s:%d: CHECK failed: %s\n", __FILE__, __LINE__, #condition); \
        }                                                                             \
    } while (0)

/* The game's own 2D interleave, function_358d0 (src/unknown_0359e0.cpp),
   restated: x bits at even positions, y at odd, the larger axis' surplus
   bits above. Returns the texel index. */
std::uint32_t game_swizzle_2d(std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height)
{
    static std::uint16_t const spread[64] = {
        0x000, 0x001, 0x004, 0x005, 0x010, 0x011, 0x014, 0x015, 0x040, 0x041, 0x044, 0x045, 0x050,
        0x051, 0x054, 0x055, 0x100, 0x101, 0x104, 0x105, 0x110, 0x111, 0x114, 0x115, 0x140, 0x141,
        0x144, 0x145, 0x150, 0x151, 0x154, 0x155, 0x400, 0x401, 0x404, 0x405, 0x410, 0x411, 0x414,
        0x415, 0x440, 0x441, 0x444, 0x445, 0x450, 0x451, 0x454, 0x455, 0x500, 0x501, 0x504, 0x505,
        0x510, 0x511, 0x514, 0x515, 0x540, 0x541, 0x544, 0x545, 0x550, 0x551, 0x554, 0x555,
    };
    auto log2 = [](std::uint32_t v) {
        std::uint32_t r = 0;
        while (v > 1) {
            v >>= 1;
            ++r;
        }
        return r;
    };
    std::uint32_t const wb = log2(width), hb = log2(height);
    std::uint32_t const common = wb < hb ? wb : hb;
    std::uint32_t const mask = (1u << common) - 1;
    std::uint32_t xb, yb;
    if (mask <= 63) {
        xb = spread[x & mask];
        yb = spread[y & mask];
    } else {
        xb = (spread[(x >> 6) & (mask >> 6)] << 12) | spread[x & 63];
        yb = (spread[(y >> 6) & (mask >> 6)] << 12) | spread[y & 63];
    }
    yb <<= 1;
    if (wb > common)
        xb |= (x >> common) << (common * 2);
    else if (hb > common)
        yb |= (y >> common) << (common * 2);
    return xb | yb;
}

/* Format word as the game builds it (function_12d6d0, src/unknown_12c0d0.cpp);
   levels is the level count. */
std::uint32_t format_word(std::uint32_t code, std::uint32_t wbits, std::uint32_t hbits, std::uint32_t dbits,
                          std::uint32_t levels, bool cube = false, bool volume = false)
{
    return 9 | (cube ? 4 : 0) | ((volume ? 3u : 2u) << 4) | (code << 8) | (levels << 16) | (wbits << 20) |
           (hbits << 24) | (dbits << 28);
}

/* linear: Format (code << 8) | 0x10029, Size from width, height, pitch */
std::uint32_t linear_format(std::uint32_t code) { return (code << 8) | 0x10029; }
std::uint32_t linear_size(std::uint32_t w, std::uint32_t h, std::uint32_t pitch)
{
    return ((pitch / 64 - 1) << 24) | ((h - 1) << 12) | (w - 1);
}

void test_swizzle_index()
{
    CHECK(swizzle_index(1, 0, 0, 4, 4, 1) == 1);
    CHECK(swizzle_index(0, 1, 0, 4, 4, 1) == 2);
    CHECK(swizzle_index(1, 1, 0, 4, 4, 1) == 3);
    CHECK(swizzle_index(2, 0, 0, 4, 4, 1) == 4);
    CHECK(swizzle_index(3, 3, 0, 4, 4, 1) == 15);
    /* 8x2: one shared bit, then x alone */
    CHECK(swizzle_index(0, 1, 0, 8, 2, 1) == 2);
    CHECK(swizzle_index(2, 0, 0, 8, 2, 1) == 4);
    CHECK(swizzle_index(4, 1, 0, 8, 2, 1) == 10);
    /* 1xN and Nx1 are linear */
    for (std::uint32_t i = 0; i < 16; ++i) {
        CHECK(swizzle_index(0, i, 0, 1, 16, 1) == i);
        CHECK(swizzle_index(i, 0, 0, 16, 1, 1) == i);
    }
    /* volumes: x, y, z bits in turn */
    CHECK(swizzle_index(1, 0, 0, 2, 2, 2) == 1);
    CHECK(swizzle_index(0, 1, 0, 2, 2, 2) == 2);
    CHECK(swizzle_index(0, 0, 1, 2, 2, 2) == 4);
    CHECK(swizzle_index(0, 0, 1, 4, 4, 2) == 4);
    CHECK(swizzle_index(2, 0, 0, 4, 4, 2) == 8);
    CHECK(swizzle_index(0, 2, 0, 4, 4, 2) == 16);
    CHECK(swizzle_index(0, 0, 2, 2, 2, 4) == 8);
    /* against the game's own 2D interleave, every size up to 512 */
    for (std::uint32_t wb = 0; wb <= 9; ++wb)
        for (std::uint32_t hb = 0; hb <= 9; ++hb) {
            std::uint32_t const w = 1u << wb, h = 1u << hb;
            bool same = true;
            for (std::uint32_t y = 0; y < h && same; y += (h > 64 ? 7 : 1))
                for (std::uint32_t x = 0; x < w && same; x += (w > 64 ? 5 : 1))
                    same = swizzle_index(x, y, 0, w, h, 1) == game_swizzle_2d(x, y, w, h);
            CHECK(same);
        }
}

void test_round_trips()
{
    struct Size {
        std::uint32_t w, h, d;
    };
    Size const sizes[] = {{1, 1, 1},  {1, 8, 1},  {8, 1, 1},  {2, 2, 1},   {4, 4, 1},  {16, 4, 1},
                          {4, 16, 1}, {64, 64, 1}, {256, 32, 1}, {1, 256, 1}, {2, 2, 2},  {32, 8, 4},
                          {1, 1, 8},  {4, 1, 16}, {16, 16, 16}, {8, 32, 2}};
    for (Size const s : sizes)
        for (std::uint32_t bpp : {1u, 2u, 4u, 8u}) {
            std::size_t const n = static_cast<std::size_t>(s.w) * s.h * s.d;
            std::vector<std::uint8_t> linear(n * bpp), swz(n * bpp), back(n * bpp);
            for (std::size_t i = 0; i < linear.size(); ++i)
                linear[i] = static_cast<std::uint8_t>(i * 7 + i / 251);
            swizzle(linear.data(), swz.data(), s.w, s.h, s.d, bpp);
            unswizzle(swz.data(), back.data(), s.w, s.h, s.d, bpp);
            CHECK(back == linear);
            /* the index is a permutation of 0..n-1 */
            std::vector<bool> seen(n);
            bool perm = true;
            for (std::uint32_t z = 0; z < s.d; ++z)
                for (std::uint32_t y = 0; y < s.h; ++y)
                    for (std::uint32_t x = 0; x < s.w; ++x) {
                        std::uint32_t const i = swizzle_index(x, y, z, s.w, s.h, s.d);
                        if (i >= n || seen[i])
                            perm = false;
                        else
                            seen[i] = true;
                    }
            CHECK(perm);
        }
    /* agrees with the old curated decoder */
    std::vector<std::uint8_t> swz(16 * 4 * 4);
    for (std::size_t i = 0; i < swz.size(); ++i)
        swz[i] = static_cast<std::uint8_t>(i * 13);
    std::vector<std::uint8_t> old, mine(swz.size());
    CHECK(h2::port::decode_argb8_swizzled(swz, 16, 4, old));
    unswizzle(swz.data(), mine.data(), 16, 4, 1, 4);
    bool same = true;
    for (std::size_t i = 0; i < 64; ++i)
        same = same && old[i * 4] == mine[i * 4 + 2] && old[i * 4 + 1] == mine[i * 4 + 1] &&
               old[i * 4 + 2] == mine[i * 4] && old[i * 4 + 3] == mine[i * 4 + 3];
    CHECK(same);
}

void test_layouts()
{
    TextureDescription d;
    /* 2D 256x64 A8R8G8B8, 9 levels */
    CHECK(describe_texture(format_word(0x06, 8, 6, 0, 9), 0, d));
    CHECK(d.kind == TextureKind::texture_2d && d.swizzled && !d.linear && !d.compressed);
    CHECK(d.width == 256 && d.height == 64 && d.depth == 1 && d.levels == 9 && d.faces == 1);
    CHECK(d.bytes_per_texel == 4);
    CHECK(d.mips[0].size == 256 * 64 * 4 && d.mips[1].offset == 256 * 64 * 4);
    CHECK(d.mips[6].width == 4 && d.mips[6].height == 1);
    CHECK(d.mips[8].width == 1 && d.mips[8].height == 1 && d.mips[8].size == 4);
    std::uint32_t sum = 0;
    for (std::uint32_t i = 0; i < 9; ++i)
        sum += d.mips[i].size;
    CHECK(d.total_size == sum);

    /* DXT1 16x16 with levels down to 1x1: sub-4x4 levels take a block */
    CHECK(describe_texture(format_word(0x0c, 4, 4, 0, 5), 0, d));
    CHECK(d.compressed && d.bytes_per_block == 8);
    CHECK(d.mips[0].size == 128 && d.mips[0].row_pitch == 32 && d.mips[0].rows == 4);
    CHECK(d.mips[2].size == 8 && d.mips[3].size == 8 && d.mips[4].size == 8);
    CHECK(d.total_size == 128 + 32 + 8 + 8 + 8);

    /* retail cube 64x64 X8R8G8B8, 7 levels: shared.map level_sizes[0] is
       131328 = 6 * align128(21844) */
    CHECK(describe_texture(format_word(0x07, 6, 6, 0, 7, true), 0, d));
    CHECK(d.kind == TextureKind::cube && d.faces == 6);
    CHECK(d.face_stride == 21888 && d.total_size == 131328);
    CHECK(d.offset_of(1, 0) == 21888 && d.offset_of(2, 1) == 2 * 21888 + 16384);
    /* retail cube 64x64 DXT1, 5 levels: 16896 = 6 * align128(2728) */
    CHECK(describe_texture(format_word(0x0c, 6, 6, 0, 5, true), 0, d));
    CHECK(d.total_size == 16896);
    /* retail cube 4x4 X8R8G8B8, 3 levels: 768 */
    CHECK(describe_texture(format_word(0x07, 2, 2, 0, 3, true), 0, d));
    CHECK(d.total_size == 768);

    /* retail volume 32^3 A8L8, 6 levels: 74898 bytes (75008 with the
       bitmap's 128-byte rounding) */
    CHECK(describe_texture(format_word(0x1a, 5, 5, 5, 6, false, true), 0, d));
    CHECK(d.kind == TextureKind::volume && d.depth == 32);
    CHECK(d.total_size == 74898);
    CHECK(d.mips[1].depth == 16 && d.mips[1].size == 16 * 16 * 16 * 2 && d.mips[1].slice_pitch == 16 * 16 * 2);
    /* retail volume 4x4x4 X8R8G8B8, 3 levels: 292 (384 rounded) */
    CHECK(describe_texture(format_word(0x07, 2, 2, 2, 3, false, true), 0, d));
    CHECK(d.total_size == 292);

    /* linear 100x50 A8R8G8B8, pitch 448 */
    CHECK(describe_texture(linear_format(0x12), linear_size(100, 50, 448), d));
    CHECK(d.linear && d.width == 100 && d.height == 50 && d.pitch == 448 && d.levels == 1);
    CHECK(d.mips[0].size == 448 * 50 && d.mips[0].row_pitch == 448);
    /* a zero Size is 1x1 with a 64-byte pitch; pitch too small, unknown code */
    CHECK(describe_texture(linear_format(0x12), 0, d) && d.width == 1 && d.height == 1 && d.pitch == 64);
    CHECK(!describe_texture(linear_format(0x12), linear_size(100, 50, 384), d));
    CHECK(!describe_texture(format_word(0xee, 2, 2, 0, 1), 0, d));
    /* non-square cube is rejected */
    CHECK(!describe_texture(format_word(0x06, 3, 2, 0, 1, true), 0, d));
}

void test_game_codes()
{
    for (int linear = 0; linear < 2; ++linear)
        for (int f = 0; f < 24; ++f) {
            std::int32_t const code = game_bitmap_hardware_code(f, linear != 0);
            if (code < 0)
                continue;
            HardwareFormat const *hf = hardware_format(static_cast<std::uint32_t>(code));
            CHECK(hf && hf->produced_by_game && hf->verified);
            CHECK(hf && (hf->layout == TexelLayout::linear) == (linear != 0) ||
                  (hf && hf->layout == TexelLayout::compressed && !linear));
            CHECK(hf && hf->conversion != Conversion::unsupported);
        }
    CHECK(game_bitmap_hardware_code(11, false, true) == 0x33);
    CHECK(game_bitmap_hardware_code(10, true, true) == 0x36);
    CHECK(hardware_format(0x33) && hardware_format(0x36) && hardware_format(0x2e) && hardware_format(0x12));
    /* every code is listed once */
    for (HardwareFormat const &a : hardware_formats())
        for (HardwareFormat const &b : hardware_formats())
            CHECK(&a == &b || a.code != b.code);
}

/* one-texel 1x1 swizzled texture of a code */
UploadImage upload_one(std::uint32_t code, std::vector<std::uint8_t> texel,
                       std::span<std::uint32_t const> palette = {}, UploadOptions options = {})
{
    TextureDescription d;
    UploadImage image;
    bool const linear = hardware_format(code)->layout == TexelLayout::linear;
    bool ok = linear ? describe_texture(linear_format(code), linear_size(1, 1, 64), d)
                     : describe_texture(format_word(code, 0, 0, 0, 1), 0, d);
    if (linear)
        texel.resize(64);
    ok = ok && prepare_upload(d, texel, 0, 0, image, palette, options);
    CHECK(ok);
    return image;
}

std::vector<std::uint8_t> rgba_one(std::uint32_t code, std::vector<std::uint8_t> texel,
                                   std::span<std::uint32_t const> palette = {})
{
    TextureDescription d;
    std::vector<std::uint8_t> rgba;
    bool ok = describe_texture(format_word(code, 0, 0, 0, 1), 0, d) && decode_rgba8(d, texel, 0, 0, rgba, palette);
    CHECK(ok);
    return rgba;
}

using Bytes = std::vector<std::uint8_t>;

void test_colour_conversions()
{
    /* A1R5G5B5 0x801f: A=1, R=0, G=0, B=31 */
    CHECK(upload_one(0x02, {0x1f, 0x80}).bytes == (Bytes{0, 0, 255, 255}));
    CHECK(upload_one(0x02, {0x00, 0x7c}).bytes == (Bytes{255, 0, 0, 0}));
    CHECK(upload_one(0x10, {0xe0, 0x03}).bytes == (Bytes{0, 255, 0, 0}));
    /* X1R5G5B5 ignores the top bit */
    CHECK(upload_one(0x03, {0x10, 0x42}).bytes == (Bytes{132, 132, 132, 255}));
    /* A4R4G4B4 0x8f0c: A=8, R=15, G=0, B=12 */
    CHECK(upload_one(0x04, {0x0c, 0x8f}).bytes == (Bytes{255, 0, 204, 136}));
    CHECK(upload_one(0x1d, {0x0c, 0x8f}).bytes == (Bytes{255, 0, 204, 136}));
    /* R5G6B5 0xf800 red, 0x07e0 green, 0x001f blue, 0x8410 mid grey */
    CHECK(upload_one(0x05, {0x00, 0xf8}).bytes == (Bytes{255, 0, 0, 255}));
    CHECK(upload_one(0x05, {0xe0, 0x07}).bytes == (Bytes{0, 255, 0, 255}));
    CHECK(upload_one(0x11, {0x1f, 0x00}).bytes == (Bytes{0, 0, 255, 255}));
    CHECK(upload_one(0x05, {0x10, 0x84}).bytes == (Bytes{132, 130, 132, 255}));

    /* 32-bit formats go up as they are */
    UploadImage argb = upload_one(0x06, {0x11, 0x22, 0x33, 0x44});
    CHECK(argb.format == MtlPixelFormat::bgra8_unorm && argb.bytes == (Bytes{0x11, 0x22, 0x33, 0x44}));
    CHECK(argb.swizzle.alpha == MtlSwizzle::alpha && argb.bytes_per_row == 4);
    UploadImage xrgb = upload_one(0x07, {0x11, 0x22, 0x33, 0x00});
    CHECK(xrgb.format == MtlPixelFormat::bgra8_unorm && xrgb.swizzle.alpha == MtlSwizzle::one);
    CHECK(upload_one(0x1e, {1, 2, 3, 4}).format == MtlPixelFormat::bgra8_unorm);
    CHECK(upload_one(0x3a, {1, 2, 3, 4}).format == MtlPixelFormat::rgba8_unorm);
    CHECK(rgba_one(0x06, {0x11, 0x22, 0x33, 0x44}) == (Bytes{0x33, 0x22, 0x11, 0x44}));
    CHECK(rgba_one(0x07, {0x11, 0x22, 0x33, 0x00}) == (Bytes{0x33, 0x22, 0x11, 0xff}));

    /* one/two channel formats keep their bytes and carry a swizzle */
    UploadImage l8 = upload_one(0x00, {0x80});
    CHECK(l8.format == MtlPixelFormat::r8_unorm && l8.bytes == (Bytes{0x80}));
    CHECK(l8.swizzle.green == MtlSwizzle::red && l8.swizzle.alpha == MtlSwizzle::one);
    CHECK(rgba_one(0x00, {0x80}) == (Bytes{0x80, 0x80, 0x80, 0xff}));
    CHECK(rgba_one(0x01, {0x80}) == (Bytes{0x80, 0x80, 0x80, 0x80}));
    CHECK(upload_one(0x01, {7}).swizzle.alpha == MtlSwizzle::red);
    CHECK(upload_one(0x19, {0x55}).format == MtlPixelFormat::a8_unorm);
    CHECK(rgba_one(0x19, {0x55}) == (Bytes{0, 0, 0, 0x55}));
    UploadImage al = upload_one(0x1a, {0x40, 0xc0});
    CHECK(al.format == MtlPixelFormat::rg8_unorm && al.swizzle.alpha == MtlSwizzle::green);
    CHECK(rgba_one(0x1a, {0x40, 0xc0}) == (Bytes{0x40, 0x40, 0x40, 0xc0}));
    CHECK(upload_one(0x1f, {9}).format == MtlPixelFormat::a8_unorm);
    CHECK(upload_one(0x13, {9}).bytes == (Bytes{9}));
    CHECK(upload_one(0x20, {9, 10}).bytes == (Bytes{9, 10}));

    /* depth: D24S8 depth in the top 24 bits */
    UploadImage z = upload_one(0x2e, {0x5a, 0xff, 0xff, 0xff});
    float depth = 0;
    std::memcpy(&depth, z.bytes.data(), 4);
    CHECK(z.format == MtlPixelFormat::r32_float && depth == 1.0f);
    z = upload_one(0x2a, {0x00, 0x00, 0x00, 0x80});
    std::memcpy(&depth, z.bytes.data(), 4);
    CHECK(depth > 0.4999f && depth < 0.5001f);
    CHECK(upload_one(0x2c, {0x34, 0x12}).format == MtlPixelFormat::r16_unorm);
}

void test_palette()
{
    std::vector<std::uint32_t> palette(256);
    for (std::uint32_t i = 0; i < 256; ++i)
        palette[i] = (i << 24) | ((255 - i) << 16) | (i << 8) | 0x5a;
    palette[3] = 0x80ff4020;
    UploadImage image = upload_one(0x0b, {3}, palette);
    CHECK(image.format == MtlPixelFormat::rgba8_unorm && image.bytes == (Bytes{0xff, 0x40, 0x20, 0x80}));

    /* a 4x4 swizzled P8 texture: every texel through the palette in place */
    TextureDescription d;
    CHECK(describe_texture(format_word(0x0b, 2, 2, 0, 1), 0, d));
    Bytes linear(16), swz(16);
    std::iota(linear.begin(), linear.end(), 10);
    swizzle(linear.data(), swz.data(), 4, 4, 1, 1);
    CHECK(prepare_upload(d, swz, 0, 0, image, palette));
    bool ok = image.bytes.size() == 64 && image.bytes_per_row == 16;
    for (std::uint32_t i = 0; ok && i < 16; ++i) {
        std::uint32_t const c = palette[linear[i]];
        ok = image.bytes[i * 4] == ((c >> 16) & 0xff) && image.bytes[i * 4 + 1] == ((c >> 8) & 0xff) &&
             image.bytes[i * 4 + 2] == (c & 0xff) && image.bytes[i * 4 + 3] == (c >> 24);
    }
    CHECK(ok);
    /* no palette, no upload */
    CHECK(!prepare_upload(d, swz, 0, 0, image));
}

void test_signed()
{
    /* V8U8 (DSDT8): U low byte -> R, V high byte -> G, RG8Snorm as is */
    UploadImage v = upload_one(0x28, {0x81, 0x7f});
    CHECK(v.format == MtlPixelFormat::rg8_snorm && v.bytes == (Bytes{0x81, 0x7f}));
    CHECK(hardware_format(0x28)->is_signed && hardware_format(0x17)->is_signed);
    CHECK(rgba_one(0x28, {0x81, 0x7f}) == (Bytes{1, 255, 255, 255}));
    CHECK(rgba_one(0x28, {0x00, 0xff}) == (Bytes{128, 127, 255, 255}));
    /* V16U16 (HILO16): RG16Snorm as is */
    UploadImage hl = upload_one(0x33, {0x00, 0x80, 0xff, 0x7f});
    CHECK(hl.format == MtlPixelFormat::rg16_snorm && hl.bytes == (Bytes{0x00, 0x80, 0xff, 0x7f}));
    CHECK(upload_one(0x36, {1, 2, 3, 4}).format == MtlPixelFormat::rg16_snorm);
    /* signed 32-bit colour on request: RGBA8Snorm, channels kept in place */
    UploadOptions options;
    options.signed_rgba8 = true;
    UploadImage q = upload_one(0x06, {0x01, 0x82, 0x7f, 0xff}, {}, options);
    CHECK(q.format == MtlPixelFormat::rgba8_snorm && q.bytes == (Bytes{0x7f, 0x82, 0x01, 0xff}));
    q = upload_one(0x3a, {0x01, 0x82, 0x7f, 0xff}, {}, options);
    CHECK(q.format == MtlPixelFormat::rgba8_snorm && q.bytes == (Bytes{0x01, 0x82, 0x7f, 0xff}));
}

void test_dxt()
{
    /* 8x8 DXT1 with 4 levels: 4 + 1 + 1 + 1 blocks */
    TextureDescription d;
    CHECK(describe_texture(format_word(0x0c, 3, 3, 0, 4), 0, d));
    CHECK(d.total_size == 7 * 8);
    Bytes data(d.total_size);
    for (std::size_t i = 0; i < data.size(); ++i)
        data[i] = static_cast<std::uint8_t>(i * 31 + 5);
    UploadImage image;
    CHECK(prepare_upload(d, data, 0, 0, image));
    CHECK(image.format == MtlPixelFormat::bc1_rgba && image.bytes_per_row == 16 && image.bytes_per_image == 32);
    CHECK(image.bytes == Bytes(data.begin(), data.begin() + 32));
    CHECK(prepare_upload(d, data, 0, 2, image));
    CHECK(image.width == 2 && image.height == 2 && image.bytes_per_row == 8);
    CHECK(image.bytes == Bytes(data.begin() + 40, data.begin() + 48));

    /* DXT1 decode: colour0 red > colour1 blue, indices 0,1,2,3 in row 0 */
    Bytes block = {0x00, 0xf8, 0x1f, 0x00, 0xe4, 0x00, 0x00, 0x00};
    CHECK(describe_texture(format_word(0x0c, 2, 2, 0, 1), 0, d));
    std::vector<std::uint8_t> rgba;
    CHECK(decode_rgba8(d, block, 0, 0, rgba));
    CHECK(rgba[0] == 255 && rgba[2] == 0 && rgba[3] == 255);           /* index 0 red */
    CHECK(rgba[4] == 0 && rgba[6] == 255);                              /* index 1 blue */
    CHECK(rgba[8] == 170 && rgba[10] == 85 && rgba[12] == 85 && rgba[14] == 170);
    /* DXT1 3-colour mode: colour0 <= colour1 makes index 3 transparent */
    Bytes block3 = {0x1f, 0x00, 0x00, 0xf8, 0xc0, 0x00, 0x00, 0x00};
    CHECK(decode_rgba8(d, block3, 0, 0, rgba));
    CHECK(rgba[12] == 0 && rgba[15] == 0 && rgba[3] == 255);
    /* DXT3 explicit alpha: nibble 0xf for texel 0, 0x8 for texel 1 */
    CHECK(describe_texture(format_word(0x0e, 2, 2, 0, 1), 0, d));
    Bytes block16 = {0x8f, 0, 0, 0, 0, 0, 0, 0, 0x00, 0xf8, 0x00, 0xf8, 0, 0, 0, 0};
    CHECK(decode_rgba8(d, block16, 0, 0, rgba));
    CHECK(rgba[3] == 255 && rgba[7] == 136 && rgba[11] == 0 && rgba[0] == 255);
    UploadImage bc2;
    CHECK(prepare_upload(d, block16, 0, 0, bc2) && bc2.format == MtlPixelFormat::bc2_rgba && bc2.bytes == block16);
    CHECK(describe_texture(format_word(0x0f, 2, 2, 0, 1), 0, d));
    CHECK(prepare_upload(d, block16, 0, 0, bc2) && bc2.format == MtlPixelFormat::bc3_rgba);
}

void test_faces_and_volumes()
{
    /* cube 4x4 A8R8G8B8, 3 levels: each face texel holds face/level/x/y */
    TextureDescription d;
    CHECK(describe_texture(format_word(0x06, 2, 2, 0, 3, true), 0, d));
    Bytes data(d.total_size, 0xcd);
    for (std::uint32_t face = 0; face < 6; ++face)
        for (std::uint32_t level = 0; level < 3; ++level) {
            MipLevel const &m = d.mips[level];
            for (std::uint32_t y = 0; y < m.height; ++y)
                for (std::uint32_t x = 0; x < m.width; ++x) {
                    std::uint8_t *t = &data[d.offset_of(face, level) + swizzle_index(x, y, 0, m.width, m.height, 1) * 4];
                    t[0] = static_cast<std::uint8_t>(face);
                    t[1] = static_cast<std::uint8_t>(level);
                    t[2] = static_cast<std::uint8_t>(x);
                    t[3] = static_cast<std::uint8_t>(y);
                }
        }
    bool ok = true;
    for (std::uint32_t face = 0; face < 6; ++face)
        for (std::uint32_t level = 0; level < 3; ++level) {
            UploadImage image;
            ok = ok && prepare_upload(d, data, face, level, image);
            MipLevel const &m = d.mips[level];
            for (std::uint32_t y = 0; ok && y < m.height; ++y)
                for (std::uint32_t x = 0; ok && x < m.width; ++x) {
                    std::uint8_t const *t = &image.bytes[(y * m.width + x) * 4];
                    ok = t[0] == face && t[1] == level && t[2] == x && t[3] == y;
                }
        }
    CHECK(ok);
    UploadImage image;
    CHECK(!prepare_upload(d, data, 6, 0, image));
    CHECK(!prepare_upload(d, data, 0, 3, image));
    CHECK(prepare_upload(d, std::span<std::uint8_t const>(data.data(), d.offset_of(5, 2) + 4), 5, 2, image));
    CHECK(!prepare_upload(d, std::span<std::uint8_t const>(data.data(), d.offset_of(5, 2) + 3), 5, 2, image));

    /* volume 8x4x2 A8L8, 2 levels: slices come out in order */
    CHECK(describe_texture(format_word(0x1a, 3, 2, 1, 2, false, true), 0, d));
    Bytes vol(d.total_size);
    for (std::uint32_t level = 0; level < 2; ++level) {
        MipLevel const &m = d.mips[level];
        for (std::uint32_t z = 0; z < m.depth; ++z)
            for (std::uint32_t y = 0; y < m.height; ++y)
                for (std::uint32_t x = 0; x < m.width; ++x) {
                    std::uint8_t *t = &vol[m.offset + swizzle_index(x, y, z, m.width, m.height, m.depth) * 2];
                    t[0] = static_cast<std::uint8_t>(x | (y << 4));
                    t[1] = static_cast<std::uint8_t>(z | (level << 4));
                }
    }
    ok = prepare_upload(d, vol, 0, 1, image);
    CHECK(ok && image.width == 4 && image.height == 2 && image.depth == 1 && image.bytes_per_image == 16);
    ok = prepare_upload(d, vol, 0, 0, image);
    CHECK(ok && image.depth == 2 && image.bytes_per_row == 16 && image.bytes_per_image == 64);
    for (std::uint32_t z = 0; ok && z < 2; ++z)
        for (std::uint32_t y = 0; ok && y < 4; ++y)
            for (std::uint32_t x = 0; ok && x < 8; ++x) {
                std::uint8_t const *t = &image.bytes[z * 64 + y * 16 + x * 2];
                ok = t[0] == (x | (y << 4)) && t[1] == z;
            }
    CHECK(ok);

    /* linear pitch is stripped */
    CHECK(describe_texture(linear_format(0x12), linear_size(3, 2, 64), d));
    Bytes lin(128, 0xee);
    for (int i = 0; i < 12; ++i) {
        lin[i] = static_cast<std::uint8_t>(i);
        lin[64 + i] = static_cast<std::uint8_t>(100 + i);
    }
    CHECK(prepare_upload(d, lin, 0, 0, image));
    CHECK(image.bytes.size() == 24 && image.bytes_per_row == 12 && image.bytes[11] == 11 && image.bytes[12] == 100);
}

/* ---- retail data ------------------------------------------------------ */

std::int16_t s16(std::uint8_t const *p) { return static_cast<std::int16_t>(p[0] | (p[1] << 8)); }
std::int32_t s32(std::uint8_t const *p)
{
    return static_cast<std::int32_t>(p[0] | (p[1] << 8) | (p[2] << 16) | (static_cast<std::uint32_t>(p[3]) << 24));
}

std::uint32_t log2_floor(std::uint32_t v)
{
    std::uint32_t r = 0;
    while (v > 1) {
        v >>= 1;
        ++r;
    }
    return r;
}

/* Every s_bitmap_data ('bitm', 0x74 bytes; layout in
   port/tests/guest_texture_pool_fixture.cpp) in the map: build the Format
   word the game would (function_12d6d0) and compare the described chain
   with the block's level_sizes[0] (the bitmap's pixel data, rounded to 128
   bytes). */
void test_retail(std::string const &maps)
{
    std::ifstream file(maps + "/shared.map", std::ios::binary);
    if (!file) {
        std::printf("retail: %s/shared.map not found, skipped\n", maps.c_str());
        return;
    }
    std::vector<std::uint8_t> map((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    int matched = 0, compared = 0;
    for (std::size_t i = 0; i + 0x74 <= map.size(); ++i) {
        std::uint8_t const *b = &map[i];
        if (b[0] != 'm' || b[1] != 't' || b[2] != 'i' || b[3] != 'b')
            continue;
        int const width = s16(b + 4), height = s16(b + 6), depth = static_cast<std::int8_t>(b[8]);
        int const type = s16(b + 10), format = s16(b + 12), flags = static_cast<std::uint16_t>(s16(b + 14));
        int const mipmaps = s16(b + 0x14);
        std::int32_t const level_size = s32(b + 0x34);
        if (width <= 0 || width > 4096 || height <= 0 || height > 4096 || depth <= 0 || type < 0 || type > 2 ||
            format < 0 || format >= 24 || mipmaps < 0 || mipmaps > 13 || level_size <= 0)
            continue;
        bool const linear = (flags & 0x10) != 0;
        std::int32_t const code = game_bitmap_hardware_code(format, linear, (flags & 0x20) != 0);
        if (code < 0)
            continue;
        std::uint32_t format_word_value, size_word_value = 0;
        if (linear) {
            std::uint32_t const pitch = (static_cast<std::uint32_t>(width) * hardware_format(code)->bits_per_texel / 8 + 63) & ~63u;
            format_word_value = linear_format(static_cast<std::uint32_t>(code));
            size_word_value = linear_size(width, height, pitch);
        } else {
            if ((width & (width - 1)) || (height & (height - 1)) || (depth & (depth - 1)))
                continue;
            /* bitmap_get_mipmap_count (src/unknown_0359e0.cpp) */
            bool const dxt = format >= 14 && format <= 16;
            int m = dxt ? log2_floor(std::max({width / 4, height / 4, depth})) : log2_floor(std::max({width, height, depth}));
            m = std::min(m, mipmaps);
            format_word_value = format_word(static_cast<std::uint32_t>(code), log2_floor(width), log2_floor(height),
                                            log2_floor(depth), m + 1, type == 2, type == 1);
        }
        TextureDescription d;
        if (!describe_texture(format_word_value, size_word_value, d)) {
            std::printf("retail: undescribable %dx%dx%d type %d format %d flags %x\n", width, height, depth, type,
                        format, flags);
            ++compared;
            continue;
        }
        ++compared;
        std::uint32_t const expected = (d.total_size + 127) & ~127u;
        if (expected == static_cast<std::uint32_t>(level_size))
            ++matched;
        else if (compared - matched <= 10)
            std::printf("retail: mismatch %dx%dx%d type %d format %d flags %x mips %d: ours %u, tag %d\n", width,
                        height, depth, type, format, flags, mipmaps, d.total_size, level_size);
    }
    std::printf("retail: %d of %d bitmap layouts in shared.map match\n", matched, compared);
    CHECK(compared > 0 && matched == compared);
}

/* mean absolute difference between neighbouring texels (x, y and z) */
double roughness(std::vector<std::uint8_t> const &t, std::uint32_t w, std::uint32_t h, std::uint32_t d,
                 std::uint32_t bpp)
{
    double total = 0;
    std::size_t n = 0;
    auto at = [&](std::uint32_t x, std::uint32_t y, std::uint32_t z) {
        return &t[((static_cast<std::size_t>(z) * h + y) * w + x) * bpp];
    };
    for (std::uint32_t z = 0; z < d; ++z)
        for (std::uint32_t y = 0; y < h; ++y)
            for (std::uint32_t x = 0; x < w; ++x)
                for (int axis = 0; axis < 3; ++axis) {
                    std::uint32_t const nx = x + (axis == 0), ny = y + (axis == 1), nz = z + (axis == 2);
                    if (nx >= w || ny >= h || nz >= d)
                        continue;
                    for (std::uint32_t c = 0; c < bpp; ++c)
                        total += std::abs(at(x, y, z)[c] - at(nx, ny, nz)[c]);
                    ++n;
                }
    return n ? total / static_cast<double>(n) : 0;
}

/* The volume interleave on real texels: mainmenu.map holds its volume
   bitmaps locally (data_offsets[0] is a file offset there). Unswizzled with
   the x/y/z interleave they must come out much smoother than with each
   slice taken as its own 2D swizzled image. */
void test_retail_volume(std::string const &maps)
{
    std::ifstream file(maps + "/mainmenu.map", std::ios::binary);
    if (!file) {
        std::printf("retail: %s/mainmenu.map not found, skipped\n", maps.c_str());
        return;
    }
    std::vector<std::uint8_t> map((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    int tested = 0;
    for (std::size_t i = 0; i + 0x74 <= map.size(); ++i) {
        std::uint8_t const *b = &map[i];
        if (b[0] != 'm' || b[1] != 't' || b[2] != 'i' || b[3] != 'b')
            continue;
        int const w = s16(b + 4), h = s16(b + 6), dd = static_cast<std::int8_t>(b[8]);
        int const type = s16(b + 10), format = s16(b + 12), flags = static_cast<std::uint16_t>(s16(b + 14));
        std::int32_t const offset = s32(b + 0x1c), size = s32(b + 0x34);
        if (type != 1 || w < 16 || w > 256 || w != h || h != dd || (flags & 0x10) || offset <= 0 ||
            static_cast<std::size_t>(offset) + static_cast<std::size_t>(size) > map.size())
            continue;
        std::int32_t const code = game_bitmap_hardware_code(format, false);
        if (code < 0 || hardware_format(code)->layout != TexelLayout::swizzled)
            continue;
        std::uint32_t const lb = log2_floor(w);
        TextureDescription d;
        if (!describe_texture(format_word(code, lb, lb, lb, 1, false, true), 0, d))
            continue;
        std::span<std::uint8_t const> texels(&map[offset], d.total_size);
        std::vector<std::uint8_t> volume(d.total_size), slices(d.total_size);
        unswizzle(texels.data(), volume.data(), w, h, dd, d.bytes_per_texel);
        std::uint32_t const slice = d.mips[0].slice_pitch;
        for (int z = 0; z < dd; ++z)
            unswizzle(texels.data() + z * slice, slices.data() + z * slice, w, h, 1, d.bytes_per_texel);
        double const r3 = roughness(volume, w, h, dd, d.bytes_per_texel);
        double const r2 = roughness(slices, w, h, dd, d.bytes_per_texel);
        std::printf("retail: volume %dx%dx%d format %d at 0x%x: roughness %.2f interleaved, %.2f per slice\n", w,
                    h, dd, format, offset, r3, r2);
        CHECK(r3 * 2 < r2);
        ++tested;
    }
    CHECK(tested > 0);
}

}  // namespace

int main(int argc, char **argv)
{
    test_swizzle_index();
    test_round_trips();
    test_layouts();
    test_game_codes();
    test_colour_conversions();
    test_palette();
    test_signed();
    test_dxt();
    test_faces_and_volumes();
    test_retail(argc > 1 ? argv[1] : "orig/game/maps");
    test_retail_volume(argc > 1 ? argv[1] : "orig/game/maps");
    std::printf("%d checks, %d failures\n", checks, failures);
    return failures ? 1 : 0;
}
