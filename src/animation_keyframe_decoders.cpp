#include "unknown_11c920.h"
#include <xmmintrin.h>
#include "globals.h"
#include "unknown_xd56787.h"
#include "animation_channel_decoders.h"
// @flags /O2 /arch:SSE /Gr

/* ANIMATION_KEYFRAME_DECODERS.CPP: the channel decoders of the keyframe
   animation codecs (byte_keyframe_lightly_quantized, word_keyframe_lightly_
   quantized and their reverse forms). Each channel of a node keeps a sorted
   run of frame numbers (bytes or words) with its keys beside them; the
   decoder finds the last key at or before the frame by binary search and
   blends it toward the next key. The byte codecs decode the interpolated
   form in src/animation_channel_decoders.cpp. */

// @retail 0x28d170
char __stdcall function_28d170(
	long a,
	long b,
	long c,
	long d)
{
	return 0;
}

typedef char (__stdcall *t_function_28d170)(long, long, long, long);
t_function_28d170 g_function_28d170 = function_28d170;

/* byte keyframe codecs: the rotation of the node at the whole frame,
   blended between the keys around it */
// @retail 0x28cdb0
void animation_codec_byte_keyframe_rotation_sample()
{
	s_animation_keyframe_table *data = (s_animation_keyframe_table *)g_sampling_settings.field_30;
	dword run = data->rotation_runs[g_5044b4];
	dword start = run >> k_animation_key_run_count_bits;
	long count = run & k_animation_key_run_count_mask;
	byte *frames = (byte *)data + (data->rotation_frames_offset + start);
	s_quantized_quaternion *keys = (s_quantized_quaternion *)((byte *)data + data->rotation_keys_offset) + start;
	long low = 0;
	long high = count;
	while (high > low + 1)
	{
		long middle = (high + low) >> 1;
		if (frames[middle] <= (dword)g_sampling_settings.frame_index)
		{
			low = middle;
		}
		else
		{
			high = middle;
		}
	}
	frames += low;
	dword frame = *frames;
	_mm_prefetch((char const *)(keys + low), _MM_HINT_T0);
	if (g_sampling_settings.frame_index != frame)
	{
		quaternionf *result = &g_5044c0->rotation;
		real weight_a = (real)(long)(frames[1] - g_sampling_settings.frame_index);
		real weight_b = (real)(long)(g_sampling_settings.frame_index - frame);
		s_quantized_quaternion *b = keys + low + 1;
		s_quantized_quaternion *a = keys + low;
		QUANTIZED_ROTATION_BLEND(a, b, weight_a, weight_b, result);
	}
	else
	{
		quaternionf *result = &g_5044c0->rotation;
		s_quantized_quaternion *a = keys + low;
		QUANTIZED_ROTATION_LOAD(a, result);
	}
}

/* byte keyframe codecs: the translation of the node at the whole frame,
   blended between the keys around it */
// @retail 0x28cf40
void animation_codec_byte_keyframe_translation_sample()
{
	long node_index = g_5044b8;
	s_animation_keyframe_table *data = (s_animation_keyframe_table *)g_sampling_settings.field_30;
	dword run = *(dword *)((byte *)data + data->translation_runs_offset + node_index * 4);
	long start = run >> k_animation_key_run_count_bits;
	long count = run & k_animation_key_run_count_mask;
	byte *frames = (byte *)data + data->translation_frames_offset + start;
	vector3f *keys = (vector3f *)((byte *)data + data->translation_keys_offset) + start;
	long low = 0;
	long high = count;
	while (high > low + 1)
	{
		long middle = (high + low) >> 1;
		if (frames[middle] <= (dword)g_sampling_settings.frame_index)
		{
			low = middle;
		}
		else
		{
			high = middle;
		}
	}
	frames += low;
	_mm_prefetch((char const *)(keys + low), _MM_HINT_T0);
	vector3f *key = keys + low;
	vector3f *translation = &g_5044c0->vector;
	*translation = *key;
	if (low + 1 < count)
	{
		dword frame = *frames;
		if (g_sampling_settings.frame_index != frame)
		{
			real offset = (real)(long)g_sampling_settings.frame_index - (real)(long)frame;
			real denominator = (real)(long)(frames[1] - frame);
			real reciprocal;
			RECIPROCAL(reciprocal, denominator);
			real t = reciprocal * offset;
			vector3f next = key[1];
			translation->i += (next.i - translation->i) * t;
			translation->j += (next.j - translation->j) * t;
			translation->k += (next.k - translation->k) * t;
		}
	}
}

