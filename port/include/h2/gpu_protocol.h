/* The guest D3D runtime's draw packets to the host renderer.
 *
 * port/runtime/guest_d3d_runtime.cpp keeps the Xbox Direct3D state the game
 * sets and, at every draw, clear and present, hands the host one of these
 * packets. The guest compiles this header as ILP32 and the host as LP64, so
 * every field is a fixed-size integer or float; guest pointers travel as
 * 32-bit guest addresses the host turns into host pointers itself.
 *
 * Fixed-function state arrives normalised to the enums below, so the host
 * never sees the port's provisional D3D enum values. Data whose encoding the
 * hardware defines travels raw: the pixel shader definition (register
 * combiner words), vertex program microcode, vertex attribute format bytes and
 * texture format words. */
#ifndef H2_GPU_PROTOCOL_H
#define H2_GPU_PROTOCOL_H

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int h2_gpu_u32;
typedef int h2_gpu_s32;
typedef unsigned char h2_gpu_u8;

#define H2_GPU_PROTOCOL_VERSION 1

enum h2_gpu_packet_kind {
    H2_GPU_PACKET_DRAW = 1,
    H2_GPU_PACKET_CLEAR = 2,
};

enum h2_gpu_primitive {
    H2_GPU_POINTS = 1,
    H2_GPU_LINES = 2,
    H2_GPU_LINE_LOOP = 3,
    H2_GPU_LINE_STRIP = 4,
    H2_GPU_TRIANGLES = 5,
    H2_GPU_TRIANGLE_STRIP = 6,
    H2_GPU_TRIANGLE_FAN = 7,
    H2_GPU_QUADS = 8,
    H2_GPU_QUAD_STRIP = 9,
    H2_GPU_POLYGON = 10,
};

enum h2_gpu_compare {
    H2_GPU_NEVER, H2_GPU_LESS, H2_GPU_EQUAL, H2_GPU_LESS_EQUAL,
    H2_GPU_GREATER, H2_GPU_NOT_EQUAL, H2_GPU_GREATER_EQUAL, H2_GPU_ALWAYS,
};

enum h2_gpu_blend_factor {
    H2_GPU_ZERO, H2_GPU_ONE,
    H2_GPU_SRC_COLOR, H2_GPU_INV_SRC_COLOR, H2_GPU_SRC_ALPHA, H2_GPU_INV_SRC_ALPHA,
    H2_GPU_DST_ALPHA, H2_GPU_INV_DST_ALPHA, H2_GPU_DST_COLOR, H2_GPU_INV_DST_COLOR,
    H2_GPU_SRC_ALPHA_SATURATE,
    H2_GPU_CONSTANT_COLOR, H2_GPU_INV_CONSTANT_COLOR,
    H2_GPU_CONSTANT_ALPHA, H2_GPU_INV_CONSTANT_ALPHA,
};

enum h2_gpu_blend_op {
    H2_GPU_ADD, H2_GPU_SUBTRACT, H2_GPU_REVERSE_SUBTRACT, H2_GPU_MIN, H2_GPU_MAX,
};

enum h2_gpu_stencil_op {
    H2_GPU_KEEP, H2_GPU_STENCIL_ZERO, H2_GPU_REPLACE, H2_GPU_INCREMENT_CLAMP,
    H2_GPU_DECREMENT_CLAMP, H2_GPU_INVERT, H2_GPU_INCREMENT_WRAP, H2_GPU_DECREMENT_WRAP,
};

enum h2_gpu_cull {
    H2_GPU_CULL_NONE, H2_GPU_CULL_CLOCKWISE, H2_GPU_CULL_COUNTER_CLOCKWISE,
};

enum h2_gpu_address {
    H2_GPU_WRAP, H2_GPU_MIRROR, H2_GPU_CLAMP, H2_GPU_BORDER, H2_GPU_CLAMP_TO_EDGE,
};

enum h2_gpu_filter {
    H2_GPU_FILTER_NONE, H2_GPU_FILTER_POINT, H2_GPU_FILTER_LINEAR,
};

/* how a draw's vertices reach the host */
enum h2_gpu_vertex_source {
    /* attribute arrays in guest memory, described by vertex_arrays */
    H2_GPU_VERTICES_ARRAYS = 1,
    /* Begin/End or an inline push array: float4 registers per vertex,
       inline_vertex_count vertices of 16 registers at inline_vertices */
    H2_GPU_VERTICES_INLINE = 2,
};

