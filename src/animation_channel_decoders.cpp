#include "unknown_11c920.h"
#include <xmmintrin.h>
#include "globals.h"
#include "unknown_xd56787.h"
#include "animation_channel_decoders.h"

// @flags /O2 /Gr /arch:SSE

/* ANIMATION_CHANNEL_DECODERS.CPP: the channel decoders of the per-frame animation
   codecs (g_47fb18): each one reads the rotation, translation or scale of
   the current node (node indices g_5044b4, g_5044b8 and g_5044bc) out of
   g_sampling_settings.field_30 and writes it to *g_5044c0. The first decoder
   set of a codec samples g_sampling_settings.frame_index; the second blends
   toward next_frame_index by frame_fraction. The interpolating decoders of
   the byte keyframe codecs live here too; their other decoders are in
   src/animation_keyframe_decoders.cpp. */

/* _uncompressed_static_data_codec: one quantized rotation per node */
// @retail 0x28c470
void animation_codec_static_rotation_sample()
{
	long node_index = g_5044b4;
	s_quantized_quaternion *source = (s_quantized_quaternion *)((s_animation_frame_table *)g_sampling_settings.field_30)->rotations + node_index;
	quaternionf *result = &g_5044c0->rotation;

	QUANTIZED_ROTATION_LOAD(source, result);
}

PRIVATE __forceinline void animation_static_translation_copy(long node_index, s_animation_frame_table *data, s_animation_output *output)
{
	long offset = node_index * 12;
	offset += data->translation_offset;
	output->vector = *(vector3f *)((byte *)data + offset);
}

/* _uncompressed_static_data_codec: one translation per node */
// @retail 0x28c4e0
void animation_codec_static_translation_sample()
{
	s_animation_output *destination = g_5044c0;
	animation_static_translation_copy(g_5044b8, (s_animation_frame_table *)g_sampling_settings.field_30, destination);
}

/* _uncompressed_static_data_codec: one scale per node */
// @retail 0x28c510
void animation_codec_static_scale_sample()
{
	s_animation_frame_table *data = (s_animation_frame_table *)g_sampling_settings.field_30;

	g_5044c0->scale = *(real *)((byte *)data + data->scale_offset + g_5044bc * 4);
}

/* _blend_screen_codec: blends the frame and next-frame rotations (full
   quaternions) of the node and normalizes the result */
// @retail 0x28c530
void animation_codec_blend_screen_rotation_interpolate()
{
	s_animation_frame_table *data = (s_animation_frame_table *)g_sampling_settings.field_30;
	quaternionf *rotations = (quaternionf *)(data->rotations + data->rotation_stride * g_5044b4);
	quaternionf *result = &g_5044c0->rotation;
	real t = g_sampling_settings.frame_fraction;
	quaternionf *b = rotations + g_sampling_settings.next_frame_index;
	quaternionf *a = rotations + g_sampling_settings.frame_index;

	__asm
	{
		mov eax, a
		mov ecx, b
		movss xmm3, t
		movaps xmm1, [eax]
		movaps xmm2, [ecx]
		shufps xmm3, xmm3, 0
		subps xmm2, xmm1
		mulps xmm2, xmm3
		addps xmm2, xmm1
		movaps xmm0, xmm2
		mulps xmm0, xmm2
		movaps xmm7, xmm0
		shufps xmm7, xmm7, 0x4e
		addps xmm0, xmm7
		movaps xmm6, xmm0
		shufps xmm6, xmm6, 0x11
		addps xmm0, xmm6
		rsqrtps xmm0, xmm0
		mov edx, result
		mulps xmm2, xmm0
		movaps [edx], xmm2
	}
}

/* *result = a + (b - a) * t */
PRIVATE __forceinline void animation_vector_interpolate(vector3f const *a, vector3f const *b, real t, vector3f *result)
{
	result->i = (b->i - a->i) * t + a->i;
	result->j = (b->j - a->j) * t + a->j;
	result->k = (b->k - a->k) * t + a->k;
}

/* the per-frame translations of a node */
PRIVATE __forceinline vector3f *animation_frame_table_translations(long node_index, s_animation_frame_table *data)
{
	long stride = data->translation_stride;
	long offset = data->translation_offset;
	return (vector3f *)(((byte *)data + stride * node_index) + offset);
}

/* _blend_screen_codec: blends the frame and next-frame translations of the
   node */
// @retail 0x28c5d0
void animation_codec_blend_screen_translation_interpolate()
{
	static vector3f vector_a;
	static vector3f vector_b;
	s_animation_frame_table *data = (s_animation_frame_table *)g_sampling_settings.field_30;
	vector3f *translations = animation_frame_table_translations(g_5044b8, data);
	real t = g_sampling_settings.frame_fraction;

	vector_a = translations[g_sampling_settings.frame_index];
	vector_b = translations[g_sampling_settings.next_frame_index];
	animation_vector_interpolate(&vector_a, &vector_b, t, &g_5044c0->vector);
}

