// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_11FC80.CPP: polygon clipping and vector helpers (the worker job
   queue that followed is in async.cpp, the profile setters in
   unknown_120d80.cpp) */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include <math.h>
#include <string.h>
#include <xtl.h>

#define k_real_epsilon 0.0001f

real function_30bf0(vector3f *v);

void function_2922b1(vector3f const *a, vector3f const *b, vector3f *out);

/* ---- globals ---- */

extern short const g_440b94[6][3] =
{
	{ 2, 1, 0 },
	{ 1, 2, 0 },
	{ 0, 2, 1 },
	{ 2, 0, 1 },
	{ 1, 0, 2 },
	{ 0, 1, 2 },
};

PRIVATE __forceinline void function_11fc81(point3f *arg_0, point3f const *arg_1, vector3f const *arg_2, real arg_3)
{
	real local_0 = arg_2->i * arg_3;
	local_0 += arg_1->x;
	arg_0->x = local_0;
	local_0 = arg_3 * arg_2->j;
	local_0 += arg_1->y;
	arg_0->y = local_0;
	arg_3 *= arg_2->k;
	arg_0->z = arg_3 + arg_1->z;
}

// @retail 0x11fc80
long function_11fc80(
	plane3f const *plane,
	bool keep_inside,
	real tolerance,
	long point_count,
	point3f const *points,
	long maximum_count,
	point3f *out)
{
	long count = 0;

	if (point_count > 0)
	{
		real offset = keep_inside ? tolerance : 0.f - tolerance;
		point3f const *previous = &points[point_count - 1];
		// retail evaluates the distances in SSE single precision: every product and sum is rounded to float
		real previous_z = plane->k * previous->z;
		real previous_y = plane->j * previous->y;
		real previous_distance = previous_z + previous_y;
		real previous_x = plane->i * previous->x;
		previous_distance += previous_x;
		previous_distance -= plane->d;
		previous_distance -= offset;
		bool previous_inside = previous_distance >= 0.f;

		for (long i = 0; i < point_count; i++)
		{
			point3f const *current = &points[i];
			real distance = (plane->i * current->x + plane->k * current->z) + plane->j * current->y;
			distance -= plane->d;
			distance -= offset;
			bool inside = distance >= 0.f;

			if (inside != previous_inside)
			{
				if (count >= maximum_count)
					break;
				vector3f delta;
				vector3d_from_points3d(previous, current, &delta);
				real span = previous_distance - distance;
				real t = previous_distance / span;
				function_11fc81(&out[count], previous, &delta, t);
				count++;
			}
			if (inside == keep_inside)
			{
				if (count >= maximum_count)
					break;
				out[count] = *current;
				count++;
			}
			previous = current;
			previous_distance = distance;
			previous_inside = inside;
		}
	}
	return count;
}

static inline real distance_squared(point3f const *a, point3f const *b)
{
	real dx = a->x - b->x;
	real dy = a->y - b->y;
	real dz = a->z - b->z;

	return dz * dz + dy * dy + dx * dx;
}

/* two-bone inverse kinematics: moves the middle node so that the chain from
   the root reaches the target (clamped to the chain's reach), rebuilds the
   root and middle axes and copies the target into the end node */