/* how vertices are processed */
enum h2_gpu_vertex_mode {
    /* the NV2A vertex program at vertex_program runs */
    H2_GPU_VERTEX_PROGRAM = 1,
    /* fixed function with already transformed (screen-space x, y, z, rhw)
       positions in register 0: the D3DFVF_XYZRHW path */
    H2_GPU_VERTEX_SCREEN = 2,
};

/* One vertex attribute register's source array. format is the NV2A vertex
   array format byte (low nibble type, high nibble component count; 0x02, a
   float with no components, means "unused"). */
typedef struct h2_gpu_vertex_array {
    h2_gpu_u32 address;   /* guest address of the first vertex's element */
    h2_gpu_u32 stride;    /* bytes between vertices */
    h2_gpu_u32 format;
} h2_gpu_vertex_array;

/* One texture stage. Fields describe the resource the game bound, copied
   from its D3D header, plus the stage's sampler state. */
typedef struct h2_gpu_texture {
    h2_gpu_u32 header;    /* guest address of the D3DBaseTexture header, 0 = none */
    h2_gpu_u32 data;      /* guest address of level 0 of the texels */
    h2_gpu_u32 format;    /* the header's Format word, raw */
    h2_gpu_u32 size;      /* the header's Size word, raw (linear formats) */
    h2_gpu_u32 palette;   /* guest address of palette entries, 0 = none */
    h2_gpu_u32 address_u, address_v, address_w;   /* h2_gpu_address */
    h2_gpu_u32 mag_filter, min_filter, mip_filter;  /* h2_gpu_filter */
    h2_gpu_u32 border_color;   /* ARGB */
    float lod_bias;
    h2_gpu_u32 max_mip_level;
    h2_gpu_u32 color_key;      /* ARGB; 0 when colour keying is off */
    h2_gpu_u32 color_key_op;   /* 0 off; the raw D3DTSS_COLORKEYOP otherwise */
} h2_gpu_texture;

/* Fixed-function texture stage combining, used only when no pixel shader
   program is set: the raw D3DTOP/D3DTA values of the port's d3d8.h. */
typedef struct h2_gpu_stage_ops {
    h2_gpu_u32 color_op, color_arg0, color_arg1, color_arg2;
    h2_gpu_u32 alpha_op, alpha_arg0, alpha_arg1, alpha_arg2;
    h2_gpu_u32 result_arg;
    h2_gpu_u32 texcoord_index;
} h2_gpu_stage_ops;