/* _8byte_quantized_rotation_only_codec and _blend_screen_codec: blends the
   frame and next-frame scales of the node */
// @retail 0x28c6b0
void animation_codec_scale_interpolate()
{
	s_animation_frame_table *data = (s_animation_frame_table *)g_sampling_settings.field_30;
	dword *scales = (dword *)((byte *)data + data->scale_stride * g_5044bc + data->scale_offset);
	dword bits_a = scales[g_sampling_settings.frame_index];
	dword bits_b = scales[g_sampling_settings.next_frame_index];
	real a = *(real *)&bits_a;
	real b = *(real *)&bits_b;

	g_5044c0->scale = (b - a) * g_sampling_settings.frame_fraction + a;
}

/* _blend_screen_codec: copies the rotation of the node at the frame */
// @retail 0x28c710
void animation_codec_blend_screen_rotation_sample()
{
	s_animation_frame_table *data = (s_animation_frame_table *)g_sampling_settings.field_30;
	quaternionf *rotations = (quaternionf *)(data->rotations + data->rotation_stride * g_5044b4);
	quaternionf *source = rotations + g_sampling_settings.frame_index;
	quaternionf *destination = &g_5044c0->rotation;

	*destination = *source;
}

/* _8byte_quantized_rotation_only_codec and _blend_screen_codec: copies the
   translation of the node at the frame */
// @retail 0x28c750
void animation_codec_translation_sample()
{
	s_animation_frame_table *data = (s_animation_frame_table *)g_sampling_settings.field_30;
	vector3f *source = (vector3f *)((byte *)data + (data->translation_stride * g_5044b8 + g_sampling_settings.frame_index * 12 + data->translation_offset));
	vector3f *destination = &g_5044c0->vector;

	*destination = *source;
}

/* _8byte_quantized_rotation_only_codec: blends the frame and next-frame
   quantized rotations of the node and normalizes the result */
// @retail 0x28c790
void animation_codec_quantized_rotation_interpolate()
{
	s_animation_frame_table *data = (s_animation_frame_table *)g_sampling_settings.field_30;
	s_quantized_quaternion *rotations = (s_quantized_quaternion *)(data->rotations + data->rotation_stride * g_5044b4);
	real inverse_t = 1.0f - g_sampling_settings.frame_fraction;
	real t = g_sampling_settings.frame_fraction;
	quaternionf *result = &g_5044c0->rotation;
	s_quantized_quaternion *b = rotations + g_sampling_settings.next_frame_index;
	s_quantized_quaternion *a = rotations + g_sampling_settings.frame_index;

	QUANTIZED_ROTATION_BLEND(a, b, inverse_t, t, result);
}

/* _8byte_quantized_rotation_only_codec: blends the frame and next-frame
   translations of the node */
// @retail 0x28c880
void animation_codec_quantized_translation_interpolate()
{
	static vector3f vector_a;
	static vector3f vector_b;
	s_animation_frame_table *data = (s_animation_frame_table *)g_sampling_settings.field_30;
	vector3f *translations = animation_frame_table_translations(g_5044b8, data);
	real t = g_sampling_settings.frame_fraction;

	vector_a = translations[g_sampling_settings.frame_index];
	vector_b = translations[g_sampling_settings.next_frame_index];
	animation_vector_interpolate(&vector_a, &vector_b, t, &g_5044c0->vector);
}

/* _8byte_quantized_rotation_only_codec: the quantized rotation of the node
   at the frame, normalized */
// @retail 0x28c960
void animation_codec_quantized_rotation_sample()
{
	s_animation_frame_table *data = (s_animation_frame_table *)g_sampling_settings.field_30;
	long node_offset = data->rotation_stride * g_5044b4;
	quaternionf *result = &g_5044c0->rotation;
	s_quantized_quaternion *source = (s_quantized_quaternion *)(data->rotations + node_offset) + g_sampling_settings.frame_index;

	QUANTIZED_ROTATION_LOAD(source, result);
}

/* byte keyframe codecs: finds the last rotation key of the node at or
   before the frame and blends it toward the next key by the exact
   (fractional) frame */
