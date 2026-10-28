// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_10E480.CPP: start an object's vibration from a set of curves.
   Decompiled by lane F for 0x18ca20; its callee 0x10de40 is in
   unknown_10dc70.cpp, whose file this function probably shares. */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_11a4d0.h"

struct s_object_list;
extern s_object_list *g_4de2f4;

struct s_vibration_curve_set;
struct s_vibration_output;
void function_10de40(s_vibration_curve_set *set, s_vibration_output *output, real volatile time, real blend, bool skip_special);

struct s_vibration_object_view
{
	long definition_index;
	byte unknown04[0x33e - 4];
	short vibration_offset;
};

struct s_vibration_object_header
{
	byte unknown00[8];
	s_vibration_object_view *object;
};

struct s_vibration_view
{
	byte unknown00[0x30];
	byte output[6];
	short index;
	long tag_index;
};

struct s_vibration_definition_view
{
	byte unknown00[0x1e4];
	real blend;
};

struct s_vibration_globals_view
{
	byte unknown00[0x81];
	bool enabled;
};

// @retail 0x10e480
void function_10e480(long object_index, long tag_index, s_vibration_curve_set *curves, real time)
{
	s_vibration_object_view *object = ((s_vibration_object_header *)g_4e0300->data)[object_index & 0xffff].object;
	s_vibration_view *vibration = (s_vibration_view *)((byte *)object + object->vibration_offset);
	s_vibration_definition_view *definition = (s_vibration_definition_view *)g_4e3b44[object->definition_index & 0xffff].bytes;
	real blend = 0.95f;

	if (definition->blend > 0.0f)
	{
		blend = definition->blend;
	}
	vibration->tag_index = tag_index;
	if (vibration->index != NONE)
	{
		bool skip_special = false;

		if (((s_vibration_globals_view *)g_4de2f4)->enabled && function_11b930(object_index))
		{
			skip_special = true;
		}
		function_10de40(curves, (s_vibration_output *)vibration->output, time, blend, skip_special);
	}
}

#include "unknown_1cafc0.h"

struct s_vibration_sample_view
{
	byte unknown00[2];
	byte countdown;
	byte unknown03[0x30 - 3];
	c_type_709360 base_animation;
	c_type_709360 weighted_animation;
	byte unknown38[8];
	real frame;
	char frames[12];
	char signed_weights[12];
	c_type_709360 animation_id;
	byte unknown60[0x6e - 0x60];
	byte weights[13];
};

extern real g_54764c;
struct s_anim_data;
void function_20adf0(s_anim_data *data, byte *dest);

// @retail 0x10ec00
void function_10ec00(long object_index, s_animation_state *state, long unused, dword const *node_mask,
	long node_count, real_quaternion_transform *transforms, byte *dest)
{
	long const &unused_reference = unused;
	(void)&unused_reference;
	s_vibration_object_view *object = ((s_vibration_object_header *)g_4e0300->data)[object_index & 0xffff].object;
	s_vibration_sample_view *vibration = (s_vibration_sample_view *)((byte *)object + object->vibration_offset);
	c_type_709360 animation_id = vibration->animation_id;
	if (animation_id.index != NONE)
	{
		c_animation_channel channel;
		if (state->channel_play(&channel, animation_id, 0x40))
		{
			s_animation *animation = NULL;
			if (channel.animation_id.index != NONE)
				animation = function_1daea0(graph_tag_get(channel.graph_tag_index), channel.animation_id);
			bool sampled = false;
			for (long i = 0; i < 13; i++)
			{
				real frame = i * 2.0f + vibration->frame;
				if (frame >= 0.0f && frame < animation->frame_count && vibration->weights[i])
				{
					real weight = vibration->weights[i] * g_54764c;
					channel.set_frame_position(frame);
					channel.sample(weight, node_mask, node_count, transforms);
					sampled = true;
				}
			}
			if (sampled)
			{
				struct s_output_10ec00 : s_animation_data
				{
					__forceinline void write_10ec00(byte *out)
					{
						function_20adf0((s_anim_data *)this, out);
					}
				};
				s_output_10ec00 data;
				function_1ddb40(&data, graph_tag_get(channel.graph_tag_index), channel.animation_id);
				data.write_10ec00(dest);
			}
		}
	}
}

#include <math.h>
extern real g_547648;
void c_animation_channel_data_get(c_animation_channel const *channel, s_animation_data *data);

