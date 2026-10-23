// @flags /O2 /GL- /GX
// Havok (third-party physics library): creates a radius-only object from its
// description.
#include "shape_2de750.h"

class radius_only_2deff0 : public radius_shape_2de750
{
public:
	REFERENCED_NEW_2D88A0(0x22)

	radius_only_2deff0(radius_info_2de750 const &info);
};

// @retail 0x2df400
radius_only_2deff0 *__cdecl function_2df400(radius_info_2de750 const &info)
{
	return new radius_only_2deff0(info);
}