// @retail 0x28c9e0
void animation_codec_byte_keyframe_rotation_interpolate()
{
	s_animation_keyframe_table *data = (s_animation_keyframe_table *)g_sampling_settings.field_30;
	dword run = data->rotation_runs[g_5044b4];
	long start = run >> k_animation_key_run_count_bits;
	long count = run & k_animation_key_run_count_mask;
	byte *frames = (byte *)data + data->rotation_frames_offset + start;
	s_quantized_quaternion *keys = (s_quantized_quaternion *)((byte *)data + data->rotation_keys_offset + start * 8);
	long low = 0;
	long high = count;
	quaternionf *result;

	while (high > low + 1)
	{
		long middle = (low + high) >> 1;

		if ((dword)frames[middle] <= (dword)g_sampling_settings.frame_index)
		{
			low = middle;
		}
		else
		{
			high = middle;
		}
	}

	_mm_prefetch((char *)(keys + low), _MM_HINT_T0);
	result = &g_5044c0->rotation;
	frames += low;

	if (low + 1 < count)
	{
		real frame = (real)(long)g_sampling_settings.frame_index + g_sampling_settings.frame_fraction;
		real weight_a = frames[1] - frame;
		real weight_b = frame - frames[0];
		s_quantized_quaternion *b = keys + (low + 1);
		s_quantized_quaternion *a = keys + low;

		QUANTIZED_ROTATION_BLEND(a, b, weight_a, weight_b, result);
	}
	else
	{
		s_quantized_quaternion *a = keys + low;

		QUANTIZED_ROTATION_LOAD(a, result);
	}
}

/* *result += (*next - *result) * t */
PRIVATE __forceinline void animation_vector_interpolate_toward(vector3f const *next, real t, vector3f *result)
{
	vector3f target = *next;
	result->i = (target.i - result->i) * t + result->i;
	result->j = (target.j - result->j) * t + result->j;
	result->k = (target.k - result->k) * t + result->k;
}

/* byte keyframe codecs: the last translation key of the node at or before
   the frame, blended toward the next key by the exact (fractional) frame */
// @retail 0x28cb70
void animation_codec_byte_keyframe_translation_interpolate()
{
	s_animation_keyframe_table *data = (s_animation_keyframe_table *)g_sampling_settings.field_30;
	dword run = *(dword *)((byte *)data + (data->translation_runs_offset + g_5044b8 * 4));
	long start = run >> k_animation_key_run_count_bits;
	long count = run & k_animation_key_run_count_mask;
	byte *frames = (byte *)data + data->translation_frames_offset + start;
	vector3f *keys = (vector3f *)((byte *)data + data->translation_keys_offset + start * 12);
	long low = 0;
	long high = count;
	vector3f *key;
	vector3f *destination;

	while (high > low + 1)
	{
		long middle = (low + high) >> 1;

		if ((dword)frames[middle] <= (dword)g_sampling_settings.frame_index)
		{
			low = middle;
		}
		else
		{
			high = middle;
		}
	}

	_mm_prefetch((char *)(keys + low), _MM_HINT_T0);
	key = keys + low;
	destination = &g_5044c0->vector;
	*destination = *key;
	frames += low;

	if (low + 1 < count)
	{
		long key_frame = frames[0];
		long next_key_frame = frames[1];
		real offset = (real)(long)g_sampling_settings.frame_index + g_sampling_settings.frame_fraction;
		offset -= (real)key_frame;
		real difference = (real)(next_key_frame - key_frame);
		real reciprocal;
		real weight;

		RECIPROCAL(reciprocal, difference);

		weight = reciprocal * offset;
		animation_vector_interpolate_toward(key + 1, weight, destination);
	}
}

/* byte keyframe codecs: the last scale key of the node at or before the
   frame, blended toward the next key by the exact (fractional) frame */
// @retail 0x28ccc0
void animation_codec_byte_keyframe_scale_interpolate()
{
	s_animation_keyframe_table *data = (s_animation_keyframe_table *)g_sampling_settings.field_30;
	dword run = *(dword *)((byte *)data + (data->scale_runs_offset + g_5044bc * 4));
	long start = run >> k_animation_key_run_count_bits;
	long count = run & k_animation_key_run_count_mask;
	byte *frames = (byte *)data + data->scale_frames_offset + start;
	dword *keys = (dword *)((byte *)data + data->scale_keys_offset + start * 4);
	long low = 0;
	long high = count;
	dword *key;
	s_animation_output *result;

	while (high > low + 1)
	{
		long middle = (low + high) >> 1;

		if ((dword)frames[middle] <= (dword)g_sampling_settings.frame_index)
		{
			low = middle;
		}
		else
		{
			high = middle;
		}
	}

	_mm_prefetch((char *)(keys + low), _MM_HINT_T0);
	key = keys + low;
	result = g_5044c0;
	*(dword *)&result->scale = *key;
	frames += low;

	if (low + 1 < count)
	{
		long key_frame = frames[0];
		long next_key_frame = frames[1];
		real offset = (real)(long)g_sampling_settings.frame_index + g_sampling_settings.frame_fraction;
		offset -= (real)key_frame;
		real difference = (real)(next_key_frame - key_frame);
		real reciprocal;
		real weight;

		RECIPROCAL(reciprocal, difference);

		weight = reciprocal * offset;
		dword next_bits = key[1];
		real next = *(real *)&next_bits;

		animation_scale_interpolate(result->scale, next, reciprocal, offset, &result->scale);
	}
}
