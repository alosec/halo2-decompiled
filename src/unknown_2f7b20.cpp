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

class triangle_terms_2f7b20 : public sized_2ed170
{
public:
	float terms[4];
	// eight indices that start out unset
	unsigned short indices[8];

	__forceinline triangle_terms_2f7b20(triangle_2f7b20 const **triangle, long arg_3) :
		sized_2ed170(arg_3)
	{
		for (int i = 0; i < 8; i++)
		{
			indices[i] = 0xffff;
		}
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

// a triangle against another shape: the terms that solve for a point's
// weights in the triangle's plane, and eight indices that start unset
// @retail 0x2f7b20
triangle_terms_2f7b20 *__cdecl function_2f7b20(description_2ed170 const *description, triangle_2f7b20 const **other, dispatcher_2ed170 **dispatcher, long arg_3)
{
	return new triangle_terms_2f7b20(other, arg_3);
}

// the same with the two shapes the other way round
// @retail 0x2f7bb0
swapped_triangle_terms_2f7b20 *__cdecl function_2f7bb0(triangle_2f7b20 const **description, typed_2ed170 **other, dispatcher_2ed170 **dispatcher, long arg_3)
{
	return new swapped_triangle_terms_2f7b20(description, arg_3);
}
