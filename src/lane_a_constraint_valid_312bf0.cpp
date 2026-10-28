// @flags /O2 /GL-
#include "lane_a_constraints.h"
#include <math.h>

static __forceinline float lane_a_dot3(__m128 a, __m128 b)
{
	__m128 product = _mm_mul_ps(a, b);
	__m128 y = _mm_shuffle_ps(product, product, 0x55);
	__m128 z = _mm_shuffle_ps(product, product, 0xaa);
	y = _mm_add_ss(y, product);
	z = _mm_add_ss(z, y);
	float value;
	_mm_store_ss(&value, z);
	return value;
}
// @retail 0x312bf0
bool_result_2da7f0 lane_a_ragdoll::valid() const
{
	if (!(fabs(lane_a_dot3(twist_a, twist_a) - 1.0f) < 1.0e-5f)) return false;
	if (!(fabs(lane_a_dot3(twist_b, twist_b) - 1.0f) < 1.0e-5f)) return false;
	if (!(fabs(lane_a_dot3(plane_a, plane_a) - 1.0f) < 1.0e-5f)) return false;
	if (!(fabs(lane_a_dot3(plane_b, plane_b) - 1.0f) < 1.0e-5f)) return false;
	if (fabs(cone_limit) > 1.0f) return false;
	if (plane_low > plane_high) return false;
	if (twist_low > twist_high) return false;
	return true;
}
// @retail 0x313cc0
bool_result_2da7f0 lane_a_hinge::valid() const
{
	if (!(fabs(fabs(lane_a_dot3(axle_a, perpendicular_a))) < 1.0e-5f)) return false;
	if (!(fabs(lane_a_dot3(axle_a, axle_a) - 1.0f) < 1.0e-5f)) return false;
	if (!(fabs(lane_a_dot3(perpendicular_a, perpendicular_a) - 1.0f) < 1.0e-5f)) return false;
	if (!(fabs(lane_a_dot3(axle_b, axle_b) - 1.0f) < 1.0e-5f)) return false;
	return true;
}
// @retail 0x3135f0
bool_result_2da7f0 lane_a_limited_hinge::valid() const
{
	if (!(fabs(fabs(lane_a_dot3(perp_a, perp_b))) < 1.0e-5f)) return false;
	if (!(fabs(fabs(lane_a_dot3(reference_b, axle_b))) < 1.0e-5f)) return false;
	if (!(fabs(lane_a_dot3(perp_a, perp_a) - 1.0f) < 1.0e-5f)) return false;
	if (!(fabs(lane_a_dot3(perp_b, perp_b) - 1.0f) < 1.0e-5f)) return false;
	if (!(fabs(lane_a_dot3(axle_b, axle_b) - 1.0f) < 1.0e-5f)) return false;
	if (!(fabs(lane_a_dot3(axle_a, axle_a) - 1.0f) < 1.0e-5f)) return false;
	float range;
	if (angle_min <= 0.0f && angle_max <= 0.0f)
		range = (float)fabs(angle_min) + angle_max;
	else
		range = angle_max - angle_min;
	if (range > 6.28318548f) return false;
	return true;
}
