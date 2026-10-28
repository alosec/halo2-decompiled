// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_0DE080.CPP: a unit's camera, moved towards its definition's offset
   as the unit looks down (lane M, for 0xcafc0) */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "unknown_11cc90.h"
#include "slot_handler.h"
#include <math.h>

/* the unit as this file reads it */
struct s_unit_camera_object
{
	long definition_index;
	byte unknown004[0x116 - 0x4];
	short node_matrices_offset;
	byte unknown118[0x3c8 - 0x118];
	bool unknown3c8;
	byte unknown3c9[3];
	point3f unknown3cc;
};

struct s_unit_camera_object_header
{
	byte unknown00[8];
	s_unit_camera_object *object;
};

/* the unit's tag */
struct s_unit_camera_definition
{
	byte unknown000[0x224];
	real pitch_minimum;
	real pitch_maximum;
	real unknown22c;
	real unknown230;
	real unknown234;
	real unknown238;
};

#define CAMERA_OBJECT(index) (((s_unit_camera_object_header *)g_4e0300->data)[(index) & 0xffff].object)
#define CAMERA_DEFINITION(index) ((s_unit_camera_definition *)g_4e3b44[(index) & 0xffff].bytes)

extern vector2f *g_468778;

bool function_a74c0(vector3f const *forward, vector3f const *up);
real function_17ca10(real x, short curve);

static inline real normalize2d(vector2f *v)
{
	real m = (real)sqrt(v->i * v->i + v->j * v->j);
	if (!(fabs(m) < 0.0001f))
	{
		real inv = 1.f / m;
		v->i = v->i * inv;
		v->j = v->j * inv;
		return m;
	}
	return 0.f;
}

static inline real dot_product2d(vector2f const *a, vector2f const *b)
{
	return a->i * b->i + a->j * b->j;
}

// @retail 0xde080
void function_de080(vector3f const *forward, vector3f const *up, vector2f *forward2d, vector2f *left2d)
{
	*forward2d = *(vector2f const *)forward;
	if (normalize2d(forward2d) == 0.0f)
	{
		*forward2d = *(vector2f const *)up;
		if (forward->k > 0.0f)
		{
			forward2d->i = -forward2d->i;
			forward2d->j = -forward2d->j;
		}
		if (normalize2d(forward2d) == 0.0f)
			*forward2d = *g_468778;
	}
	left2d->i = -forward2d->j;
	left2d->j = forward2d->i;
}

// score 84 (was 84): frame 0x1c vs 0x28, basis and vertical-displacement homes differ.
// complete displacement 94; displacement/target helper 94; native target copy 84;
// displacement/native copy 94; node helper with/without complete displacement 144/139;
// paired basis names 94; both displacement helpers/names 94; projected point setter
// with split/complete displacement 84/100.
// @retail 0xdf380
void function_df380(long unit_index, point3f *position, vector3f *forward, vector3f *up)
{
	s_unit_camera_object *unit = CAMERA_OBJECT(unit_index);
	s_unit_camera_definition *definition = CAMERA_DEFINITION(unit->definition_index);

	function_a74c0(forward, up);
	if (definition->pitch_maximum > definition->pitch_minimum)
	{
		real range = definition->pitch_maximum - definition->pitch_minimum;
		real pitch;
		real t;

		if (-1.0f >= forward->k)
			pitch = 1.5707964f;
		else if (0.0f > forward->k)
			pitch = 1.5707964f - (real)acos(-forward->k);
		else
			pitch = 0.0f;

		t = (pitch - definition->pitch_minimum) / range;
		if (0.0f > t)
			t = 0.0f;
		else if (t > 1.0f)
			t = 1.0f;
		t = function_17ca10(t, 5);

		if (t > 0.0f && unit->unknown3c8)
		{
			vector2f forward2d;
			vector2f left2d;
			point3f *origin;
			vector3f offset;
			point3f local;
			point3f target;
			vector3f delta;

			function_de080(forward, up, &forward2d, &left2d);
			origin = &object_node_matrix(object_get(unit_index), 0)->position;
			offset.i = position->x - origin->x;
			offset.j = position->y - origin->y;
			offset.k = position->z - origin->z;
			local.x = dot_product2d(&forward2d, (vector2f const *)&offset);
			local.y = dot_product2d(&left2d, (vector2f const *)&offset);
			local.z = offset.k;

			target = unit->unknown3cc;
			target.x = target.x > definition->unknown238 ? target.x : definition->unknown238;
			delta.i = target.x - local.x;
			delta.j = target.y - local.y;
			delta.k = target.z - local.z;
			delta.i = definition->unknown22c * delta.i * t;
			delta.j = definition->unknown230 * delta.j * t;
			delta.k = definition->unknown234 * delta.k * t;

			position->x += forward2d.i * delta.i + left2d.i * delta.j;
			position->y += forward2d.j * delta.i + left2d.j * delta.j;
			position->z += delta.k;
		}
	}
	function_a74c0(forward, up);
}