// score 215 (was 221): Frame 0x30 versus 0x2c, destination/ordinal homes and getter expansion remain.
// Lane 8 r64: countdown weight with the `>= 3` case first (retail's fall-through) 215, kept; frame
// byte in a long local 260 (32-bit compare as retail, aligned 119, 969 bytes); char/word loop index
// 215 (identical: the loop stays a pointer plus spilled countdown); sampled flag declared before
// the getter result 285 (994 bytes); weight as `frame * (weight byte * scale)` 215. Diagnostic only: 0x1c6440
// declared noinline gives 940 bytes and 166 (with the long frame 215, aligned 94), but matched
// callers such as 0x1c6500, 0x1c6560 and 0x1ccb40 inline it, so the header was restored.
// and spilled induction remain; native-byte sample frame 221; short sample frame 261;
// native-byte sample frame 221; short sample frame 261;
// one tag frame count 255; input cursors 273; signed-byte sampled result 221;
// signed-byte ordinal 221; native frame/count 255; cursors/sampled result 273.
// Earlier: destination and induction homes plus getter expansion remain;
// successful-sample count 274; equality bound 221; frame cursor 283;
// weight cursor 284; end-controlled input cursors 265; frame guard before weight 266;
// weight-magnitude first 266; actual animation reference 221.
// Round 230: Read-only sample/tag/both views 221/221/221; reused weight scale 284;
// scale with either/both views 284/284/284; full input views with named conversion product 284.
// Round 164: Base identifier confined, separate base admission, both, separate
// weighted admission and its three combinations 221 each; complete countdown
// admission split 221. Original playback graph and all constructor points restored.
// Earlier at 221 (from 221): destination and induction homes plus getter expansion remain;
// Positive/countdown/touch order 289; touch/positive/countdown 276;
// touch/countdown/positive 304; countdown/positive/touch 341; countdown/touch/positive
// 351; positive/touch, touch/positive and countdown/positive guard exits 284/276/341.
// Negative no-samples bool/byte/short/long in weighted scope 221/221/222/221;
// across the positive playback phase 313/313/328/331. Original sampled flag and constructors restored.
// Round 150: Data producer/exporter inline contracts with direct/shared data acquisition
// 221/221/221/457/221/457; explicit out-of-line provider pair 221, with shared acquisition 221. Native
// long/dword/short/byte sample ordinal initialized before channel setup or at function entry 221 each.
// Complete original body and provider declarations restored; frame and getter expansion remain.
// Round 234: Last successful sample ordinal in long/short/signed-byte/dword
// at the original result lifetime 268/277/316/268; initialized before the getter
// 296/302/323/296. Frame and destination/ordinal allocation remain. Original
// Boolean result, all channel/data constructors and executable body restored.
// Scoped output names in seven combinations 221 each; with separate channel names 221.
// Common final publication bool/byte/short/long 301/301/300/299;
// with touch admission before construction 343/343/346/341. Original restored.
// Lane 8 r58: weighted id read into the animation id local / its own local 301/301; frame
// byte in a long local for the compare and the conversion 260; both 280; channel data getter
// call in the first two arms 221 (identical code, kept); with id local and long frame 280.
// The loop's pointer/countdown spill is the extra frame dword; retail indexes by i.
// Native unsigned-long/word induction with a full-width frame before/after weight 260 each;
// signed/unsigned 64-bit induction with that frame 256/249 at either boundary, recovering frame 0x2c.
// Signed 64-bit direct/char/short frames 295/295/256; unsigned 288/288/244; unsigned direct frame
// with byte/short sampled result 288/292. Extra loop length regresses; all restored.
// Round 60: ours inlines the weighted arm's animation getter call (retail calls it) and tail-merges
// the second arm's data/publish calls into the first arm's (retail keeps both). Weight product
// reordered, short and int index 221 each (identical); weighted id copied into the id local or its
// own local 301/301 (the tail un-merges as retail but the state pointer leaves EBP).
// @retail 0x10e530
void function_10e530(long object_index, s_animation_state *state, long unused, dword const *node_mask,
	long node_count, real_quaternion_transform *transforms, byte *dest)
{
	long const &unused_reference = unused;
	(void)&unused_reference;
	s_vibration_object_view *object = ((s_vibration_object_header *)g_4e0300->data)[object_index & 0xffff].object;
	s_vibration_sample_view *vibration = (s_vibration_sample_view *)((byte *)object + object->vibration_offset);
	real frame = vibration->frame;
	if (frame > 0.0001f)
	{
		c_animation_channel channel;
		c_type_709360 animation_id = vibration->base_animation;
		if (animation_id.index != NONE && state->channel_play(&channel, animation_id, 0))
		{
			channel.set_frame_position(0.0f);
			channel.sample(vibration->frame, node_mask, node_count, transforms);
			s_animation_data data;
			c_animation_channel_data_get(&channel, &data);
			function_20adf0((s_anim_data *)&data, dest);
		}
		if (vibration->weighted_animation.index != NONE && state->channel_play(&channel, vibration->weighted_animation, 0))
		{
			s_animation *animation = channel.function_1c6440();
			bool sampled = false;
			for (long i = 0; i < 12; i++)
			{
				real weight = vibration->signed_weights[i] * g_547648 * vibration->frame;
				if (vibration->frames[i] < animation->frame_count && !(fabs(weight) < 0.0001f))
				{
					channel.set_frame_position((real)vibration->frames[i]);
					channel.sample(weight, node_mask, node_count, transforms);
					sampled = true;
				}
			}
			if (sampled)
			{
				s_animation_data data;
				c_animation_channel_data_get(&channel, &data);
				function_20adf0((s_anim_data *)&data, dest);
			}
		}
	}
	else if (frame != 0.0f)
	{
		if (vibration->base_animation.index != NONE)
			state->animation_touch(vibration->base_animation);
		if (vibration->weighted_animation.index != NONE)
			state->animation_touch(vibration->weighted_animation);
	}
	else if (vibration->weighted_animation.index != NONE && vibration->countdown > 0 && vibration->countdown < 7)
	{
		c_animation_channel channel;
		if (state->channel_play(&channel, vibration->weighted_animation, 0) && channel.function_1c6440()->frame_count > 17)
		{
			real weight = vibration->countdown >= 3 ? (7 - vibration->countdown) * 0.25f : vibration->countdown * (1.0f / 3.0f);
			channel.set_frame_position(17.0f);
			channel.sample(weight, node_mask, node_count, transforms);
			s_animation_data data;
			c_animation_channel_data_get(&channel, &data);
			function_20adf0((s_anim_data *)&data, dest);
		}
	}
}