// @retail 0x120220
void function_120220(
	transform4x3f *target,
	transform4x3f *end,
	transform4x3f *root,
	transform4x3f *mid)
{
	real mid_root_squared = distance_squared(&mid->position, &root->position);
	real end_mid_squared = distance_squared(&end->position, &mid->position);
	real root_target_squared = distance_squared(&root->position, &target->position);

	if (mid_root_squared > k_real_epsilon && end_mid_squared > k_real_epsilon && root_target_squared > k_real_epsilon)
	{
		real mid_root = (real)sqrt(mid_root_squared);
		real end_mid = (real)sqrt(end_mid_squared);
		real root_target = (real)sqrt(root_target_squared);

		vector3f delta;
		delta.i = target->position.x - root->position.x;
		delta.j = target->position.y - root->position.y;
		delta.k = target->position.z - root->position.z;
		vector3f to_mid;
		to_mid.i = mid->position.x - root->position.x;
		to_mid.j = mid->position.y - root->position.y;
		to_mid.k = mid->position.z - root->position.z;

		real inverse = 1.f / root_target;
		vector3f u;
		u.i = delta.i * inverse;
		u.j = delta.j * inverse;
		u.k = delta.k * inverse;

		vector3f normal;
		function_2922b1(&u, &to_mid, &normal);
		function_30bf0(&normal);

		vector3f e;
		function_2922b1(&normal, &u, &e);

		real distance = root_target;
		real limit = (end_mid + mid_root) * 0.98f;
		if (distance > limit)
		{
			target->position.x = limit * u.i + root->position.x;
			target->position.y = limit * u.j + root->position.y;
			target->position.z = limit * u.k + root->position.z;
			distance = limit;
		}

		real along = (distance * distance + mid_root * mid_root - end_mid * end_mid) / (distance * 2.f);
		real remaining = distance - along;
		real height_squared = mid_root * mid_root - along * along;
		real height;
		if (height_squared >= k_real_epsilon)
			height = (real)sqrt(height_squared);
		else
			height = 0.f;

		vector3f lift;
		lift.i = e.i * height;
		lift.j = e.j * height;
		lift.k = e.k * height;

		vector3f *root_left = &root->left;
		vector3f *mid_left = &mid->left;
		vector3f *root_forward = &root->forward;
		vector3f *root_up = &root->up;

		root_forward->i = along * u.i + lift.i;
		root_forward->j = along * u.j + lift.j;
		root_forward->k = along * u.k + lift.k;
		function_30bf0(root_forward);

		function_2922b1(root_forward, root_left, root_up);
		function_30bf0(root_up);

		function_2922b1(root_up, root_forward, root_left);

		point3f new_mid;
		new_mid.x = mid_root * root_forward->i + root->position.x;
		new_mid.y = root_forward->j * mid_root + root->position.y;
		new_mid.z = root_forward->k * mid_root + root->position.z;

		vector3f *mid_forward = &mid->forward;
		vector3f *mid_up = &mid->up;

		mid_forward->i = remaining * u.i - lift.i;
		mid_forward->j = remaining * u.j - lift.j;
		mid_forward->k = remaining * u.k - lift.k;
		function_30bf0(mid_forward);

		function_2922b1(mid_forward, mid_left, mid_up);
		function_30bf0(mid_up);

		function_2922b1(mid_up, mid_forward, mid_left);

		mid->position = new_mid;
		*end = *target;
	}
}

// @retail 0x1201a0
real function_1201a0(vector3f *v, vector3f const *fallback)
{
	real m = (real)sqrt(v->i * v->i + v->j * v->j + v->k * v->k);
	if (!(fabs(m) < k_real_epsilon))
	{
		real inv = 1.f / m;
		v->i = v->i * inv;
		v->j = inv * v->j;
		v->k = inv * v->k;
		return m;
	}
	*v = *fallback;
	return 0.f;
}

struct s_120791
{
    real field_0;
};

static __forceinline s_120791 function_120791(real const *arg_0, real const *arg_1, short arg_2, short arg_3, short arg_4)
{
    s_120791 local_0;
    local_0.field_0 = arg_0[3] - arg_0[arg_2] * arg_1[0] - arg_0[arg_3] * arg_1[1];
    local_0.field_0 /= arg_0[arg_4];
    return local_0;
}

// @retail 0x120790
real *function_120790(real *out, real const *plane, real const *point, short axis, byte side)
{
	long local_0 = side + axis * 2;
	short a = g_440b94[local_0][0];
	short b = g_440b94[local_0][1];

	out[a] = point[0];
	out[b] = point[1];
	if (fabs(plane[axis]) < k_real_epsilon)
		out[axis] = 0.f;
	else
		out[axis] = function_120791(plane, point, a, b, axis).field_0;
	return out;
}

// @retail 0x120810
real *function_120810(real const *point, short axis, byte side, real *out)
{
	long index = side + axis * 2;
	real x = point[g_440b94[index][0]];
	real y = point[g_440b94[index][1]];

	out[0] = x;
	out[1] = y;
	return out;
}