#pragma optimize("p", on)
/* byte keyframe codecs: the scale of the node at the whole frame, blended
   between the keys around it */
// @retail 0x28d090
void animation_codec_byte_keyframe_scale_sample()
{
	s_animation_keyframe_table *data = (s_animation_keyframe_table *)g_sampling_settings.field_30;
	dword run = *(dword *)((byte *)data + data->scale_runs_offset + g_5044bc * 4);
	long start = run >> k_animation_key_run_count_bits;
	long count = run & k_animation_key_run_count_mask;
	byte *frames = (byte *)data + data->scale_frames_offset + start;
	dword *keys = (dword *)((byte *)data + data->scale_keys_offset) + start;
	long low = 0;
	long high = count;
	while (high > low + 1)
	{
		long middle = (high + low) >> 1;
		if (frames[middle] <= (dword)g_sampling_settings.frame_index)
		{
			low = middle;
		}
		else
		{
			high = middle;
		}
	}
	s_animation_output *result = g_5044c0;
	frames += low;
	_mm_prefetch((char const *)(keys + low), _MM_HINT_T0);
	dword *key = keys + low;
	*(dword *)&result->scale = *key;
	if (low + 1 < count)
	{
		dword frame = *frames;
		long frame_index = g_sampling_settings.frame_index;
		if (frame_index != frame)
		{
			real offset = (real)frame_index - (real)(long)frame;
			real denominator = (real)(long)(frames[1] - frame);
			real reciprocal;
			RECIPROCAL(reciprocal, denominator);
			dword next_bits = key[1];
			animation_scale_interpolate(result->scale, *(real *)&next_bits, reciprocal, offset, &result->scale);
		}
	}
}
#pragma optimize("", on)

/* word keyframe codecs: the last rotation key of the node at or before the
   frame, blended toward the next key by the exact (fractional) frame */
// @retail 0x28d180
void animation_codec_word_keyframe_rotation_interpolate()
{
	s_animation_keyframe_table *data = (s_animation_keyframe_table *)g_sampling_settings.field_30;
	dword run = data->rotation_runs[g_5044b4];
	long start = run >> k_animation_key_run_count_bits;
	word *frames = (word *)((byte *)data + data->rotation_frames_offset) + start;
	long count = run & k_animation_key_run_count_mask;
	s_quantized_quaternion *keys = (s_quantized_quaternion *)((byte *)data + data->rotation_keys_offset) + start;
	long low = 0;
	long high = count;
	while (high > low + 1)
	{
		long middle = (high + low) >> 1;
		if (frames[middle] <= (dword)g_sampling_settings.frame_index)
		{
			low = middle;
		}
		else
		{
			high = middle;
		}
	}
	_mm_prefetch((char const *)(keys + low), _MM_HINT_T0);
	quaternionf *result = &g_5044c0->rotation;
	if (low + 1 < count)
	{
		real frame = (real)(long)g_sampling_settings.frame_index + g_sampling_settings.frame_fraction;
		real weight_a = (real)(long)frames[low + 1] - frame;
		real weight_b = frame - (real)(long)frames[low];
		s_quantized_quaternion *b = keys + low + 1;
		s_quantized_quaternion *a = keys + low;
		QUANTIZED_ROTATION_BLEND(a, b, weight_a, weight_b, result);
	}
	else
	{
		s_quantized_quaternion *a = keys + low;
		QUANTIZED_ROTATION_LOAD(a, result);
	}
}

/* word keyframe codecs: the last translation key of the node at or before
   the frame, blended toward the next key by the exact (fractional) frame */
