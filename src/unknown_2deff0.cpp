// @flags /O2 /GL-
// Havok (third-party physics library): builds a radius-only object from its
// description.
#include "shape_2de750.h"

class radius_only_2deff0 : public radius_shape_2de750
{
public:
	REFERENCED_NEW_2D88A0(0x22)

	radius_only_2deff0(radius_info_2de750 const &info);
};

// @retail 0x2deff0
radius_only_2deff0::radius_only_2deff0(radius_info_2de750 const &info) : radius_shape_2de750(info)
{
}
