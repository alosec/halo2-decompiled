#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_218ac0.h"

#ifdef H2_NATIVE_PORT

struct s_sound
{
	word flags;
	byte unknown02[6];
	word flags08;
	word flags0a;
	byte unknown0c[0x28];
	real minimum_distance;
	real maximum_distance;
	word inner_cone_angle;
	word outer_cone_angle;
	long outer_cone_gain;
};
static_assert(sizeof(s_sound) == 0x44, "sound override layout");

namespace
{
constexpr real k_pi = 3.14159274f;

struct s_sound_class
{
	real minimum_distance;
	real maximum_distance;
	real skip_fraction_scale;
	byte unknown0c[4];
	long gain_base;
	long gain_variance;
	short pitch_lower;
	short pitch_upper;
	real inner_cone_angle;
	real outer_cone_angle;
	long outer_cone_gain;
	byte unknown28[0x10];
};
static_assert(sizeof(s_sound_class) == 0x38, "sound class layout");

struct s_sound_globals_classes_view
{
	byte unknown00[4];
	s_sound_class *classes;
};

struct s_sound_promotion_view
{
	byte unknown00[0x14];
	real minimum_distance;
	real maximum_distance;
	byte unknown1c[0x40];
};
static_assert(sizeof(s_sound_promotion_view) == 0x5c, "sound promotion layout");

inline bool test(word value, unsigned bit)
{
	return (value & (word(1) << bit)) != 0;
}

inline real decompress_angle(long value)
{
	if (value == 0)
		return 0.0f;
	if (value >= 0xffff)
		return k_pi;
	return static_cast<real>(value) * k_pi * (1.0f / 65535.0f);
}

inline s_sound_class *sound_class_get(short index)
{
	return &reinterpret_cast<s_sound_globals_classes_view *>(g_51ebd4)->classes[index];
}
}

s_sound_definition *sound_definition_get(long definition_index);
struct s_unknown_5c;
s_unknown_5c *function_221810(short index);
real function_12aff0(real a, real b, real c, bool flag);

// @retail 0x124fc0
real sound_get_minimum_distance(s_sound const *sound, long definition_index)
{
	if (test(sound->flags, 2))
		return sound->minimum_distance;
	s_sound_definition *definition = sound_definition_get(definition_index);
	if (definition->flags & 0x400)
		return reinterpret_cast<s_sound_promotion_view *>(function_221810(definition->promotion_index))->minimum_distance;
	return sound_class_get(definition->class_index)->minimum_distance;
}

// @retail 0x125010
real sound_get_maximum_distance(s_sound const *sound, long definition_index)
{
	if (test(sound->flags, 3))
		return sound->maximum_distance;
	s_sound_definition *definition = sound_definition_get(definition_index);
	if (definition->flags & 0x800)
		return reinterpret_cast<s_sound_promotion_view *>(function_221810(definition->promotion_index))->maximum_distance;
	return sound_class_get(definition->class_index)->maximum_distance;
}

// @retail 0x125060
long function_125060(s_sound const *sound, long definition_index)
{
	return test(sound->flags, 4) || test(sound->flags, 5) ||
		!(sound_definition_get(definition_index)->flags & 0x1000);
}

// @retail 0x1250a0
real sound_get_inner_cone_angle(s_sound const *sound, long definition_index)
{
	if (test(sound->flags, 4))
		return decompress_angle(sound->inner_cone_angle);
	return sound_class_get(sound_definition_get(definition_index)->class_index)->inner_cone_angle;
}

// @retail 0x125120
real sound_get_outer_cone_angle(s_sound const *sound, long definition_index)
{
	if (test(sound->flags, 5))
		return decompress_angle(sound->outer_cone_angle);
	return sound_class_get(sound_definition_get(definition_index)->class_index)->outer_cone_angle;
}

// @retail 0x1251a0
long sound_get_outer_cone_gain(s_sound const *sound, long definition_index)
{
	if (test(sound->flags, 6))
		return sound->outer_cone_gain;
	return sound_class_get(sound_definition_get(definition_index)->class_index)->outer_cone_gain;
}

// @retail 0x12ac20
real sound_get_distance_gain(long definition_index, s_sound const *sound, real distance)
{
	real minimum = sound_get_minimum_distance(sound, definition_index);
	if (minimum < 0.001f)
		minimum = 0.001f;
	real const maximum = sound_get_maximum_distance(sound, definition_index);
	real denominator = distance;
	if (denominator < minimum)
		denominator = minimum;
	return minimum / denominator * function_12aff0(maximum, minimum, distance, false);
}

#endif
