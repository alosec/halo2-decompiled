#ifndef TRIANGLE_AGENT_2F42F0_H
#define TRIANGLE_AGENT_2F42F0_H
// Havok (third-party physics library): the object kept for a pair of shapes
// one of which is a triangle: the triangle's edge and area measures, and
// three indices that start out unset.
#include <xmmintrin.h>
#include "pair_agent_2ed170.h"

struct triangle_measures_308a50
{
	float inverse_edge_lengths[3];
	float inverse_double_area;
	float double_area;
};

void __cdecl function_308a50(__m128 const *vertices, triangle_measures_308a50 *measures);

// a shape whose three corners start at its 16th byte
struct triangle_2f42f0
{
	char field_0[0x10];
	__m128 vertices[3];
};

class triangle_agent_2f42f0 : public sized_2ed170
{
public:
	unsigned short field_c;
	unsigned short field_e;
	unsigned short field_10;
	triangle_measures_308a50 measures;

	__forceinline triangle_agent_2f42f0(triangle_2f42f0 const **triangle, long arg_3) :
		sized_2ed170(arg_3)
	{
		field_c = 0xffff;
		field_e = 0xffff;
		field_10 = 0xffff;
		function_308a50((*triangle)->vertices, &measures);
	}
};

// the same with the two shapes the other way round
class swapped_triangle_agent_2f42f0 : public triangle_agent_2f42f0
{
public:
	__forceinline swapped_triangle_agent_2f42f0(triangle_2f42f0 const **triangle, long arg_3) :
		triangle_agent_2f42f0(triangle, arg_3)
	{
	}
};

#endif
