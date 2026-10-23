// @flags /O2 /GL- /GX
// Havok (third-party physics library): constructing a triangle's description
// (three zero corners) in place, and creating a triangle from its
// description.
#include <new>
#include "shape_2de750.h"

struct class_2df1e0
{
	long field_0;

	class_2df1e0();
};

struct class_2df1d0 : class_2df1e0
{
	class_2df1d0();
};

struct class_2df1a0 : class_2df1d0
{
	float field_4;

	class_2df1a0();
};

struct triangle_description_2fca80 : class_2df1a0
{
	__m128 corners[3];

	triangle_description_2fca80()
	{
		corners[0] = _mm_setzero_ps();
		corners[1] = _mm_setzero_ps();
		corners[2] = _mm_setzero_ps();
	}
};

// @retail 0x2fca80
void __cdecl function_2fca80(void *memory)
{
	new (memory) triangle_description_2fca80;
}

struct triangle_info_2fcaf0 : radius_info_2de750
{
	__m128 third;
};

class triangle_2fcaf0 : public radius_shape_2de750
{
public:
	REFERENCED_NEW_2D88A0(0x22)

	__m128 corners[3];

	triangle_2fcaf0(triangle_info_2fcaf0 const &info) : radius_shape_2de750(info)
	{
		corners[0] = info.first;
		corners[1] = info.second;
		corners[2] = info.third;
	}
	virtual void slot_1() {}
};

// @retail 0x2fcaf0
triangle_2fcaf0 *__cdecl function_2fcaf0(triangle_info_2fcaf0 const &info)
{
	return new triangle_2fcaf0(info);
}
