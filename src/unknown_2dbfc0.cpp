// @flags /O2 /GL-
// Havok (third-party physics library): the child shape's constructor from a
// child, a value and a distance, with the default radius and a second
// distance of 5.
#include "child_shape_2dbfc0.h"

// the default radius, 0.05 (src/unknown_2df1a0.cpp)
extern float g_461eec;

// @retail 0x2dbfc0
c_2dbfc0::c_2dbfc0(long arg_0, void *arg_1, float arg_2)
	: radius_base_2dbfc0(g_461eec), child((radius_base_2dbfc0 *)arg_0)
{
	radius_base_2dbfc0 *child_ = (radius_base_2dbfc0 *)arg_0;
	field_10 = _mm_setzero_ps();
	child_->reference_count++;
	field_34 = (long)arg_1;
	distance_squared = arg_2 * arg_2;
	distance = arg_2;
	second_distance = 5.0f;
	float gap = arg_2 - 5.0f;
	shortfall_squared = gap * gap;
	radius = child_->radius;
}
