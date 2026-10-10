#include "sound_sources.h"
#include <float.h>

#ifdef H2_NATIVE_PORT

struct s_sound_location_source
{
	byte unknown00[2];
	char type;
	char spatialization : 4;
	char unknown03 : 4;
	byte unknown04[8];
	point3f position;
	byte unknown18[0x18];
	real height;
};
static_assert(sizeof(s_sound_location_source) == 0x34,
	"sound location source layout");

namespace
{
struct s_sound_listener
{
	long leaf_index;
	short cluster_index;
	bool active;
	byte unknown07;
	real velocity_scale;
	vector3f forward;
	vector3f left;
	vector3f up;
	point3f position;
	vector3f velocity;
};
static_assert(sizeof(s_sound_listener) == 0x48, "sound listener layout");

struct s_sound_system_source_view
{
	byte unknown00[0x84];
	s_sound_listener listeners[4];
};
static_assert(sizeof(s_sound_system_source_view) == 0x1a4,
	"sound source listener layout");

}

struct s_4e6380;
extern s_4e6380 *g_4e6380;
real magnitude3d(vector3f const *value);

// @retail 0x1427f0
vector3f *function_1427f0(transform4x3f const *matrix,
	vector3f const *vector, vector3f *out)
{
	real x = vector->i;
	real y = vector->j;
	real z = vector->k;
	if (matrix->scale != 1.0f)
	{
		real const inverse = 1.0f / matrix->scale;
		x = inverse * x;
		y = inverse * y;
		z = inverse * z;
	}
	out->i = matrix->rotation.forward.k * z +
		matrix->rotation.forward.j * y + matrix->rotation.forward.i * x;
	out->j = matrix->rotation.left.k * z +
		matrix->rotation.left.j * y + matrix->rotation.left.i * x;
	out->k = matrix->rotation.up.k * z +
		matrix->rotation.up.j * y + matrix->rotation.up.i * x;
	return out;
}

// @retail 0x12abe0
void sound_source_get_position(s_sound_location_source const *source,
	long listener_index, point3f *position)
{
	switch (source->type)
	{
	case 0:
		*position = source->position;
		break;
	case 1:
		position->x = source->position.x;
		position->y = source->position.y;
		position->z = reinterpret_cast<s_sound_system_source_view *>(g_4e6380)
			->listeners[listener_index].position.z;
		break;
	}
}

// @retail 0x127e20
real sound_source_get_listener_distance(s_sound_location_source const *source,
	long listener_index)
{
	real result;
	switch (source->spatialization)
	{
	case 0:
		result = 0.0f;
		break;
	case 1:
	{
		s_sound_listener const *listeners =
			reinterpret_cast<s_sound_system_source_view *>(g_4e6380)->listeners;
		real const dz = listeners[listener_index].position.z - source->position.z;
		s_sound_listener const *listener = &listeners[listener_index];
		switch (source->type)
		{
		case 0:
		{
			real const dx = listener->position.x - source->position.x;
			real const dy = listener->position.y - source->position.y;
			result = dz * dz;
			result += dy * dy;
			result += dx * dx;
			break;
		}
		case 1:
		{
			real clamped = 0.0f;
			if (!(0.0f > dz))
				clamped = dz > source->height ? source->height : dz;
			if (clamped == dz)
			{
				real const dy = source->position.y - listener->position.y;
				real const dx = source->position.x - listener->position.x;
				result = dx * dx + dy * dy;
			}
			else
				result = FLT_MAX;
			break;
		}
		}
		break;
	}
	default:
		result = magnitude3d(reinterpret_cast<vector3f const *>(&source->position));
		break;
	}
	return result;
}

#endif
