/* ANIMATION_CHANNEL_DECODERS.H: the layouts of an animation's codec data, as the
   codecs' channel decoders (src/animation_channel_decoders.cpp and
   src/animation_keyframe_decoders.cpp) read it through
   g_sampling_settings.field_30. The first byte picks the codec in g_47fb18;
   every other offset is in bytes from the start of the data. */

#ifndef ANIMATION_CHANNEL_DECODERS_H
#define ANIMATION_CHANNEL_DECODERS_H

#include <stddef.h>
#include "unknown_11c920.h"

/* a rotation quantized to four signed words (8 bytes); the decoders widen
   and normalize it */
struct s_quantized_quaternion
{
	short i;
	short j;
	short k;
	short w;
};
typedef char assert_quantized_quaternion_size[(sizeof(s_quantized_quaternion) == 0x8) ? 1 : -1];

/* the data of the per-frame codecs (_uncompressed_static_data_codec,
   _8byte_quantized_rotation_only_codec, _blend_screen_codec): each node's
   channel is a run of values, `*_stride` bytes per node */
struct s_animation_frame_table
{
	byte codec;
	byte unknown01[0xb];
	long translation_offset;
	long scale_offset;
	long rotation_stride;
	long translation_stride;
	long scale_stride;
	byte rotations[1];
};
typedef char assert_frame_table_translation_offset[(offsetof(s_animation_frame_table, translation_offset) == 0xc) ? 1 : -1];
typedef char assert_frame_table_scale_stride[(offsetof(s_animation_frame_table, scale_stride) == 0x1c) ? 1 : -1];
typedef char assert_frame_table_rotations[(offsetof(s_animation_frame_table, rotations) == 0x20) ? 1 : -1];

/* the data of the keyframe codecs (byte_keyframe_lightly_quantized and its
   word and reverse forms): each node's channel is a dword key run, then a
   sorted list of frame numbers (bytes or words) at `*_frames_offset` and the
   keys beside them at `*_keys_offset`. The rotation runs sit at the end of
   the header; the translation and scale runs at their offsets. */
struct s_animation_keyframe_table
{
	byte codec;
	byte unknown01[0xb];
	long translation_runs_offset;
	long scale_runs_offset;
	long rotation_frames_offset;
	long translation_frames_offset;
	long scale_frames_offset;
	long rotation_keys_offset;
	long translation_keys_offset;
	long scale_keys_offset;
	long unknown2c;
	dword rotation_runs[1];
};
typedef char assert_keyframe_table_translation_runs[(offsetof(s_animation_keyframe_table, translation_runs_offset) == 0xc) ? 1 : -1];
typedef char assert_keyframe_table_rotation_keys[(offsetof(s_animation_keyframe_table, rotation_keys_offset) == 0x20) ? 1 : -1];
typedef char assert_keyframe_table_scale_keys[(offsetof(s_animation_keyframe_table, scale_keys_offset) == 0x28) ? 1 : -1];
typedef char assert_keyframe_table_rotation_runs[(offsetof(s_animation_keyframe_table, rotation_runs) == 0x30) ? 1 : -1];

/* a key run (dword): the channel's key count in the low 12 bits, the index
   of its first key (and frame number) above them */
enum
{
	k_animation_key_run_count_bits = 12,
	k_animation_key_run_count_mask = (1 << k_animation_key_run_count_bits) - 1
};

#if H2_NATIVE_PORT
/* Native equivalents of the retail MMX/SSE sequences below: widen the four
   signed words, blend, and scale by the reciprocal square root of the dot
   product (computed exactly rather than with rsqrtps' estimate). */
