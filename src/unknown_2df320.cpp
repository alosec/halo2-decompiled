// @flags /O2 /GL- /GX
// Havok (third-party physics library): creates a radius-carrying object with
// two points from its description.
#include "shape_2de750.h"

class two_point_2de750 : public radius_shape_2de750
{
public:
	REFERENCED_NEW_2D88A0(0x22)

	__m128 first;
	__m128 second;

	two_point_2de750(radius_info_2de750 const &info);
};

// @retail 0x2df320
two_point_2de750 *__cdecl function_2df320(radius_info_2de750 const &info)
{
	return new two_point_2de750(info);
}