// @retail 0x28d320
void animation_codec_word_keyframe_translation_interpolate()
{
	s_animation_keyframe_table *data = (s_animation_keyframe_table *)g_sampling_settings.field_30;
	dword run = *(dword *)((byte *)data + data->translation_runs_offset + g_5044b8 * 4);
	dword start = run >> k_animation_key_run_count_bits;
	long count = run & k_animation_key_run_count_mask;
	word *frames = (word *)((byte *)data + data->translation_frames_offset) + start;
	vector3f *keys = (vector3f *)((byte *)data + data->translation_keys_offset) + start;
	long low = 0;
	long high = count;
	while (high > low + 1)
	{
		long middle = (high + low) >> 1;
		if (frames[middle] <= (dword)g_sampling_settings.frame_index)
		{
			low = middle;
		}
		else
		{
			high = middle;
		}
	}
	vector3f *translation = &g_5044c0->vector;
	_mm_prefetch((char const *)(keys + low), _MM_HINT_T0);
	vector3f *key = keys + low;
	*translation = *key;
	if (low + 1 < count)
	{
		dword key_frame = frames[low];
		dword next_key_frame = frames[low + 1];
		real offset = (real)(long)g_sampling_settings.frame_index + g_sampling_settings.frame_fraction;
		offset -= (real)(long)key_frame;
		real denominator = (real)(long)(next_key_frame - key_frame);
		real reciprocal;
		RECIPROCAL(reciprocal, denominator);
		real t = reciprocal * offset;
		vector3f next = key[1];
		translation->i += (next.i - translation->i) * t;
		translation->j += (next.j - translation->j) * t;
		translation->k += (next.k - translation->k) * t;
	}
}

/* word keyframe codecs: the last scale key of the node at or before the
   frame, blended toward the next key by the exact (fractional) frame */
// @retail 0x28d470
void animation_codec_word_keyframe_scale_interpolate()
{
	s_animation_keyframe_table *data = (s_animation_keyframe_table *)g_sampling_settings.field_30;
	dword run = *(dword *)((byte *)data + data->scale_runs_offset + g_5044bc * 4);
	dword start = run >> k_animation_key_run_count_bits;
	long count = run & k_animation_key_run_count_mask;
	word *frames = (word *)((byte *)data + data->scale_frames_offset) + start;
	dword *keys = (dword *)((byte *)data + data->scale_keys_offset) + start;
	long low = 0;
	long high = count;
	while (high > low + 1)
	{
		long middle = (high + low) >> 1;
		if (frames[middle] <= (dword)g_sampling_settings.frame_index)
		{
			low = middle;
		}
		else
		{
			high = middle;
		}
	}
	_mm_prefetch((char const *)(keys + low), _MM_HINT_T0);
	dword *key = keys + low;
	s_animation_output *result = g_5044c0;
	*(dword *)&result->scale = *key;
	if (low + 1 < count)
	{
		dword key_frame = frames[low];
		dword next_key_frame = frames[low + 1];
		real frame = (real)(long)g_sampling_settings.frame_index + g_sampling_settings.frame_fraction;
		real offset = frame - (real)(long)key_frame;
		real denominator = (real)(long)(next_key_frame - key_frame);
		real reciprocal;
		RECIPROCAL(reciprocal, denominator);
		dword next_bits = key[1];
		animation_scale_interpolate(result->scale, *(real *)&next_bits, reciprocal, offset, &result->scale);
	}
}

/* word keyframe codecs: the rotation of the node at the whole frame,
   blended between the keys around it */
// @retail 0x28d560
void animation_codec_word_keyframe_rotation_sample()
{
	s_animation_keyframe_table *data = (s_animation_keyframe_table *)g_sampling_settings.field_30;
	dword run = data->rotation_runs[g_5044b4];
	dword start = run >> k_animation_key_run_count_bits;
	long count = run & k_animation_key_run_count_mask;
	word *frames = (word *)((byte *)data + data->rotation_frames_offset) + start;
	s_quantized_quaternion *keys = (s_quantized_quaternion *)((byte *)data + data->rotation_keys_offset) + start;
	long low = 0;
	long high = count;
	while (high > low + 1)
	{
		long middle = (high + low) >> 1;
		if (frames[middle] <= (dword)g_sampling_settings.frame_index)
		{
			low = middle;
		}
		else
		{
			high = middle;
		}
	}
	dword frame = frames[low];
	_mm_prefetch((char const *)(keys + low), _MM_HINT_T0);
	if (g_sampling_settings.frame_index != frame)
	{
		quaternionf *result = &g_5044c0->rotation;
		real weight_a = (real)(long)(frames[low + 1] - g_sampling_settings.frame_index);
		real weight_b = (real)(long)(g_sampling_settings.frame_index - frame);
		s_quantized_quaternion *b = keys + low + 1;
		s_quantized_quaternion *a = keys + low;
		QUANTIZED_ROTATION_BLEND(a, b, weight_a, weight_b, result);
	}
	else
	{
		quaternionf *result = &g_5044c0->rotation;
		s_quantized_quaternion *a = keys + low;
		QUANTIZED_ROTATION_LOAD(a, result);
	}
}

