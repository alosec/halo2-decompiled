// @flags /O2 /Gr
/* ARM-safe selective recovery of the playback effect selector from
   unknown_221da0.cpp.  The broad source also contains unrelated x87 inline
   assembly; keep this exact data-routing boundary independently compilable. */

#include "unknown_11c920.h"
#include "globals.h"

#define PIN(n, floor, ceiling) ((n) < (floor) ? (floor) : ((n) > (ceiling) ? (ceiling) : (n)))

struct s_tag_data_view
{
	long size;
	byte *address;
};

struct s_effect_parameter
{
	long type;
	real default_value;
	real minimum;
	real maximum;
};

struct s_effect_component
{
	long name;
	byte header[0xc];
	long parameter_count;
	s_effect_parameter *parameters;
};

struct s_effect_components
{
	long count;
	s_effect_component *components;
};

struct s_effect_function
{
	short input;
	short range_input;
	s_tag_data_view function;
	real period;
};

struct s_effect_component_overrides
{
	long function_count;
	s_effect_function *functions;
	long constant_count;
	real *constants;
	long source_count;
	char *sources;
	dword parameter_mask;
};

struct s_effect_overrides
{
	long count;
	s_effect_component_overrides *components;
	byte unknown08[8];
	dword component_mask;
};

struct s_effect_inputs
{
	real values[4];
	vector3f direction;
};

struct s_playback_sound_effect
{
	byte unknown00[4];
	long definition_tag_index;
	byte unknown08[0x20 - 8];
	long override_count;
	s_effect_overrides *overrides;
};

struct s_platform_playback_view
{
	byte unknown00[0x14];
	long filter_count;
	void *filters;
	long pitch_lfo_count;
	void *pitch_lfos;
	long filter_lfo_count;
	void *filter_lfos;
	long effect_count;
	s_playback_sound_effect *effects;
};

struct s_sound_effect_tag_view
{
	byte unknown00[0x14];
	long count;
	s_effect_components *components;
};

struct s_sound_effect_buffer
{
	long data[0x100];
	long size;
};

struct s_sound_playback_effects
{
	byte unknown00[8];
	long unknown08;
	long unknown0c;
	void *filter;
	void *pitch_lfo;
	void *filter_lfo;
	s_sound_effect_buffer effect;
};

struct s_sound_class_definition;
void *function_18d090(long tag_index, long handle);
s_sound_class_definition *sound_get_class(long tag_index);
void function_2225a0(s_effect_overrides const *overrides,
	s_effect_components const *components, s_effect_inputs const *inputs,
	long *buffer, long *buffer_size);

static inline void *tag_block_get_first(long count, void *elements)
{
	return PIN(0, 0, count - 1) == 0 ? elements : NULL;
}

static inline bool playback_sound_effect_valid(s_playback_sound_effect const *effect)
{
	return effect && effect->definition_tag_index != NONE && effect->override_count > 0;
}

static inline s_effect_components *sound_effect_tag_get_components(long tag_index)
{
	return ((s_sound_effect_tag_view *)g_4e3b44[tag_index & 0xffff].bytes)->components;
}

// @retail 0x222770
void function_222770(long tag_index, long handle, s_effect_inputs const *inputs,
	s_sound_playback_effects *effects)
{
	s_platform_playback_view *source = NULL;
	s_platform_playback_view *playback =
		(s_platform_playback_view *)function_18d090(tag_index, handle);
	s_platform_playback_view *class_playback =
		(s_platform_playback_view *)sound_get_class(tag_index);

	if (class_playback)
	{
		void *filter = tag_block_get_first(class_playback->filter_count, class_playback->filters);
		void *pitch_lfo = tag_block_get_first(class_playback->pitch_lfo_count, class_playback->pitch_lfos);
		void *filter_lfo = tag_block_get_first(class_playback->filter_lfo_count, class_playback->filter_lfos);
		s_playback_sound_effect *effect = (s_playback_sound_effect *)tag_block_get_first(
			class_playback->effect_count, class_playback->effects);

		effects->unknown08 = 0;
		effects->unknown0c = 0;
		effects->filter = filter;
		effects->pitch_lfo = pitch_lfo;
		effects->filter_lfo = filter_lfo;
		if (playback_sound_effect_valid(effect))
			source = class_playback;
	}
	if (playback && playback != class_playback)
	{
		void *filter = tag_block_get_first(playback->filter_count, playback->filters);
		void *pitch_lfo = tag_block_get_first(playback->pitch_lfo_count, playback->pitch_lfos);
		void *filter_lfo = tag_block_get_first(playback->filter_lfo_count, playback->filter_lfos);
		s_playback_sound_effect *effect = (s_playback_sound_effect *)tag_block_get_first(
			playback->effect_count, playback->effects);

		if (filter)
			effects->filter = filter;
		if (pitch_lfo)
			effects->pitch_lfo = pitch_lfo;
		if (filter_lfo)
			effects->filter_lfo = filter_lfo;
		if (playback_sound_effect_valid(effect))
			source = playback;
	}

	if (source)
	{
		/* Preserve retail's selected-playback effect read exactly. */
		s_playback_sound_effect *effect = playback->effect_count > 0 ? playback->effects : NULL;
		function_2225a0(effect->overrides,
			sound_effect_tag_get_components(effect->definition_tag_index), inputs,
			effects->effect.data, &effects->effect.size);
	}
	else
	{
		effects->effect.size = 0;
	}
}
