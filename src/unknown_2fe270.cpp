// @flags /O2 /GL- /GX
// Havok (third-party physics library): creating a box from its description.
#include "shape_2de750.h"

class box_2fe6d0 : public radius_shape_2de750
{
public:
	REFERENCED_NEW_2D88A0(0x22)

	__m128 half_extents;

	box_2fe6d0(radius_info_2de750 const &info);
	virtual void slot_1() {}
};

// @retail 0x2fe270
box_2fe6d0 *__cdecl function_2fe270(radius_info_2de750 const &info)
{
	return new box_2fe6d0(info);
}
