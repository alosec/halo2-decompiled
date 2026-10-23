/* ANIMATION_SAMPLING.H: the sampling state the samplers
   (src/unknown_279d80.cpp) and the codecs' channel decoders share */

#ifndef ANIMATION_SAMPLING_H
#define ANIMATION_SAMPLING_H

#include "unknown_11c920.h"

/* the channel decoders of a codec: rotation, translation and scale */
struct s_animation_samplers
{
	void (*rotation)(void);
	void (*translation)(void);
	void (*scale)(void);
};

struct s_animation;
struct s_graph_inheritance;
struct s_animation_data;
struct real_quaternion_transform;

/* the sampling state (0x64 bytes at 0x504450): function_2798a0 fills it, and
   the samplers and the codecs' decoders read it */
struct s_animation_sampling_settings
{
	long blend_method;
	long node_kind;
	bool interpolated_decompressors;
	long node_count;
	long destination_node_count;
	long frame_index;
	long next_frame_index;
	real frame_fraction;
	real blend_weight;
	s_animation *animation;
	s_graph_inheritance *inheritance;
	dword const *destination_node_mask;
	s_animation_data *field_30;
	s_animation_samplers decompressors;
	byte *rotation_bit_flags;
	byte *translation_bit_flags;
	byte *scale_bit_flags;
	real_quaternion_transform *field_4c;
	bool blend_frames;
	long blend_frame_index;
	long blend_next_frame_index;
	real blend_frame_fraction;
	real blend_fraction;
};

extern s_animation_sampling_settings g_sampling_settings;

#endif
