#include "sound_sources.h"
#include <math.h>

#ifdef H2_NATIVE_PORT

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

struct s_sound_system_doppler_view
{
	byte unknown00[0x80];
	long time;
	s_sound_listener listeners[4];
};
static_assert(sizeof(s_sound_system_doppler_view) == 0x1a4,
	"sound listener array layout");

real normalize(vector3f *value)
{
	real const length = static_cast<real>(sqrt(
		value->i * value->i + value->j * value->j + value->k * value->k));
	if (!(0.0001f > fabs(length)))
	{
		real const inverse = 1.0f / length;
		value->i = inverse * value->i;
		value->j *= inverse;
		value->k *= inverse;
	}
	return length;
}
}

struct s_4e6380;
extern s_4e6380 *g_4e6380;

// @retail 0x12a9d0
real function_12a9d0(long listener_index, s_sound_position const *position)
{
	s_sound_listener *listener =
		&reinterpret_cast<s_sound_system_doppler_view *>(g_4e6380)->listeners[listener_index];
	vector3f direction;
	direction.i = position->position.x - listener->position.x;
	direction.j = position->position.y - listener->position.y;
	direction.k = position->position.z - listener->position.z;
	normalize(&direction);
	vector3f velocity = listener->velocity;
	if (listener->velocity_scale != 1.0f)
	{
		velocity.i = listener->velocity_scale * velocity.i;
		velocity.j = listener->velocity_scale * velocity.j;
		velocity.k = listener->velocity_scale * velocity.k;
	}
	vector3f listener_velocity;
	listener_velocity.i = listener->up.i * velocity.k +
		listener->left.i * velocity.j + listener->forward.i * velocity.i;
	listener_velocity.j = listener->up.j * velocity.k +
		listener->left.j * velocity.j + listener->forward.j * velocity.i;
	listener_velocity.k = listener->up.k * velocity.k +
		listener->left.k * velocity.j + listener->forward.k * velocity.i;
	real const listener_speed = 0.0f -
		(listener_velocity.k * direction.k + listener_velocity.j * direction.j +
		 listener_velocity.i * direction.i);
	real const source_speed = position->velocity.k * direction.k +
		position->velocity.j * direction.j + position->velocity.i * direction.i +
		111.548553f;
	real denominator = source_speed;
	if (denominator < 0.001f)
		denominator = 0.001f;
	real ratio = (111.548553f - listener_speed) / denominator;
	if (ratio < 0.125f)
		ratio = 0.125f;
	else if (ratio > 4.0f)
		ratio = 4.0f;
	return static_cast<real>(log(ratio) * 1731.234f);
}

#endif
