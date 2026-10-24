/* NV2A texture formats, layouts, unswizzling and conversion for the Metal
 * backend. See texture_formats.hpp for the Format/Size word layout.
 *
 * Sources (all public, permissively licensed or the game's own code):
 *   [rnndb]  envytools rnndb/graph/nv_3ddefs.xml, enum nv10_tex_format,
 *            variants NV20_3D (MIT). Same values as Mesa's classic nouveau
 *            src/mesa/drivers/dri/nouveau/nv20_3d.xml.h (MIT, Mesa 20.3).
 *   [wiki]   xboxdevwiki.net "XPR": NV097_SET_TEXTURE_FORMAT_COLOR_* list.
 *   [game]   the recovered source: g_450768 (src/unknown_0158f0.cpp) maps
 *            bitmap formats to codes, row 0 swizzled, row 1 linear;
 *            function_1d4b0 (src/unknown_01cf50.cpp) lists the codes the
 *            game treats as linear; function_1d6f0 (src/unknown_0359e0.cpp)
 *            builds 0x06/0x12 colour and 0x2e depth surfaces;
 *            texture_cache_bitmap_build_texture / function_12d6d0
 *            (src/unknown_12c0d0.cpp) use 0x33/0x36 for v16u16 bitmaps.
 *            Halo 2 bitmap formats: 0 a8, 1 y8, 2 ay8, 3 a8y8, 6 r5g6b5,
 *            8 a1r5g5b5, 9 a4r4g4b4, 10 x8r8g8b8, 11 a8r8g8b8, 14 dxt1,
 *            15 dxt3, 16 dxt5, 17 p8 bump, 18 p8, 22 v8u8, 23 g8b8 (bits
 *            per pixel in g_453550, src/unknown_136490.cpp).
 *   [maps]   facts measured from the retail cache files (see the tests):
 *            mip chains are packed with no per-level padding, each cube
 *            face (with its whole chain) is padded to 128 bytes.
 *
 * Nothing here comes from SDK headers or GPL emulator sources. */
#include "texture_formats.hpp"

#include "h2/texture_decode.hpp"

#include <algorithm>
#include <cstring>

