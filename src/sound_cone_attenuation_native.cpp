#include "unknown_11c920.h"
#include "unknown_218ac0.h"
#include <math.h>

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

s_sound_definition *sound_definition_get(long definition_index);
real sound_get_distance_gain(long definition_index, s_sound const *sound, real distance);
real sound_get_inner_cone_angle(s_sound const *sound, long definition_index);
real sound_get_outer_cone_angle(s_sound const *sound, long definition_index);
long sound_get_outer_cone_gain(s_sound const *sound, long definition_index);
real function_12aff0(real a, real b, real c, bool flag);
real function_2195f0(real decibels);
real function_30bf0(vector3f *vector);
real function_11cf50(vector3f const *a, vector3f const *b);

namespace
{
inline bool test(word value, unsigned bit)
{
	return (value & (word(1) << bit)) != 0;
}

inline real length_squared(vector3f const *value)
{
	return value->i * value->i + value->j * value->j + value->k * value->k;
}

inline real decibels_from_bits(long bits)
{
	union
	{
		long integer;
		real scalar;
	} value = {bits};
	return value.scalar;
}
}

// @retail 0x12ad30
real function_12ad30(vector3f const *offset, s_sound const *sound,
	long definition_index, vector3f const *direction)
{
	real const distance_gain = sound_get_distance_gain(
		definition_index, sound, static_cast<real>(sqrt(length_squared(offset))));
	real cone_gain = 1.0f;
	if ((test(sound->flags, 4) || test(sound->flags, 5) ||
		!(sound_definition_get(definition_index)->flags & 0x1000)) &&
		!(fabs(distance_gain) < 0.0001f) &&
		length_squared(offset) >= 0.001f * 0.001f)
	{
		vector3f reverse = *offset;
		reverse.i = 0.0f - reverse.i;
		reverse.j = 0.0f - reverse.j;
		reverse.k = 0.0f - reverse.k;
		if (function_30bf0(&reverse) > 0.0f)
		{
			long const gain = sound_get_outer_cone_gain(sound, definition_index);
			real const angle = function_11cf50(&reverse, direction);
			real const outer = sound_get_outer_cone_angle(sound, definition_index);
			real const inner = sound_get_inner_cone_angle(sound, definition_index);
			real const fraction = function_12aff0(inner, outer, angle, true);
			cone_gain = (function_2195f0(decibels_from_bits(gain)) - 1.0f) *
				fraction + 1.0f;
		}
	}
	real result = cone_gain * distance_gain;
	if (test(sound->flags, 9))
		result = 1.0f - result;
	return result;
}

#endif
