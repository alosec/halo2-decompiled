// @flags /O2 /GL-
// Havok (third-party physics library): a box's constructor from its
// description: its half extents, with the smallest of them kept in the
// fourth component (w).
#include "shape_2de750.h"

class box_2fe6d0 : public radius_shape_2de750
{
public:
	__m128 half_extents;

	box_2fe6d0(radius_info_2de750 const &info);
	virtual void slot_1() {}
};

// @retail 0x2fe6d0
box_2fe6d0::box_2fe6d0(radius_info_2de750 const &info) : radius_shape_2de750(info)
{
	half_extents = info.first;
	float smallest = half_extents.m128_f32[0] < half_extents.m128_f32[1] ? half_extents.m128_f32[0] : half_extents.m128_f32[1];
	float &w = half_extents.m128_f32[3];
	w = smallest;
	if (smallest > half_extents.m128_f32[2])
	{
		smallest = half_extents.m128_f32[2];
	}
	w = smallest;
}
