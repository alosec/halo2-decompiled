// @flags /O2 /GL- /GX
// Havok (third-party physics library): a query between the two sides of a
// pair run with the sides the other way round.
#include "pair_agents_2eab70.h"
#include "side_swap_collectors_2eb1d0.h"
#include "swapped_query_2ed2a0.h"

// with the two sides the other way round, the normals of the hits it adds
// flipped back
// @retail 0x2faa60
void class_2fa2f0::function_2faa60(void const *a, void const *b, void const *input, hit_collector_2f88a0 *collector)
{
	int i = collector->hits->size;
	function_2fa2f0(b, a, input, collector);
	for (; i < collector->hits->size; i++)
	{
		__m128 *normal = &collector->hits->data[i].normal;
		*normal = _mm_xor_ps(*normal, *(const __m128 *)g_4124a0);
	}
}
