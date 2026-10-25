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

/* the cross product a x b, its components worked out last to first (inlined in 0x120220) */
PRIVATE __forceinline void function_120221(vector3f const *a, vector3f const *b, vector3f *out)
{
	real k = a->i * b->j - a->j * b->i;
	real j = a->k * b->i - a->i * b->k;
	real i = a->j * b->k - a->k * b->j;
	out->i = i;
	out->j = j;
	out->k = k;
}

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

// score 79 (was 79): current distance uses xmm3 instead of xmm2, then
// delta y/z use xmm4/2 instead of xmm3/4 through the unroll and remainder.
// Per-use indexed current 356; unsigned ordinal 82; accepted memcpy 79;
// interpolation output/previous references 79/79; complete result value
// 517; positive intersection/current capacity scopes 79/79.
// Unit-scaled difference 1280/376; complete current distance 85;
// complete previous distance 79; both complete producers 77 (two-point gain dropped);
// unadjusted distance/projection lifetimes 79/79; indexed accepted input 79;
// current Y/X/XY product orders 79/79/79. SSE distance/delta allocation still differs.
// Also 79: delta declared before the loop; t = previous_distance / (previous_distance - distance)
// computed before delta, without the span local. In the 4x unrolled body the copies' current-distance
// sums differ: retail copy 1 i*x + k*z + y*j, copy 2 i*x + y*j + z*k; ours copy 1 i*x + k*z + j*y,
// copy 2 i*x + z*k + y*j.
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
// score 26 (was 232): what still differs is operand order. In the four
// cross products on the node axes retail loads a->j before b->i in k and a->k
// before b->i in j, where ours loads b->i first; new_mid.x is
// `movaps xmm1, xmm0; mulss xmm1, [ebp]` in retail and the 1-byte-longer
// `movss xmm1, [ebp]; mulss xmm1, xmm0` in ours.
// Tried: lift as a vector (232) or three scalars (216); the direction kept in
// a delta vector (215); u = inverse * delta, u in k-i-j order, vector3f_scale
// (215-223); delta scaled in place (308); 1 / root_target written inline
// (253); a separate distance local (193); the cross product written to out
// directly, cross3f (174), with its components in i-j-k, j-i-k or j-k-i order
// (50; k-j-i, k-i-j and i-k-j give 26), or with each product's operands
// swapped (26); new_mid.x as root_forward->i * mid_root (26); the node axes
// written as root->forward etc. without the pointer locals (59).
// score 26 (was 26): frame 0x30 agrees; cross-product operand loads and the extra midpoint MOVSS byte remain.
// Six product values 28; cross accumulators 34; first/second/both input copies 512/565/741; returned cross value 462; cross reference parameters 26; local interpolation helper 28.
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

		vector3f to_mid;
		to_mid.i = mid->position.x - root->position.x;
		to_mid.j = mid->position.y - root->position.y;
		to_mid.k = mid->position.z - root->position.z;

		real inverse = 1.f / root_target;
		vector3f u;
		u.i = (target->position.x - root->position.x) * inverse;
		u.j = (target->position.y - root->position.y) * inverse;
		u.k = (target->position.z - root->position.z) * inverse;

		vector3f normal;
		function_120221(&u, &to_mid, &normal);
		function_30bf0(&normal);

		vector3f e;
		function_120221(&normal, &u, &e);

		real limit = (end_mid + mid_root) * 0.98f;
		if (root_target > limit)
		{
			target->position.x = limit * u.i + root->position.x;
			target->position.y = limit * u.j + root->position.y;
			target->position.z = limit * u.k + root->position.z;
			root_target = limit;
		}

		real along = (root_target * root_target + mid_root * mid_root - end_mid * end_mid) / (root_target * 2.f);
		real remaining = root_target - along;
		real height_squared = mid_root * mid_root - along * along;
		real height;
		if (height_squared >= k_real_epsilon)
			height = (real)sqrt(height_squared);
		else
			height = 0.f;

		vector3f *root_left = &root->left;
		vector3f *mid_left = &mid->left;
		vector3f *root_forward = &root->forward;
		vector3f *root_up = &root->up;

		root_forward->i = along * u.i + e.i * height;
		root_forward->j = along * u.j + e.j * height;
		root_forward->k = along * u.k + e.k * height;
		function_30bf0(root_forward);

		function_120221(root_forward, root_left, root_up);
		function_30bf0(root_up);

		function_120221(root_up, root_forward, root_left);

		point3f new_mid;
		new_mid.x = mid_root * root_forward->i + root->position.x;
		new_mid.y = root_forward->j * mid_root + root->position.y;
		new_mid.z = root_forward->k * mid_root + root->position.z;

		vector3f *mid_forward = &mid->forward;
		vector3f *mid_up = &mid->up;

		mid_forward->i = remaining * u.i - e.i * height;
		mid_forward->j = remaining * u.j - e.j * height;
		mid_forward->k = remaining * u.k - e.k * height;
		function_30bf0(mid_forward);

		function_120221(mid_forward, mid_left, mid_up);
		function_30bf0(mid_up);

		function_120221(mid_up, mid_forward, mid_left);

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
