#ifndef SIDE_BOUNDS_2E9140_H
#define SIDE_BOUNDS_2E9140_H
// The bounds of one side of a pair query (a shape, a key, a placement and a
// parent side) taken into the other side's frame and widened by the query's
// tolerance; 0x2e9140 also stretches them along the query's path.
#include <xmmintrin.h>

// a box given by its two corners
struct bounds_2e9140
{
	__m128 minimum;
	__m128 maximum;
};

void __cdecl function_2e9140(void const *a, void const *b, void const *input, bounds_2e9140 *bounds);
void __cdecl function_2e91c0(void const *a, void const *b, void const *input, bounds_2e9140 *bounds);

#endif
