// @flags /O2 /GL-
// Havok (third-party physics library): the island grouping's two helpers
// that port/recovered/havok_dispatch_372xxx.cpp inlines into 0x372200 (the
// grouping is include/joined_groups_3720a0.h's grouping_3720a0: a parent
// table where a root holds minus its group's size, and a scratch table of
// each pair's root):
//   0x3720c0  join: both bodies' roots found (the paths made to point at
//             them), the smaller root taking the other's group; returns the
//             root
//   0x372180  flatten: every parent and every scratch entry made the root
//             it leads to
//   0x372760  the list of groups' storage released (the inline
//             ~array_groups_372300 of the header; the unwind funclets jump
//             to it)
// Nothing calls them by address; they are here for completeness, matching
// the inlined copies.
#include "growable_array_2d91d0.h"

class grouping_3720c0
{
public:
	array_2d91d0 *parents;
	array_2d91d0 *scratch;
	long group_count;

	long join(long a, long b);
	void flatten();
};

// the root of `node`, every node on the way made to point at it
static __forceinline long find_3720c0(long *parents, long node)
{
	long root = node;
	while (parents[root] >= 0)
	{
		root = parents[root];
	}
	if (parents[node] >= 0)
	{
		do
		{
			long next = parents[node];
			parents[node] = root;
			node = next;
		} while (parents[node] >= 0);
	}
	return root;
}

// @retail 0x3720c0
long grouping_3720c0::join(long a, long b)
{
	long const root_a = find_3720c0((long *)parents->data, a);
	long const root_b = find_3720c0((long *)parents->data, b);
	if (root_a == root_b)
	{
		return root_b;
	}
	long *table = (long *)parents->data;
	if (root_a < root_b)
	{
		table[root_a] += table[root_b];
		table[root_b] = root_a;
		return root_a;
	}
	table[root_b] += table[root_a];
	table[root_a] = root_b;
	return root_b;
}

// @retail 0x372180
void grouping_3720c0::flatten()
{
	long *table = (long *)parents->data;
	long *end = table + parents->size;
	for (long *entry = table; entry != end; entry++)
	{
		if (*entry >= 0)
		{
			while (table[*entry] >= 0)
			{
				*entry = table[*entry];
			}
		}
	}
	long *pair_root = (long *)scratch->data;
	long *pair_end = pair_root + scratch->size;
	for (long *entry = pair_root; entry != pair_end; entry++)
	{
		while (table[*entry] >= 0)
		{
			*entry = table[*entry];
		}
	}
}

// the list of groups' storage given back (include/joined_groups_3720a0.h's
// ~array_groups_372300, inlined there): 12 bytes a group, tag 0x12, unless
// the list does not own it
struct groups_372760
{
	void *data;
	long size;
	long capacity_and_flags;

	void release();
};

// @retail 0x372760
void groups_372760::release()
{
	if (capacity_and_flags >= 0)
	{
		g_memory_2d88a0->release_block(data, (capacity_and_flags & 0x7fffffff) * 12, 0x12);
	}
}