/* word keyframe codecs: the translation of the node at the whole frame,
   blended between the keys around it */
// @retail 0x28d6f0
void animation_codec_word_keyframe_translation_sample()
{
	s_animation_keyframe_table *data = (s_animation_keyframe_table *)g_sampling_settings.field_30;
	dword run = *(dword *)((byte *)data + data->translation_runs_offset + g_5044b8 * 4);
	dword start = run >> k_animation_key_run_count_bits;
	long count = run & k_animation_key_run_count_mask;
	word *frames = (word *)((byte *)data + data->translation_frames_offset) + start;
	vector3f *keys = (vector3f *)((byte *)data + data->translation_keys_offset) + start;
	long low = 0;
	long high = count;
	while (high > low + 1)
	{
		long middle = (high + low) >> 1;
		if (frames[middle] <= (dword)g_sampling_settings.frame_index)
		{
			low = middle;
		}
		else
		{
			high = middle;
		}
	}
	vector3f *translation = &g_5044c0->vector;
	_mm_prefetch((char const *)(keys + low), _MM_HINT_T0);
	vector3f *key = keys + low;
	*translation = *key;
	if (low + 1 < count)
	{
		dword frame = frames[low];
		if (g_sampling_settings.frame_index != frame)
		{
			real t = (real)(long)g_sampling_settings.frame_index - (real)(long)frame;
			real denominator = (real)(long)(frames[low + 1] - frame);
			real reciprocal;
			RECIPROCAL(reciprocal, denominator);
			t = reciprocal * t;
			vector3f next = key[1];
			translation->i += (next.i - translation->i) * t;
			translation->j += (next.j - translation->j) * t;
			translation->k += (next.k - translation->k) * t;
		}
	}
}

#pragma optimize("p", on)
/* word keyframe codecs: the scale of the node at the whole frame, blended
   between the keys around it */
// @retail 0x28d840
void animation_codec_word_keyframe_scale_sample()
{
	s_animation_keyframe_table *data = (s_animation_keyframe_table *)g_sampling_settings.field_30;
	dword run = *(dword *)((byte *)data + data->scale_runs_offset + g_5044bc * 4);
	dword start = run >> k_animation_key_run_count_bits;
	long count = run & k_animation_key_run_count_mask;
	word *frames = (word *)((byte *)data + data->scale_frames_offset) + start;
	dword *keys = (dword *)((byte *)data + data->scale_keys_offset) + start;
	long low = 0;
	long high = count;
	while (high > low + 1)
	{
		long middle = (high + low) >> 1;
		if (frames[middle] <= (dword)g_sampling_settings.frame_index)
		{
			low = middle;
		}
		else
		{
			high = middle;
		}
	}
	_mm_prefetch((char const *)(keys + low), _MM_HINT_T0);
	dword *key = keys + low;
	real *scale = &g_5044c0->scale;
	*(dword *)scale = *key;
	if (low + 1 < count)
	{
		dword frame = frames[low];
		long frame_index = g_sampling_settings.frame_index;
		if (frame_index != frame)
		{
			real offset = (real)frame_index - (real)(long)frame;
			real denominator = (real)(long)(frames[low + 1] - frame);
			real reciprocal;
			RECIPROCAL(reciprocal, denominator);
			dword next_bits = key[1];
			animation_scale_interpolate(*scale, *(real *)&next_bits, reciprocal, offset, scale);
		}
	}
}
#pragma optimize("", on)