namespace h2::gpu {
namespace {

using F = MtlPixelFormat;
using S = MtlSwizzle;
using C = Conversion;
using L = TexelLayout;

constexpr SwizzleChannels k_id = k_identity_swizzle;
constexpr SwizzleChannels k_rgb1{S::red, S::green, S::blue, S::one};
constexpr SwizzleChannels k_rrr1{S::red, S::red, S::red, S::one};
constexpr SwizzleChannels k_rrrr{S::red, S::red, S::red, S::red};
constexpr SwizzleChannels k_rrrg{S::red, S::red, S::red, S::green};
constexpr SwizzleChannels k_rg11{S::red, S::green, S::one, S::one};

/* code, name, layout, bits, block bytes, upload, swizzle, conversion,
   signed, depth, produced by game, verified */
constexpr HardwareFormat k_formats[] = {
    /* [rnndb] L8; [game] y8 -> 0x00. Luminance: (L, L, L, 1). */
    {0x00, "SZ_L8", L::swizzled, 8, 0, F::r8_unorm, k_rrr1, C::none, false, false, true, true},
    /* [rnndb] I8; [game] ay8 -> 0x01. Intensity: (I, I, I, I). */
    {0x01, "SZ_I8", L::swizzled, 8, 0, F::r8_unorm, k_rrrr, C::none, false, false, true, true},
    /* [rnndb][wiki] A1R5G5B5; [game] a1r5g5b5 -> 0x02 */
    {0x02, "SZ_A1R5G5B5", L::swizzled, 16, 0, F::rgba8_unorm, k_id, C::a1r5g5b5_to_rgba8, false, false, true, true},
    /* [wiki] X1R5G5B5 */
    {0x03, "SZ_X1R5G5B5", L::swizzled, 16, 0, F::rgba8_unorm, k_id, C::x1r5g5b5_to_rgba8, false, false, false, true},
    /* [rnndb][wiki] A4R4G4B4; [game] a4r4g4b4 -> 0x04 */
    {0x04, "SZ_A4R4G4B4", L::swizzled, 16, 0, F::rgba8_unorm, k_id, C::a4r4g4b4_to_rgba8, false, false, true, true},
    /* [rnndb][wiki] R5G6B5; [game] r5g6b5 -> 0x05 */
    {0x05, "SZ_R5G6B5", L::swizzled, 16, 0, F::rgba8_unorm, k_id, C::r5g6b5_to_rgba8, false, false, true, true},
    /* [rnndb][wiki] A8R8G8B8; [game] a8r8g8b8 -> 0x06. Bytes B, G, R, A. */
    {0x06, "SZ_A8R8G8B8", L::swizzled, 32, 0, F::bgra8_unorm, k_id, C::none, false, false, true, true},
    /* [rnndb][wiki] X8R8G8B8; [game] x8r8g8b8 -> 0x07 */
    {0x07, "SZ_X8R8G8B8", L::swizzled, 32, 0, F::bgra8_unorm, k_rgb1, C::none, false, false, true, true},
    /* [rnndb] INDEX8, [wiki] I8_A8R8G8B8; [game] p8 and p8 bump -> 0x0b */
    {0x0b, "SZ_INDEX8", L::swizzled, 8, 0, F::rgba8_unorm, k_id, C::p8_to_rgba8, false, false, true, true},
    /* [rnndb][wiki] DXT1 (wiki: L_DXT1_A1R5G5B5); [game] dxt1 -> 0x0c */
    {0x0c, "DXT1", L::compressed, 4, 8, F::bc1_rgba, k_id, C::none, false, false, true, true},
    /* [rnndb] DXT3, [wiki] L_DXT23_A8R8G8B8; [game] dxt3 -> 0x0e */
    {0x0e, "DXT23", L::compressed, 8, 16, F::bc2_rgba, k_id, C::none, false, false, true, true},
    /* [rnndb] DXT5, [wiki] L_DXT45_A8R8G8B8; [game] dxt5 -> 0x0f */
    {0x0f, "DXT45", L::compressed, 8, 16, F::bc3_rgba, k_id, C::none, false, false, true, true},
    /* [rnndb] A1R5G5B5_RECT; [game] a1r5g5b5 linear -> 0x10 */
    {0x10, "LU_A1R5G5B5", L::linear, 16, 0, F::rgba8_unorm, k_id, C::a1r5g5b5_to_rgba8, false, false, true, true},
    /* [rnndb][wiki] R5G6B5_RECT; [game] r5g6b5 linear -> 0x11 */
    {0x11, "LU_R5G6B5", L::linear, 16, 0, F::rgba8_unorm, k_id, C::r5g6b5_to_rgba8, false, false, true, true},
    /* [rnndb][wiki] A8R8G8B8_RECT; [game] a8r8g8b8 linear -> 0x12 */
    {0x12, "LU_A8R8G8B8", L::linear, 32, 0, F::bgra8_unorm, k_id, C::none, false, false, true, true},
    /* [rnndb] L8_RECT (NV20); [game] y8 linear -> 0x13 */
    {0x13, "LU_L8", L::linear, 8, 0, F::r8_unorm, k_rrr1, C::none, false, false, true, true},
    /* [rnndb] DSDT8_RECT; [game] v8u8/g8b8 linear -> 0x17. Signed du (low
       byte), dv (high byte). */
    {0x17, "LU_DSDT8", L::linear, 16, 0, F::rg8_snorm, k_rg11, C::none, true, false, true, true},
    /* [wiki] A8; [game] a8 -> 0x19. Metal A8Unorm samples (0, 0, 0, A). */
    {0x19, "SZ_A8", L::swizzled, 8, 0, F::a8_unorm, k_id, C::none, false, false, true, true},
    /* [rnndb] A8L8; [game] a8y8 -> 0x1a. Low byte L, high byte A. */
    {0x1a, "SZ_A8L8", L::swizzled, 16, 0, F::rg8_unorm, k_rrrg, C::none, false, false, true, true},
    /* [rnndb] I8_RECT (NV20); [game] ay8 linear -> 0x1b */
    {0x1b, "LU_I8", L::linear, 8, 0, F::r8_unorm, k_rrrr, C::none, false, false, true, true},
    /* UNVERIFIED: [game] function_1d4b0 counts 0x1c as linear but maps no
       bitmap format to it; X1R5G5B5 is an assumption (the linear partner of
       0x03, as 0x10 is of 0x02). */
    {0x1c, "LU_X1R5G5B5?", L::linear, 16, 0, F::rgba8_unorm, k_id, C::x1r5g5b5_to_rgba8, false, false, false, false},
    /* [rnndb] A4R4G4B4_RECT; [game] a4r4g4b4 linear -> 0x1d */
    {0x1d, "LU_A4R4G4B4", L::linear, 16, 0, F::rgba8_unorm, k_id, C::a4r4g4b4_to_rgba8, false, false, true, true},
    /* [rnndb] R8G8B8_RECT, [wiki] LU_IMAGE_X8R8G8B8; [game] x8r8g8b8 linear */
    {0x1e, "LU_X8R8G8B8", L::linear, 32, 0, F::bgra8_unorm, k_rgb1, C::none, false, false, true, true},
    /* [game] a8 linear -> 0x1f */
    {0x1f, "LU_A8", L::linear, 8, 0, F::a8_unorm, k_id, C::none, false, false, true, true},
    /* [rnndb] A8L8_RECT; [game] a8y8 linear -> 0x20 */
    {0x20, "LU_A8L8", L::linear, 16, 0, F::rg8_unorm, k_rrrg, C::none, false, false, true, true},
    /* [rnndb] DSDT8; [game] v8u8/g8b8 -> 0x28 */
    {0x28, "SZ_DSDT8", L::swizzled, 16, 0, F::rg8_snorm, k_rg11, C::none, true, false, true, true},
    /* [rnndb] Z24 (24-bit depth over 8-bit stencil) */
    {0x2a, "SZ_Z24S8", L::swizzled, 32, 0, F::r32_float, k_rrr1, C::d24s8_to_r32f, false, true, false, true},
    /* [rnndb] Z16 */
    {0x2c, "SZ_Z16", L::swizzled, 16, 0, F::r16_unorm, k_rrr1, C::none, false, true, false, true},
    /* [game] function_1d6f0 builds linear depth surfaces with 0x2e (the
       game's D24S8 depth buffer; rnndb guesses 0x2b for Z24_RECT) */
    {0x2e, "LU_Z24S8", L::linear, 32, 0, F::r32_float, k_rrr1, C::d24s8_to_r32f, false, true, true, true},
    /* [wiki] LU_IMAGE_DEPTH_Y16_FIXED */
    {0x30, "LU_Z16", L::linear, 16, 0, F::r16_unorm, k_rrr1, C::none, false, true, false, true},
    /* UNVERIFIED for NV2A: [rnndb] A16 (NV30 variant, "also L16/I16") */
    {0x32, "SZ_L16?", L::swizzled, 16, 0, F::r16_unorm, k_rrr1, C::none, false, false, false, false},
    /* [rnndb] HILO16; [game] v16u16 bitmaps (flag 0x20, formats 10/11) ->
       0x33. Taken as signed U (low word), V (high word). */
    {0x33, "SZ_HILO16", L::swizzled, 32, 0, F::rg16_snorm, k_rg11, C::none, true, false, true, true},
    /* UNVERIFIED for NV2A: [rnndb] A16_RECT (NV30); [game] 0x35 is linear */
    {0x35, "LU_L16?", L::linear, 16, 0, F::r16_unorm, k_rrr1, C::none, false, false, false, false},
    /* [rnndb] HILO16_RECT; [game] v16u16 linear -> 0x36 */
    {0x36, "LU_HILO16", L::linear, 32, 0, F::rg16_snorm, k_rg11, C::none, true, false, true, true},
    /* [wiki] SZ_A8B8G8R8. Bytes R, G, B, A. */
    {0x3a, "SZ_A8B8G8R8", L::swizzled, 32, 0, F::rgba8_unorm, k_id, C::none, false, false, false, true},
    /* [wiki] LU_IMAGE_A8B8G8R8; [game] 0x3f is linear */
    {0x3f, "LU_A8B8G8R8", L::linear, 32, 0, F::rgba8_unorm, k_id, C::none, false, false, false, true},
    /* [rnndb] HILO8 / SIGNED_HILO8 and their _RECT forms (NV20) */
    {0x44, "SZ_HILO8", L::swizzled, 16, 0, F::rg8_unorm, k_rg11, C::none, false, false, false, true},
    {0x45, "SZ_SIGNED_HILO8", L::swizzled, 16, 0, F::rg8_snorm, k_rg11, C::none, true, false, false, true},
    {0x46, "LU_HILO8", L::linear, 16, 0, F::rg8_unorm, k_rg11, C::none, false, false, false, true},
    {0x47, "LU_SIGNED_HILO8", L::linear, 16, 0, F::rg8_snorm, k_rg11, C::none, true, false, false, true},
};

/* g_450768, src/unknown_0158f0.cpp */
constexpr std::int32_t k_game_codes[2][24] = {
    {0x19, 0, 1, 0x1a, -1, -1, 5, -1, 2, 4, 7, 6, -1, -1, 0xc, 0xe, 0xf, 0xb, 0xb, -1, -1, -1, 0x28, 0x28},
    {0x1f, 0x13, 0x1b, 0x20, -1, -1, 0x11, -1, 0x10, 0x1d, 0x1e, 0x12, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1, 0x17, 0x17},
};

std::uint32_t at_least_one(std::uint32_t value) { return value ? value : 1; }

std::uint32_t align_up(std::uint32_t value, std::uint32_t alignment)
{
    return (value + alignment - 1) & ~(alignment - 1);
}

/* per-axis contributions to the swizzled index */
struct SwizzleTables {
    std::vector<std::uint32_t> x, y, z;
};

SwizzleTables swizzle_tables(std::uint32_t width, std::uint32_t height, std::uint32_t depth)
{
    std::uint32_t const sizes[3] = {width, height, depth};
    std::uint32_t shift[3][32] = {};
    std::uint32_t out_bit = 0;
    for (std::uint32_t bit = 0; bit < 32; ++bit) {
        bool any = false;
        for (int axis = 0; axis < 3; ++axis) {
            if ((std::uint64_t{1} << bit) < sizes[axis]) {
                shift[axis][bit] = out_bit++;
                any = true;
            }
        }
        if (!any)
            break;
    }
    SwizzleTables tables;
    std::vector<std::uint32_t> *const out[3] = {&tables.x, &tables.y, &tables.z};
    for (int axis = 0; axis < 3; ++axis) {
        out[axis]->resize(sizes[axis]);
        for (std::uint32_t v = 0; v < sizes[axis]; ++v) {
            std::uint32_t index = 0;
            for (std::uint32_t bit = 0; (std::uint64_t{1} << bit) < sizes[axis]; ++bit)
                index |= ((v >> bit) & 1u) << shift[axis][bit];
            (*out[axis])[v] = index;
        }
    }
    return tables;
}

template <bool ToLinear>
void swizzle_copy(std::uint8_t const *source, std::uint8_t *destination, std::uint32_t width,
                  std::uint32_t height, std::uint32_t depth, std::uint32_t bpp)
{
    width = at_least_one(width);
    height = at_least_one(height);
    depth = at_least_one(depth);
    SwizzleTables const t = swizzle_tables(width, height, depth);
    std::size_t linear = 0;
    for (std::uint32_t z = 0; z < depth; ++z) {
        for (std::uint32_t y = 0; y < height; ++y) {
            std::uint32_t const zy = t.z[z] | t.y[y];
            for (std::uint32_t x = 0; x < width; ++x, ++linear) {
                std::size_t const swizzled = static_cast<std::size_t>(zy | t.x[x]) * bpp;
                if constexpr (ToLinear)
                    std::memcpy(destination + linear * bpp, source + swizzled, bpp);
                else
                    std::memcpy(destination + swizzled, source + linear * bpp, bpp);
            }
        }
    }
}

std::uint16_t u16(std::uint8_t const *p) { return static_cast<std::uint16_t>(p[0] | (p[1] << 8)); }

std::uint32_t u32(std::uint8_t const *p)
{
    return p[0] | (p[1] << 8) | (p[2] << 16) | (static_cast<std::uint32_t>(p[3]) << 24);
}

std::uint8_t expand5(std::uint32_t v) { return static_cast<std::uint8_t>((v << 3) | (v >> 2)); }
std::uint8_t expand6(std::uint32_t v) { return static_cast<std::uint8_t>((v << 2) | (v >> 4)); }
std::uint8_t expand4(std::uint32_t v) { return static_cast<std::uint8_t>(v * 17); }

/* the level's texels in source encoding, unswizzled / pitch stripped,
   tightly packed rows (rows of blocks for DXT) */
bool extract_level(TextureDescription const &d, std::span<std::uint8_t const> texels,
                   std::uint32_t face, std::uint32_t level, std::vector<std::uint8_t> &tight,
                   std::uint32_t &tight_row)
{
    if (!d.format || level >= d.levels || face >= d.faces)
        return false;
    MipLevel const &m = d.mips[level];
    std::size_t const offset = d.offset_of(face, level);
    if (offset > texels.size() || m.size > texels.size() - offset)
        return false;
    std::uint8_t const *source = texels.data() + offset;
    switch (d.layout) {
    case TexelLayout::swizzled:
        tight_row = m.width * d.bytes_per_texel;
        tight.resize(m.size);
        swizzle_copy<true>(source, tight.data(), m.width, m.height, m.depth, d.bytes_per_texel);
        return true;
    case TexelLayout::compressed:
        tight_row = m.row_pitch;
        tight.assign(source, source + m.size);
        return true;
    case TexelLayout::linear:
        tight_row = m.width * d.bytes_per_texel;
        tight.resize(static_cast<std::size_t>(tight_row) * m.height);
        for (std::uint32_t y = 0; y < m.height; ++y)
            std::memcpy(tight.data() + static_cast<std::size_t>(y) * tight_row,
                        source + static_cast<std::size_t>(y) * m.row_pitch, tight_row);
        return true;
    }
    return false;
}

void put(std::uint8_t *o, std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a)
{
    o[0] = r;
    o[1] = g;
    o[2] = b;
    o[3] = a;
}

/* tight source texels -> upload format, per the table's conversion */
bool convert(HardwareFormat const &f, std::vector<std::uint8_t> const &tight, std::size_t texels,
             std::span<std::uint32_t const> palette, std::vector<std::uint8_t> &out)
{
    switch (f.conversion) {
    case C::none:
        out = tight;
        return true;
    case C::a1r5g5b5_to_rgba8:
    case C::x1r5g5b5_to_rgba8:
        out.resize(texels * 4);
        for (std::size_t i = 0; i < texels; ++i) {
            std::uint32_t const v = u16(&tight[i * 2]);
            std::uint8_t const a = f.conversion == C::x1r5g5b5_to_rgba8 ? 255 : (v & 0x8000 ? 255 : 0);
            put(&out[i * 4], expand5((v >> 10) & 31), expand5((v >> 5) & 31), expand5(v & 31), a);
        }
        return true;
    case C::a4r4g4b4_to_rgba8:
        out.resize(texels * 4);
        for (std::size_t i = 0; i < texels; ++i) {
            std::uint32_t const v = u16(&tight[i * 2]);
            put(&out[i * 4], expand4((v >> 8) & 15), expand4((v >> 4) & 15), expand4(v & 15),
                expand4(v >> 12));
        }
        return true;
    case C::r5g6b5_to_rgba8:
        out.resize(texels * 4);
        for (std::size_t i = 0; i < texels; ++i) {
            std::uint32_t const v = u16(&tight[i * 2]);
            put(&out[i * 4], expand5(v >> 11), expand6((v >> 5) & 63), expand5(v & 31), 255);
        }
        return true;
    case C::p8_to_rgba8:
        if (palette.size() < 256)
            return false;
        out.resize(texels * 4);
        for (std::size_t i = 0; i < texels; ++i) {
            std::uint32_t const c = palette[tight[i]];
            put(&out[i * 4], static_cast<std::uint8_t>(c >> 16), static_cast<std::uint8_t>(c >> 8),
                static_cast<std::uint8_t>(c), static_cast<std::uint8_t>(c >> 24));
        }
        return true;
    case C::d24s8_to_r32f:
        out.resize(texels * 4);
        for (std::size_t i = 0; i < texels; ++i) {
            float const depth = static_cast<float>(u32(&tight[i * 4]) >> 8) / 16777215.0f;
            std::memcpy(&out[i * 4], &depth, 4);
        }
        return true;
    case C::unsupported:
        break;
    }
    return false;
}

}  // namespace

HardwareFormat const *hardware_format(std::uint32_t code)
{
    for (HardwareFormat const &f : k_formats)
        if (f.code == code)
            return &f;
    return nullptr;
}

std::span<HardwareFormat const> hardware_formats() { return k_formats; }

std::int32_t game_bitmap_hardware_code(int bitmap_format, bool linear, bool v16u16)
{
    if (bitmap_format < 0 || bitmap_format >= 24)
        return -1;
    if (v16u16 && (bitmap_format == 10 || bitmap_format == 11))
        return linear ? 0x36 : 0x33;
    return k_game_codes[linear ? 1 : 0][bitmap_format];
}

bool describe_texture(std::uint32_t format_word, std::uint32_t size_word, TextureDescription &out)
{
    out = TextureDescription{};
    out.format_word = format_word;
    out.size_word = size_word;
    out.code = (format_word >> 8) & 0xff;
    out.format = hardware_format(out.code);
    if (!out.format)
        return false;
    HardwareFormat const &f = *out.format;
    out.layout = f.layout;
    out.swizzled = f.layout == TexelLayout::swizzled;
    out.linear = f.layout == TexelLayout::linear;
    out.compressed = f.layout == TexelLayout::compressed;
    out.bytes_per_block = f.block_bytes;
    out.bytes_per_texel = f.block_bytes ? 0 : f.bits_per_texel / 8;

    std::uint32_t const dims = (format_word >> 4) & 0xf;
    bool const cube = (format_word & 4) != 0;
    if (out.linear) {
        /* Size: width - 1, height - 1 << 12, pitch / 64 - 1 << 24 (so a
           zero Size is a 1x1 texture with a 64-byte pitch) */
        if (cube)
            return false;
        out.kind = TextureKind::texture_2d;
        out.width = (size_word & 0xfff) + 1;
        out.height = ((size_word >> 12) & 0xfff) + 1;
        out.depth = 1;
        out.pitch = ((size_word >> 24) + 1) * 64;
        out.levels = 1;
        if (out.pitch < out.width * out.bytes_per_texel)
            return false;
        MipLevel &m = out.mips[0];
        m.width = out.width;
        m.height = out.height;
        m.depth = 1;
        m.row_pitch = out.pitch;
        m.rows = out.height;
        m.slice_pitch = m.size = out.pitch * out.height;
        out.total_size = m.size;
        out.face_stride = m.size;
        return true;
    }

    if (dims != 2 && dims != 3)
        return false;
    if (cube && dims != 2)
        return false;
    out.kind = cube ? TextureKind::cube : dims == 3 ? TextureKind::volume : TextureKind::texture_2d;
    out.width = 1u << ((format_word >> 20) & 0xf);
    out.height = 1u << ((format_word >> 24) & 0xf);
    out.depth = dims == 3 ? 1u << ((format_word >> 28) & 0xf) : 1;
    if (cube && out.width != out.height)
        return false;
    out.levels = std::clamp<std::uint32_t>((format_word >> 16) & 0xf, 1, k_max_levels);
    out.faces = cube ? 6 : 1;

    std::uint32_t offset = 0;
    for (std::uint32_t i = 0; i < out.levels; ++i) {
        MipLevel &m = out.mips[i];
        m.width = at_least_one(out.width >> i);
        m.height = at_least_one(out.height >> i);
        m.depth = at_least_one(out.depth >> i);
        if (out.compressed) {
            m.row_pitch = ((m.width + 3) / 4) * f.block_bytes;
            m.rows = (m.height + 3) / 4;
        } else {
            m.row_pitch = m.width * out.bytes_per_texel;
            m.rows = m.height;
        }
        m.slice_pitch = m.row_pitch * m.rows;
        m.size = m.slice_pitch * m.depth;
        m.offset = offset;
        offset += m.size;
    }
    /* [maps] cube faces are padded to 128 bytes; chains themselves are packed */
    out.face_stride = cube ? align_up(offset, k_cube_face_alignment) : offset;
    out.total_size = out.face_stride * out.faces;
    return true;
}

std::uint32_t swizzle_index(std::uint32_t x, std::uint32_t y, std::uint32_t z, std::uint32_t width,
                            std::uint32_t height, std::uint32_t depth)
{
    std::uint32_t const sizes[3] = {width, height, depth};
    std::uint32_t const coords[3] = {x, y, z};
    std::uint32_t index = 0, out_bit = 0;
    for (std::uint32_t bit = 0; bit < 32; ++bit) {
        bool any = false;
        for (int axis = 0; axis < 3; ++axis) {
            if ((std::uint64_t{1} << bit) < sizes[axis]) {
                index |= ((coords[axis] >> bit) & 1u) << out_bit++;
                any = true;
            }
        }
        if (!any)
            break;
    }
    return index;
}

void unswizzle(std::uint8_t const *source, std::uint8_t *destination, std::uint32_t width,
               std::uint32_t height, std::uint32_t depth, std::uint32_t bytes_per_texel)
{
    swizzle_copy<true>(source, destination, width, height, depth, bytes_per_texel);
}

void swizzle(std::uint8_t const *source, std::uint8_t *destination, std::uint32_t width,
             std::uint32_t height, std::uint32_t depth, std::uint32_t bytes_per_texel)
{
    swizzle_copy<false>(source, destination, width, height, depth, bytes_per_texel);
}

bool prepare_upload(TextureDescription const &description, std::span<std::uint8_t const> texels,
                    std::uint32_t face, std::uint32_t level, UploadImage &out,
                    std::span<std::uint32_t const> palette, UploadOptions options)
{
    out = UploadImage{};
    std::vector<std::uint8_t> tight;
    std::uint32_t tight_row = 0;
    if (!extract_level(description, texels, face, level, tight, tight_row))
        return false;
    HardwareFormat const &f = *description.format;
    MipLevel const &m = description.mips[level];
    std::size_t const count = static_cast<std::size_t>(m.width) * m.height * m.depth;

    out.width = m.width;
    out.height = m.height;
    out.depth = m.depth;
    out.format = f.upload_format;
    out.swizzle = f.swizzle;

    bool const is_32bit_colour = f.upload_format == F::bgra8_unorm || f.upload_format == F::rgba8_unorm;
    if (options.signed_rgba8 && is_32bit_colour && f.conversion == C::none) {
        /* same channel placement, components reinterpreted as signed */
        out.bytes.resize(count * 4);
        bool const bgra = f.upload_format == F::bgra8_unorm;
        for (std::size_t i = 0; i < count; ++i) {
            std::uint8_t const *s = &tight[i * 4];
            put(&out.bytes[i * 4], bgra ? s[2] : s[0], s[1], bgra ? s[0] : s[2], s[3]);
        }
        out.format = F::rgba8_snorm;
    } else if (!convert(f, tight, count, palette, out.bytes)) {
        return false;
    }

    if (description.compressed) {
        out.bytes_per_row = m.row_pitch;
        out.bytes_per_image = m.slice_pitch;
    } else {
        std::uint32_t const out_bpp = f.conversion == C::none ? description.bytes_per_texel : 4;
        out.bytes_per_row = m.width * out_bpp;
        out.bytes_per_image = out.bytes_per_row * m.height;
    }
    return true;
}

bool decode_rgba8(TextureDescription const &description, std::span<std::uint8_t const> texels,
                  std::uint32_t face, std::uint32_t level, std::vector<std::uint8_t> &rgba,
                  std::span<std::uint32_t const> palette)
{
    std::vector<std::uint8_t> tight;
    std::uint32_t tight_row = 0;
    if (!extract_level(description, texels, face, level, tight, tight_row))
        return false;
    HardwareFormat const &f = *description.format;
    MipLevel const &m = description.mips[level];
    std::size_t const count = static_cast<std::size_t>(m.width) * m.height * m.depth;

    if (description.compressed) {
        rgba.resize(count * 4);
        std::size_t const slice_out = static_cast<std::size_t>(m.width) * m.height * 4;
        std::vector<std::uint8_t> slice;
        for (std::uint32_t z = 0; z < m.depth; ++z) {
            std::span<std::uint8_t const> blocks(tight.data() + static_cast<std::size_t>(z) * m.slice_pitch,
                                                 m.slice_pitch);
            bool ok = f.code == 0x0c   ? h2::port::decode_dxt1(blocks, m.width, m.height, slice)
                      : f.code == 0x0e ? h2::port::decode_dxt3(blocks, m.width, m.height, slice)
                                       : h2::port::decode_dxt5(blocks, m.width, m.height, slice);
            if (!ok)
                return false;
            std::memcpy(rgba.data() + z * slice_out, slice.data(), slice_out);
        }
        return true;
    }
    if (f.conversion != C::none && f.conversion != C::d24s8_to_r32f)
        return convert(f, tight, count, palette, rgba);

    rgba.resize(count * 4);
    auto snorm = [](std::uint8_t v) { return static_cast<std::uint8_t>(static_cast<std::int8_t>(v) + 128); };
    for (std::size_t i = 0; i < count; ++i) {
        std::uint8_t *o = &rgba[i * 4];
        std::uint8_t const *s = &tight[i * description.bytes_per_texel];
        switch (f.upload_format) {
        case F::bgra8_unorm:
            put(o, s[2], s[1], s[0], f.swizzle.alpha == S::one ? 255 : s[3]);
            break;
        case F::rgba8_unorm:
            put(o, s[0], s[1], s[2], s[3]);
            break;
        case F::a8_unorm:
            put(o, 0, 0, 0, s[0]);
            break;
        case F::r8_unorm:
            put(o, s[0], s[0], s[0], f.swizzle.alpha == S::red ? s[0] : 255);
            break;
        case F::rg8_unorm:
            if (f.swizzle.alpha == S::green)
                put(o, s[0], s[0], s[0], s[1]);
            else
                put(o, s[0], s[1], 255, 255);
            break;
        case F::rg8_snorm:
            put(o, snorm(s[0]), snorm(s[1]), 255, 255);
            break;
        case F::rg16_snorm:
            put(o, snorm(s[1]), snorm(s[3]), 255, 255);
            break;
        case F::r16_unorm:
            put(o, s[1], s[1], s[1], 255);
            break;
        case F::r32_float:
            put(o, s[3], s[3], s[3], 255);
            break;
        default:
            return false;
        }
    }
    return true;
}

}  // namespace h2::gpu
