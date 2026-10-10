// @flags /O2 /Gr
/* ARM-safe selective recovery of the playback effect selector from
   unknown_221da0.cpp.  The broad source also contains unrelated x87 inline
   assembly; keep this exact data-routing boundary independently compilable. */

#include "unknown_11c920.h"
#include "globals.h"
#include <math.h>

#define PIN(n, floor, ceiling) ((n) < (floor) ? (floor) : ((n) > (ceiling) ? (ceiling) : (n)))

struct s_tag_data_view
{
	long size;
	byte *address;
};

struct s_effect_data_header
{
	dword effect_mask;
	byte flags;
	byte unknown05[3];
	word offset;
	word size;
};

struct s_effect_data_block
{
	s_effect_data_header header;
	long data[1];
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
	s_effect_data_header header;
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
dword function_191660(long string_handle, long index);
real function_13b390(void const *function, real input, real range);
real function_30bf0(vector3f *vector);
real function_192b90(point3f const *direction, long speaker, bool linear);

// @retail 0x2221d0
long function_2221d0(real value)
{
	value = PIN(value, -1.0f, 1.0f);
	if (value >= 0.0f)
		return (long)((double)value * 8388607.0);
	return 0x1000000 - (long)((double)value * -8388608.0);
}

// @retail 0x222250
real function_222250(vector3f const *direction, long speaker)
{
	real result = 1.0f;
	if (PIN(speaker, 0, 4) == speaker)
	{
		vector3f normal = *direction;
		function_30bf0(&normal);
		result = function_192b90((point3f const *)&normal, speaker, true);
		result = PIN(result, 0.0f, 1.0f);
	}
	return result;
}

static inline long effect_real_to_long(real value)
{
	/* Retail fistp uses nearest-even under the default x87 control word. */
	double converted = value;
	long truncated = (long)converted;
	double fraction = converted - (double)truncated;
	if (fraction > 0.5 || (fraction == 0.5 && (truncated & 1)))
		++truncated;
	else if (fraction < -0.5 || (fraction == -0.5 && (truncated & 1)))
		--truncated;
	return truncated;
}

// @retail 0x2222c0
void function_2222c0(s_effect_data_block *block,
	s_effect_component const *component,
	s_effect_component_overrides const *overrides,
	s_effect_inputs const *inputs, long speaker)
{
	long source_index = 0;
	block->header = component->header;
	block->header.effect_mask = function_191660(component->name, speaker);
	if (component->header.flags & 1)
	{
		long effect = NONE;
		if (component->name == 0xd00012e)
			effect = 10;
		else if (component->name == 0x1300012d)
			effect = 4;
		block->data[0] = effect;
	}

	for (long i = 0; i < component->parameter_count; i++)
	{
		s_effect_parameter const *parameter = &component->parameters[i];
		real value = parameter->default_value;
		if (overrides && (overrides->parameter_mask & (1 << i)))
		{
			char source = overrides->sources[source_index];
			if (source & 1)
			{
				s_effect_function const *function = &overrides->functions[source >> 1];
				short input = function->input;
				bool ranged = TEST_FIELD_BIT(function->function.address[1] & 1);
				real x = inputs->values[input];
				real y = ranged ? inputs->values[function->range_input] : 0.0f;
				if (input == 1)
					x = (real)fmod((double)(x / function->period), 1.0);
				if (speaker != NONE && input == 3)
					x *= function_222250(&inputs->direction, speaker);
				if (ranged)
				{
					short range_input = function->range_input;
					if (range_input == 1)
						y = (real)fmod((double)(y / function->period), 1.0);
					if (speaker != NONE && range_input == 3)
						y *= function_222250(&inputs->direction, speaker);
				}
				if (function->function.address && function->function.size > 0)
				{
					value = function_13b390(&function->function, x, y);
					byte const *field_30 = function->function.address;
					if (!(field_30[1] & 0xf0))
					{
						real lower = *(real const *)(field_30 + 4);
						real upper = *(real const *)(field_30 + 8);
						value = lower + (upper - lower) * PIN(value, 0.0f, 1.0f);
					}
				}
				else
				{
					value = 0.0f;
				}
			}
			else
			{
				value = overrides->constants[source >> 1];
			}
			source_index++;
		}

		value = PIN(value, parameter->minimum, parameter->maximum);
		if (component->header.flags & 1)
		{
			if (parameter->type == 0)
				block->data[i + 1] = effect_real_to_long(value);
			else if (parameter->type == 1)
				*(real *)&block->data[i + 1] = value;
		}
		else
		{
			if (parameter->type == 0)
				block->data[i] = effect_real_to_long(value);
			else if (parameter->type == 1)
				block->data[i] = function_2221d0(value);
		}
	}
}

// @retail 0x2225a0
void function_2225a0(s_effect_overrides const *overrides,
	s_effect_components const *components, s_effect_inputs const *inputs,
	long *buffer, long *buffer_size)
{
	long offset = 0;
	long override_index = 0;

	buffer[0] = components->count;
	*buffer_size = 4;
	for (long i = 0; i < components->count; i++)
	{
		s_effect_component const *component = &components->components[i];
		s_effect_component_overrides const *component_overrides = NULL;
		if (overrides->component_mask & (1 << i))
			component_overrides = &overrides->components[override_index++];

		bool by_speaker = component->name == 0x11000138;
		long speaker_count = by_speaker ? 4 : 0;
		for (long speaker = by_speaker ? 0 : NONE;
			speaker < speaker_count; speaker++)
		{
			s_effect_data_block *block =
				(s_effect_data_block *)&buffer[offset + 1];
			function_2222c0(block, component, component_overrides, inputs, speaker);
			*buffer_size += block->header.size + 0xc;
			offset += (block->header.size + 0xf) >> 2;
		}
	}
}

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