typedef struct h2_gpu_draw {
    h2_gpu_u32 kind;               /* H2_GPU_PACKET_DRAW */
    h2_gpu_u32 version;            /* H2_GPU_PROTOCOL_VERSION */

    /* primitives */
    h2_gpu_u32 primitive;          /* h2_gpu_primitive */
    h2_gpu_u32 first_vertex;
    h2_gpu_u32 vertex_count;       /* vertices, or indices when index_data is set */
    h2_gpu_u32 index_data;         /* guest address of 16-bit indices, 0 = none */

    /* vertices */
    h2_gpu_u32 vertex_source;      /* h2_gpu_vertex_source */
    h2_gpu_u32 vertex_mode;        /* h2_gpu_vertex_mode */
    h2_gpu_vertex_array vertex_arrays[16];
    h2_gpu_u32 inline_vertices;    /* guest address: inline_vertex_count x 16 x float4 */
    h2_gpu_u32 inline_vertex_count;
    /* the values of registers no array feeds (SetVertexData*) */
    float vertex_defaults[16][4];

    /* the vertex program: 4-word NV2A instructions, from instruction 0 */
    h2_gpu_u32 vertex_program;     /* guest address of the first instruction */
    h2_gpu_u32 vertex_program_length;   /* instructions */
    h2_gpu_u32 vertex_program_start;    /* instruction the program begins at */
    h2_gpu_u32 vertex_constants;   /* guest address of 192 float4s (c[-96] first) */

    /* the pixel shader: a D3DPIXELSHADERDEF (60 words) when has_pixel_shader */
    h2_gpu_u32 has_pixel_shader;
    h2_gpu_u32 pixel_shader[60];
    h2_gpu_stage_ops stage_ops[4];

    h2_gpu_texture textures[4];

    /* fixed-function raster state (normalised) */
    h2_gpu_u32 blend_enable;
    h2_gpu_u32 blend_src_color, blend_dst_color, blend_op_color;
    h2_gpu_u32 blend_src_alpha, blend_dst_alpha, blend_op_alpha;
    h2_gpu_u32 blend_constant;     /* ARGB */
    h2_gpu_u32 color_write_mask;   /* bit 0 red, 1 green, 2 blue, 3 alpha */
    h2_gpu_u32 alpha_test_enable;
    h2_gpu_u32 alpha_func;         /* h2_gpu_compare */
    float alpha_ref;               /* 0..1 */
    h2_gpu_u32 depth_test_enable;
    h2_gpu_u32 depth_write_enable;
    h2_gpu_u32 depth_func;         /* h2_gpu_compare */
    float depth_bias;              /* constant offset */
    float depth_slope_scale;
    h2_gpu_u32 stencil_enable;
    h2_gpu_u32 stencil_func, stencil_ref, stencil_mask, stencil_write_mask;
    h2_gpu_u32 stencil_fail, stencil_depth_fail, stencil_pass;   /* h2_gpu_stencil_op */
    h2_gpu_u32 cull;               /* h2_gpu_cull */
    h2_gpu_u32 front_face_clockwise;
    h2_gpu_u32 fill_wireframe;
    h2_gpu_u32 fog_enable;
    h2_gpu_u32 fog_color;          /* ARGB */
    h2_gpu_u32 texture_factor;     /* ARGB, D3DRS_TEXTUREFACTOR */

    /* viewport in render-target pixels and its depth range */
    float viewport_x, viewport_y, viewport_width, viewport_height;
    float viewport_min_z, viewport_max_z;
    h2_gpu_u32 scissor_enable;
    h2_gpu_s32 scissor_left, scissor_top, scissor_right, scissor_bottom;

    /* targets: guest addresses of the D3DSurface headers; render_target is 0
       for the back buffer */
    h2_gpu_u32 render_target;
    h2_gpu_u32 depth_stencil;
    h2_gpu_u32 target_width, target_height;   /* pixels of the colour target */
    h2_gpu_u32 two_sided_lighting;            /* back faces take oB0/oB1 */
    /* guest addresses of the back and front buffers' pixels, which textures
       the game builds over them sample */
    h2_gpu_u32 back_buffer_data, front_buffer_data;
    /* 1: inside a visibility test (the push buffer turned on the z-pass
       pixel count); the samples that pass count towards the test the next
       host_h2_gpu_visibility_end closes */
    h2_gpu_u32 visibility_test;
    /* D3DRS_DEPTHCLIPCONTROL's D3DDCC_CLAMP (0x10): depth past the near and
       far planes is clamped instead of clipped (the sky, drawn beyond the
       far plane) */
    h2_gpu_u32 depth_clamp;
    /* points: D3DRS_POINTSCALEENABLE (the vertex program's oPts sizes them;
       otherwise they take D3DRS_POINTSIZE, which the game leaves at 1) and
       D3DRS_POINTSPRITEENABLE (texture stage 3 reads the position within
       the point) */
    h2_gpu_u32 point_scale_enable;
    h2_gpu_u32 point_sprite_enable;
    /* D3DRS_STIPPLEENABLE: a fragment at game pixel (x, y) is drawn only if
       bit x & 31 of stipple[y & 31] is set (the game's dither fades) */
    h2_gpu_u32 stipple_enable;
    h2_gpu_u32 stipple[32];
} h2_gpu_draw;

typedef struct h2_gpu_clear {
    h2_gpu_u32 kind;               /* H2_GPU_PACKET_CLEAR */
    h2_gpu_u32 version;
    h2_gpu_u32 flags;              /* bit 0 colour, bit 1 depth, bit 2 stencil;
                                      colour channels 0x10 red, 0x20 green,
                                      0x40 blue, 0x80 alpha (none: all) */
    h2_gpu_u32 color;              /* ARGB */
    float depth;
    h2_gpu_u32 stencil;
    h2_gpu_u32 rect_count;         /* 0 = the whole viewport */
    h2_gpu_s32 rects[8][4];        /* x1, y1, x2, y2 */
    h2_gpu_u32 render_target;   /* 0 for the back buffer */
    h2_gpu_u32 depth_stencil;
    h2_gpu_u32 target_width, target_height;
} h2_gpu_clear;

#ifdef __cplusplus
}
#endif

#endif
