// @flags /O2 /GL- /GX
// Havok (third-party physics library): creating the object holding a copy of
// a list of points from its description.
#include "shape_2de750.h"

struct points_info_2fca10 : shape_info_2de750
{
	__m128 const *points;
	long count;
};

class points_2fca10 : public shape_2de750
{
public:
	REFERENCED_NEW_2D88A0(0x22)

	long count;
	__m128 points[8];

	points_2fca10(points_info_2fca10 const &info);
};

// @retail 0x2fc550
points_2fca10 *__cdecl function_2fc550(points_info_2fca10 const &info)
{
	return new points_2fca10(info);
}
