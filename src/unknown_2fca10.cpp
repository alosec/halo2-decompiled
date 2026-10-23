// @flags /O2 /GL-
// Havok (third-party physics library): an object holding a copy of a list of
// points, built from its description, and a triangle's description out as a
// free function.
#include "shape_2de750.h"

struct points_info_2fca10 : shape_info_2de750
{
	__m128 const *points;
	long count;
};

class points_2fca10 : public shape_2de750
{
public:
	long count;
	__m128 points[1];

	points_2fca10(points_info_2fca10 const &info);
	virtual void slot_1() {}
};

// @retail 0x2fca10
points_2fca10::points_2fca10(points_info_2fca10 const &info) : shape_2de750(info)
{
	count = info.count;
	for (int i = 0; i < count; i++)
	{
		points[i] = info.points[i];
	}
}

struct s_2fcb70;

class triangle_2fcb70
{
public:
	void function_2fcb70(s_2fcb70 *out) const;
};

// @retail 0x2fca60
void __cdecl function_2fca60(triangle_2fcb70 const *object, s_2fcb70 *out)
{
	object->function_2fcb70(out);
}
