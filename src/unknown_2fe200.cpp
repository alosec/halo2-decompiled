// @flags /O2 /GL-
// Havok (third-party physics library): a constructor from a description
// holding a value and two reference-counted objects, taking a reference to
// each.
#include "shape_2de750.h"

struct info_2fe200 : shape_info_2de750
{
	referenced_2d88a0 *first;
	referenced_2d88a0 *second;
};

class pair_2fe200 : public shape_2de750
{
public:
	referenced_2d88a0 *first;
	referenced_2d88a0 *second;

	pair_2fe200(info_2fe200 const &info);
};

// @retail 0x2fe200
pair_2fe200::pair_2fe200(info_2fe200 const &info)
	: shape_2de750(info), first(info.first), second(info.second)
{
	second->reference_count++;
	first->reference_count++;
}
