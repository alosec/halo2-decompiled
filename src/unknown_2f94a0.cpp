// @flags /O2 /GL-
// Havok (third-party physics library): a query between the two sides of a
// pair run with the sides the other way round.
#include "pair_agents_2eab70.h"

static __declspec(align(16)) const unsigned long g_4124a0[4] = {0x80000000, 0x80000000, 0x80000000, 0x80000000};

// with the two sides the other way round, the normals of the hits it adds
// flipped back
// @retail 0x2f94a0
void class_2f90a0::function_2f94a0(void const *a, void const *b, void const *input, hit_collector_2f88a0 *collector)
{
	int i = collector->hits->size;
	function_2f90a0(b, a, input, collector);
	for (; i < collector->hits->size; i++)
	{
		__m128 *normal = &collector->hits->data[i].normal;
		*normal = _mm_xor_ps(*normal, *(const __m128 *)g_4124a0);
	}
}
