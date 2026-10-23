// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_181800.CPP: whether a sphere lies outside, across or inside a
   frustum given by its bounds and six planes */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"

struct s_frustum_bounds
{
	real x0;
	real x1;
	real y0;
	real y1;
	real z0;
	real z1;
};

struct s_sphere_frustum
{
	byte unknown00[0x60];
	s_frustum_bounds bounds;
	plane3f planes[6];
};

// @retail 0x181800
long function_181800(s_sphere_frustum const *frustum, point3f const *center, real radius)
{
	s_frustum_bounds const &bounds = frustum->bounds;
	if (center->x - radius > bounds.x1)
	{
		return 0;
	}
	if (center->y - radius > bounds.y1)
	{
		return 0;
	}
	if (center->z - radius > bounds.z1)
	{
		return 0;
	}
	if (bounds.x0 > center->x + radius)
	{
		return 0;
	}
	if (bounds.y0 > radius + center->y)
	{
		return 0;
	}
	if (bounds.z0 > radius + center->z)
	{
		return 0;
	}

	real distance0 = plane_distance_to_point(&frustum->planes[0], center);
	if (distance0 > radius)
	{
		return 0;
	}
	real distance1 = plane_distance_to_point(&frustum->planes[1], center);
	if (distance1 > radius)
	{
		return 0;
	}
	real distance2 = plane_distance_to_point(&frustum->planes[2], center);
	if (distance2 > radius)
	{
		return 0;
	}
	real distance3 = plane_distance_to_point(&frustum->planes[3], center);
	if (distance3 > radius)
	{
		return 0;
	}
	if (plane_distance_to_point(&frustum->planes[4], center) > radius)
	{
		return 0;
	}
	real distance5 = plane_distance_to_point(&frustum->planes[5], center);
	if (distance5 > radius)
	{
		return 0;
	}
	return distance0 < -radius && distance1 < -radius && distance2 < -radius && distance3 < -radius && distance5 < -radius ? 2 : 1;
}
