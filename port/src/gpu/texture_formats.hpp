/* NV2A texture resources for the Metal backend.
 *
 * The guest D3D runtime hands the renderer, per texture stage, the raw
 * Format and Size words of a D3DBaseTexture header plus a host pointer to
 * the texels (and to the palette for P8). This module
 *
 *   - describes a texture from those two words (kind, sizes, level count,
 *     hardware format, layout, and the byte offset and size of every mip
 *     level of every cube face),
 *   - lists the NV2A texture format codes the game can produce, each with
 *     the Metal pixel format the backend uploads it as and whether the
 *     texels go up as they are (after unswizzling) or need converting,
 *   - unswizzles (Morton/Z order over the power-of-two sizes) and converts
 *     one level of one face into tightly packed rows of that upload format.
 *
 * The Format word (NV097_SET_TEXTURE_FORMAT, as the recovered code builds
 * it: src/unknown_12c0d0.cpp function_12d6d0, src/unknown_0359e0.cpp
 * function_1d6f0; field names from envytools rnndb graph/nv20_3d.xml, MIT):
 *   bits 0-1   DMA context            bit 2   cube map
 *   bit 3      no border              bits 4-7  dimension count (2 or 3)
 *   bits 8-15  hardware format code   bits 16-19 mip level count
 *   bits 20-23 log2 width  bits 24-27 log2 height  bits 28-31 log2 depth
 * Linear formats carry their geometry in the Size word instead
 * (src/unknown_12c0d0.cpp function_12d6d0; xboxdevwiki "XPR"):
 *   bits 0-11 width - 1, bits 12-23 height - 1, bits 24-31 pitch / 64 - 1.
 *
 * Nothing here comes from SDK headers or GPL emulator sources. */
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace h2::gpu {

/* Raw MTLPixelFormat values (Metal's public MTLPixelFormat enum), so this
   file needs no Objective-C. */
enum class MtlPixelFormat : std::uint32_t {
    invalid = 0,
    a8_unorm = 1,
    r8_unorm = 10,
    r16_unorm = 20,
    rg8_unorm = 30,
    rg8_snorm = 32,
    r32_float = 55,
    rg16_snorm = 62,
    rgba8_unorm = 70,
    rgba8_snorm = 72,
    bgra8_unorm = 80,
    bc1_rgba = 130,
    bc2_rgba = 132,
    bc3_rgba = 134,
};

/* Raw MTLTextureSwizzle values, for MTLTextureSwizzleChannels on the
   texture (or a view of it) so that one- and two-channel uploads sample
   the way the NV2A returns them. */
enum class MtlSwizzle : std::uint8_t { zero = 0, one = 1, red = 2, green = 3, blue = 4, alpha = 5 };

struct SwizzleChannels {
    MtlSwizzle red, green, blue, alpha;
};
inline constexpr SwizzleChannels k_identity_swizzle{MtlSwizzle::red, MtlSwizzle::green,
                                                    MtlSwizzle::blue, MtlSwizzle::alpha};

enum class TexelLayout : std::uint8_t {
    swizzled,    /* Morton order over power-of-two sizes, packed mip chain */
    linear,      /* rows at a 64-byte multiple pitch, one level */
    compressed,  /* DXT 4x4 blocks in row-major block order, packed mip chain */
};

/* How the texels reach the upload format. */
enum class Conversion : std::uint8_t {
    none,              /* bytes are already the upload format (after unswizzle) */
    a1r5g5b5_to_rgba8,
    x1r5g5b5_to_rgba8,
    a4r4g4b4_to_rgba8,
    r5g6b5_to_rgba8,
    p8_to_rgba8,       /* 256-entry D3DCOLOR (ARGB) palette */
    d24s8_to_r32f,     /* depth in bits 8-31, stencil dropped */
    unsupported,
};

enum class TextureKind : std::uint8_t { texture_2d, cube, volume };

struct HardwareFormat {
    std::uint8_t code;
    char const *name;
    TexelLayout layout;
    std::uint8_t bits_per_texel;  /* 4 for DXT1, 8 for DXT3/5 */
    std::uint8_t block_bytes;     /* 8 or 16 for DXT, 0 otherwise */
    MtlPixelFormat upload_format;
    SwizzleChannels swizzle;      /* to apply to the Metal texture */
    Conversion conversion;
    bool is_signed;               /* upload is a signed-normalised format */
    bool is_depth;
    bool produced_by_game;        /* reachable from the game's own tables */
    bool verified;                /* meaning backed by a cited source */
};

/* nullptr for codes not in the table */
HardwareFormat const *hardware_format(std::uint32_t code);
std::span<HardwareFormat const> hardware_formats();

/* The game's bitmap format -> hardware code tables (g_450768,
   src/unknown_0158f0.cpp), -1 for unsupported. v16u16 marks bitmaps with
   flag 0x20 in formats 10/11 (src/unknown_12c0d0.cpp). */
std::int32_t game_bitmap_hardware_code(int bitmap_format, bool linear, bool v16u16 = false);

struct MipLevel {
    std::uint32_t width = 0, height = 0, depth = 0;
    std::uint32_t offset = 0;       /* from the start of the face */
    std::uint32_t size = 0;         /* bytes, all slices */
    std::uint32_t row_pitch = 0;    /* source bytes per row (of blocks, for DXT) */
    std::uint32_t rows = 0;         /* rows (block rows for DXT) per slice */
    std::uint32_t slice_pitch = 0;  /* row_pitch * rows */
};

inline constexpr std::uint32_t k_max_levels = 16;
inline constexpr std::uint32_t k_cube_face_alignment = 128;

struct TextureDescription {
    std::uint32_t format_word = 0, size_word = 0;
    std::uint32_t code = 0;
    HardwareFormat const *format = nullptr;
    TextureKind kind = TextureKind::texture_2d;
    TexelLayout layout = TexelLayout::swizzled;
    bool swizzled = false, linear = false, compressed = false;
    std::uint32_t width = 0, height = 0, depth = 0;
    std::uint32_t levels = 0;
    std::uint32_t faces = 1;
    std::uint32_t bytes_per_texel = 0;  /* 0 for DXT */
    std::uint32_t bytes_per_block = 0;  /* 0 unless DXT */
    std::uint32_t pitch = 0;            /* linear only */
    std::uint32_t face_stride = 0;      /* bytes between cube faces */
    std::uint32_t total_size = 0;       /* bytes the texture occupies */
    std::array<MipLevel, k_max_levels> mips{};

    std::uint32_t offset_of(std::uint32_t face, std::uint32_t level) const
    {
        return face * face_stride + mips[level].offset;
    }
};

/* false if the code is unknown or the words are inconsistent */
bool describe_texture(std::uint32_t format_word, std::uint32_t size_word, TextureDescription &out);

/* ---- swizzling -------------------------------------------------------- */

/* Byte-agnostic texel index of (x, y, z) in a swizzled image of the given
   power-of-two sizes: one bit of each axis in turn (x lowest, then y, then
   z) while that axis still has bits, so the larger axes' surplus bits end
   up on top. For 2D this is the game's own interleave (function_358d0,
   src/unknown_0359e0.cpp). */
std::uint32_t swizzle_index(std::uint32_t x, std::uint32_t y, std::uint32_t z, std::uint32_t width,
                            std::uint32_t height, std::uint32_t depth);

/* Swizzled -> tightly packed rows (x fastest, then y, then z). */
void unswizzle(std::uint8_t const *source, std::uint8_t *destination, std::uint32_t width,
               std::uint32_t height, std::uint32_t depth, std::uint32_t bytes_per_texel);
/* Tightly packed -> swizzled. */
void swizzle(std::uint8_t const *source, std::uint8_t *destination, std::uint32_t width,
             std::uint32_t height, std::uint32_t depth, std::uint32_t bytes_per_texel);

/* ---- upload ----------------------------------------------------------- */

struct UploadImage {
    MtlPixelFormat format = MtlPixelFormat::invalid;
    SwizzleChannels swizzle = k_identity_swizzle;
    std::uint32_t width = 0, height = 0, depth = 0;
    std::uint32_t bytes_per_row = 0;    /* row of texels, or of 4x4 blocks */
    std::uint32_t bytes_per_image = 0;  /* one slice */
    std::vector<std::uint8_t> bytes;    /* depth slices, tightly packed */
};

struct UploadOptions {
    /* Treat a 32-bit colour format as having signed components (the
       NV2A's per-channel signed sampling, e.g. a D3D Q8W8V8U8 texture):
       the upload becomes RGBA8Snorm with the same channel placement. */
    bool signed_rgba8 = false;
};

/* One level of one cube face (face 0 for non-cubes), all slices.
   `texels` starts at the texture's Data; `palette` is the 256 D3DCOLORs
   of a P8 texture. */
bool prepare_upload(TextureDescription const &description, std::span<std::uint8_t const> texels,
                    std::uint32_t face, std::uint32_t level, UploadImage &out,
                    std::span<std::uint32_t const> palette = {}, UploadOptions options = {});

/* Fallback / debugging: any format to RGBA8 (R, G, B, A bytes), DXT
   decompressed, signed components biased to v * 0.5 + 0.5, depth as grey. */
bool decode_rgba8(TextureDescription const &description, std::span<std::uint8_t const> texels,
                  std::uint32_t face, std::uint32_t level, std::vector<std::uint8_t> &rgba,
                  std::span<std::uint32_t const> palette = {});

}  // namespace h2::gpu
