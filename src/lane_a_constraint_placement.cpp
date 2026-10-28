// @flags /O2 /GL- /GX
#include "lane_b_constraint_leaves.h"
#include <new>
// Nullable placement construction of the existing contact aggregate.
// @retail 0x31c4b0
void __cdecl lane_a_construct_constraint_aggregate(void *storage)
{
	new (storage) lane_b_constraint_aggregate;
}
