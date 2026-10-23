// @flags /O2 /GL-
// Havok (third-party physics library): the child shape's constructor from
// its description.
#include "child_shape_2dbfc0.h"

// @retail 0x2dbf10
c_2dbfc0::c_2dbfc0(info_2dbf10 const &info)
	: radius_base_2dbfc0(info), child(info.child)
{
	field_10 = _mm_setzero_ps();
	child->reference_count++;
	field_34 = 0;
	distance_squared = info.distance * info.distance;
	distance = info.distance;
	float second = info.second_distance;
	second_distance = second;
	float gap = distance - second;
	shortfall_squared = gap * gap;
	radius = child->radius;
}
