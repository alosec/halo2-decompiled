// @flags /O2 /GL-
// Havok (third-party physics library): builds a radius-carrying object with
// two points, each with the radius in its fourth lane.
#include "shape_2de750.h"

class two_point_2de750 : public radius_shape_2de750
{
public:
	REFERENCED_NEW_2D88A0(0x22)

	__m128 first;
	__m128 second;

	two_point_2de750(radius_info_2de750 const &info);
};

// @retail 0x2de750
two_point_2de750::two_point_2de750(radius_info_2de750 const &info) : radius_shape_2de750(info)
{
	first = info.first;
	((float *)&first)[3] = info.radius;
	second = info.second;
	((float *)&second)[3] = info.radius;
}
