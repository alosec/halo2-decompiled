// @flags /O2 /GL- /GX
// Havok (third-party physics library): creating the object kept for a pair
// of shapes one of which is a triangle, holding the terms that solve for a
// point's weights in the triangle's plane and four unset indices, either way
// round.
#include <xmmintrin.h>
#include "pair_agent_2ed170.h"

void __cdecl function_308910(__m128 const *corners, float *out);

// a shape whose three corners start at its 16th byte
struct triangle_2f7b20
{
	char field_0[0x10];
	__m128 corners[3];
};

// four indices that start out unset
struct indices_2f7b20
{
	long values[4];

	indices_2f7b20()
	{
		values[0] = -1;
		values[1] = -1;
		values[2] = -1;
		values[3] = -1;
	}
};

class triangle_terms_2f7b20 : public sized_2ed170
{
public:
	float terms[4];
	indices_2f7b20 indices;

	__forceinline triangle_terms_2f7b20(triangle_2f7b20 const **triangle, long arg_3) :
		sized_2ed170(arg_3)
	{
		function_308910((*triangle)->corners, terms);
	}
};

// the same with the two shapes the other way round
class swapped_triangle_terms_2f7b20 : public triangle_terms_2f7b20
{
public:
	__forceinline swapped_triangle_terms_2f7b20(triangle_2f7b20 const **triangle, long arg_3) :
		triangle_terms_2f7b20(triangle, arg_3)
	{
	}
};

// score 12 (was 12): retail loads the triangle** into ecx before the four
// index stores, keeps the -1 in edx and dereferences the pointer after the
// stores; ours loads it into eax after the stores and keeps the -1 in eax.
// The vtable's address cannot match in the reduced image
// @retail 0x2f7b20
triangle_terms_2f7b20 *__cdecl function_2f7b20(description_2ed170 const *description, triangle_2f7b20 const **other, dispatcher_2ed170 **dispatcher, long arg_3)
{
	return new triangle_terms_2f7b20(other, arg_3);
}

// score 12 (was 12): the same body as 0x2f7b20 and the same gap: retail
// loads the triangle** into ecx before the four index stores and keeps the -1
// in edx, ours loads it into eax after the stores and keeps the -1 in eax
// @retail 0x2f7bb0
swapped_triangle_terms_2f7b20 *__cdecl function_2f7bb0(triangle_2f7b20 const **description, typed_2ed170 **other, dispatcher_2ed170 **dispatcher, long arg_3)
{
	return new swapped_triangle_terms_2f7b20(description, arg_3);
}
