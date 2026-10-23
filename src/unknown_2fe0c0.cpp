// @flags /O2 /GL- /GX
// Havok (third-party physics library): creating an object that holds two
// reference-counted objects from its description.
#include "shape_2de750.h"

struct info_2fe200;

class pair_2fe200 : public shape_2de750
{
public:
	REFERENCED_NEW_2D88A0(0x22)

	referenced_2d88a0 *first;
	referenced_2d88a0 *second;

	pair_2fe200(info_2fe200 const &info);
};

// @retail 0x2fe0c0
pair_2fe200 *__cdecl function_2fe0c0(info_2fe200 const &info)
{
	return new pair_2fe200(info);
}