static __forceinline void h2_quantized_rotation_store(real const *value, void *result)
{
	real dot = value[0] * value[0] + value[1] * value[1] + value[2] * value[2] + value[3] * value[3];
	real scale = 1.0f / __builtin_sqrtf(dot);
	real *out = (real *)result;
	out[0] = value[0] * scale;
	out[1] = value[1] * scale;
	out[2] = value[2] * scale;
	out[3] = value[3] * scale;
}
static __forceinline void h2_quantized_rotation_blend(void const *a, void const *b, real t0, real t1, void *result)
{
	short const *qa = (short const *)a;
	short const *qb = (short const *)b;
	real value[4];
	for (long i = 0; i < 4; i++)
		value[i] = t0 * (real)qa[i] + t1 * (real)qb[i];
	h2_quantized_rotation_store(value, result);
}
static __forceinline void h2_quantized_rotation_load(void const *a, void *result)
{
	short const *qa = (short const *)a;
	real value[4] = { (real)qa[0], (real)qa[1], (real)qa[2], (real)qa[3] };
	h2_quantized_rotation_store(value, result);
}
#define QUANTIZED_ROTATION_BLEND(a, b, t0, t1, result) h2_quantized_rotation_blend((a), (b), (t0), (t1), (result))
#define QUANTIZED_ROTATION_LOAD(a, result) h2_quantized_rotation_load((a), (result))
#define RECIPROCAL(r, x) ((r) = 1.0f / (x))
#else
/* result = normalize(t0 * a + t1 * b), a and b quantized quaternions
   (s_quantized_quaternion *), t0 and t1 reals, result a quaternionf * */
#define QUANTIZED_ROTATION_BLEND(a, b, t0, t1, result) 	__asm mov ecx, a 	__asm mov edx, b 	__asm mov eax, result 	__asm movq mm3, [ecx] 	__asm punpcklwd mm1, mm3 	__asm punpckhwd mm2, mm3 	__asm psrad mm1, 0x10 	__asm psrad mm2, 0x10 	__asm cvtpi2ps xmm1, mm1 	__asm cvtpi2ps xmm2, mm2 	__asm movq mm3, [edx] 	__asm punpcklwd mm1, mm3 	__asm punpckhwd mm2, mm3 	__asm psrad mm1, 0x10 	__asm psrad mm2, 0x10 	__asm movss xmm5, t0 	__asm movss xmm6, t1 	__asm shufps xmm5, xmm5, 0 	__asm shufps xmm6, xmm6, 0 	__asm cvtpi2ps xmm3, mm1 	__asm cvtpi2ps xmm4, mm2 	__asm movlhps xmm1, xmm2 	__asm movlhps xmm3, xmm4 	__asm mulps xmm1, xmm5 	__asm mulps xmm3, xmm6 	__asm addps xmm1, xmm3 	__asm emms 	__asm movaps xmm0, xmm1 	__asm mulps xmm0, xmm1 	__asm movaps xmm3, xmm0 	__asm shufps xmm3, xmm3, 0x4e 	__asm addps xmm0, xmm3 	__asm movaps xmm4, xmm0 	__asm shufps xmm4, xmm4, 0x11 	__asm addps xmm0, xmm4 	__asm rsqrtps xmm0, xmm0 	__asm mov eax, result 	__asm mulps xmm1, xmm0 	__asm movaps [eax], xmm1

/* result = normalize(a), a quantized quaternion */
#define QUANTIZED_ROTATION_LOAD(a, result) 	__asm mov ecx, a 	__asm mov eax, result 	__asm movq mm3, [ecx] 	__asm punpcklwd mm1, mm3 	__asm punpckhwd mm2, mm3 	__asm psrad mm1, 0x10 	__asm psrad mm2, 0x10 	__asm cvtpi2ps xmm1, mm1 	__asm cvtpi2ps xmm2, mm2 	__asm emms 	__asm movlhps xmm1, xmm2 	__asm movaps xmm0, xmm1 	__asm mulps xmm0, xmm1 	__asm movaps xmm3, xmm0 	__asm shufps xmm3, xmm3, 0x4e 	__asm addps xmm0, xmm3 	__asm movaps xmm4, xmm0 	__asm shufps xmm4, xmm4, 0x11 	__asm addps xmm0, xmm4 	__asm rsqrtps xmm0, xmm0 	__asm mulps xmm1, xmm0 	__asm movaps [eax], xmm1

/* r = an approximate 1 / x (rcpss), both reals */
#define RECIPROCAL(r, x) 	__asm rcpss xmm0, x 	__asm movss r, xmm0
#endif

/* *result = current + (next - current) * (reciprocal * offset): a scale key
   blended toward the next one */
__forceinline void animation_scale_interpolate(real current, real next, real reciprocal, real offset, real *result)
{
	*result = (next - current) * (reciprocal * offset) + current;
}

#endif
