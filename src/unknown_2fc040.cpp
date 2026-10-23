// @flags /O2 /GL-
// Havok (third-party physics library): creating, from a description, an
// object holding a value and a reference to a child object.
#include "shape_2de750.h"

struct info_2fc040 : shape_info_2de750
{
	referenced_2d88a0 *child;
};

class child_holder_2fc040 : public shape_2de750
{
public:
	REFERENCED_NEW_2D88A0(0x22)

	referenced_2d88a0 *child;

	child_holder_2fc040(info_2fc040 const &info) : shape_2de750(info), child(info.child)
	{
		child->reference_count++;
	}
};

// @retail 0x2fc040
child_holder_2fc040 *__cdecl function_2fc040(info_2fc040 const &info)
{
	return new child_holder_2fc040(info);
}
