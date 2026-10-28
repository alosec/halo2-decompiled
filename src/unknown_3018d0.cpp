// @flags /O2 /GL-
// Havok (third-party physics library): a pair passed on to the collector at
// +8 with its two bodies swapped.
#include "lane_b_flip_collectors.h"

// @retail 0x3018d0
void lane_b_flip_body_pair_collector::add(body_2ea940 const *a, body_2ea940 const *b)
{
	collector->add(b, a);
}
