// @flags /O2 /arch:SSE /Gr
/* one step of a point's physics (retail 0x211060; the velocity in eax, ten
   arguments on the stack, ret 0x28), used by the flexible chains (0x17b850,
   flexible_chain.cpp) and others with a point physics tag:

   the point takes the flow of its medium (0x211b70 for a given index when
   flag 1 of the first argument is set, flag 2 then saying it is in water;
   else the location's cluster, 0x211b30), then:
   - its mass is (the medium density + the tag's density) * radius^3, the
     buoyancy 1 - 2 * the tag's +0x20 / (the medium's density + +0x20),
     dropped by tag flag 0x20, and the drag (+0x28 in water, +0x24 in air)
     * radius^2 * dt / mass, clamped to [0, 1] (1 when massless and dragged);
   - a force (the sixth argument) accelerates it, gravity (with buoyancy)
     pulls its k, and the drag blends the velocity toward the flow;
   - with collision flags (tag flags 2 and 4) and unless the first
     argument's flag 4 is set, the move is cast up to three times: each hit
     reflects the velocity on the plane (friction +0x2c, elasticity +0x30),
     moves the point off the plane by min(radius, 0.005) and continues with
     the time left; without collision the point simply moves and finds its
     location (0x11bed0).
   Returns 1 in air or 2 in water, with 4 or 8 for a hit of type 1 or 2. */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "unknown_1eb550.h"
#include "object_queries.h"

/* a collision result (0x5c bytes), as src/unknown_1689b0.cpp declares it */
struct s_collision_location_211060
{
	long leaf_index;
	short cluster_index;
	short bsp_index;
};

struct s_collision_result_1697c0
{
	long type;
	real t;
	point3f point;
	s_collision_location_211060 start_location;
	s_collision_location_211060 end_location;
	short material_type;
	byte unknown26[2];
	plane3f plane;
	byte unknown38[0x5c - 0x38];
};

extern real g_4f93ac; /* the water's density */
extern real g_4f93b0; /* the air's density */

bool function_211b30(byte const *location, real const *position, byte flags, vector3f *out);
void function_211b70(short index, real const *position, byte flags, vector3f *out);
bool __stdcall function_1697c0(long flags, point3f const *point, vector3f const *vector,
	long ignore_object_index, long ignore_unit_index, s_collision_result_1697c0 *result);
void function_11bed0(s_location *location, point3f const *point);

/* the point physics tag, as read here */
struct s_point_physics_view
{
	dword flags;
	real density;
	byte unknown08[0x20 - 0x08];
	real water_buoyancy; /* +0x20 */
	real air_drag; /* +0x24 */
	real water_drag; /* +0x28 */
	real friction; /* +0x2c */
	real elasticity; /* +0x30 */
};

// @retail 0x211060
long function_211060(long flags, void *physics_definition, s_location *location, long medium_index,
	point3f *position, vector3f const *force, vector3f *hit_normal, short *hit_material,
	real radius, real dt, vector3f *velocity)
{
	if (dt == 0.0f)
		return 0;

	s_point_physics_view const *physics = (s_point_physics_view const *)physics_definition;
	real radius_squared = radius * radius;
	real radius_cubed = radius_squared * radius;
	byte medium_flags = (byte)((physics->flags >> 3) & 1);
	if (physics->flags & 0x10)
		medium_flags |= 2;
	else
		medium_flags &= ~2;

	vector3f flow;
	bool in_water;
	if (flags & 1)
	{
		in_water = (flags >> 1) & 1;
		function_211b70((short)medium_index, position->n, medium_flags, &flow);
	}
	else
		in_water = function_211b30((byte const *)location, position->n, medium_flags, &flow);

	s_unknown_1eb550 const *globals = g_51e9c4;
	long result;
	real density;
	real buoyancy;
	real drag;
	if (in_water)
	{
		density = g_4f93ac + physics->density;
		buoyancy = physics->water_buoyancy * 2.0f / (globals->unknown4 + physics->water_buoyancy);
		drag = physics->water_drag;
		result = 2;
	}
	else
	{
		density = g_4f93b0 + physics->density;
		buoyancy = physics->water_buoyancy * 2.0f / (globals->unknown8 + physics->water_buoyancy);
		drag = physics->air_drag;
		result = 1;
	}
	real gravity_scale = 1.0f - buoyancy;
	drag *= radius_squared;
	if (physics->flags & 0x20)
		gravity_scale = 0.0f;

	real mass = density * radius_cubed;
	real blend;
	if (mass != 0.0f)
	{
		real scale = dt / mass;
		blend = scale * drag;
		if (blend < 0.0f)
			blend = 0.0f;
		else if (blend > 1.0f)
			blend = 1.0f;
		if (force)
		{
			velocity->i += force->i * scale;
			velocity->j += force->j * scale;
			velocity->k += force->k * scale;
		}
	}
	else
		blend = drag == 0.0f ? 0.0f : 1.0f;

	velocity->k += globals->unknown0 * gravity_scale * dt;
	velocity->j += (flow.j - velocity->j) * blend;
	velocity->i += (flow.i - velocity->i) * blend;
	velocity->k += (flow.k - velocity->k) * blend;

	long collision_flags = (physics->flags & 2) ? 0x800005 : 0;
	if (physics->flags & 4)
		collision_flags |= 0x800002;

	if (!collision_flags || (flags & 4))
	{
		position->x += velocity->i * dt;
		position->y += velocity->j * dt;
		position->z += velocity->k * dt;
		s_location found;
		function_11bed0(&found, position);
		if (found.leaf_index != NONE)
			*location = found;
		return result;
	}

	for (short iteration = 0; iteration < 3; ++iteration)
	{
		vector3f displacement;
		displacement.i = velocity->i * dt;
		displacement.j = velocity->j * dt;
		displacement.k = velocity->k * dt;
		s_collision_result_1697c0 collision;
		collision.material_type = NONE;
		if (!function_1697c0(collision_flags, position, &displacement, NONE, NONE, &collision))
		{
			if (collision.end_location.leaf_index != NONE)
				*(s_collision_location_211060 *)location = collision.end_location;
			*position = collision.point;
			return result;
		}

		real offset = radius > 0.005f ? 0.005f : radius;
		if (collision.type == 2)
			result |= 8;
		else if (collision.type == 1)
			result |= 4;
		if (hit_normal)
			*hit_normal = collision.plane.n;
		if (hit_material)
			*hit_material = collision.material_type;

		/* the velocity reflected on the plane */
		vector3f const *normal = &collision.plane.n;
		real dot = velocity->k * normal->k + velocity->j * normal->j + velocity->i * normal->i;
		real normal_i = dot * normal->i;
		real normal_j = dot * normal->j;
		real normal_k = dot * normal->k;
		real tangent_i = velocity->i - normal_i;
		real tangent_j = velocity->j - normal_j;
		real tangent_k = velocity->k - normal_k;
		velocity->i = (1.0f - physics->friction) * tangent_i - physics->elasticity * normal_i;
		velocity->j = (1.0f - physics->friction) * tangent_j - physics->elasticity * normal_j;
		velocity->k = (1.0f - physics->friction) * tangent_k - physics->elasticity * normal_k;

		if (collision.end_location.leaf_index != NONE)
			*(s_collision_location_211060 *)location = collision.end_location;
		position->x = normal->i * offset + collision.point.x;
		position->y = normal->j * offset + collision.point.y;
		position->z = normal->k * offset + collision.point.z;

		dt -= collision.t * dt;
		if (dt == 0.0f)
			break;
	}
	return result;
}
