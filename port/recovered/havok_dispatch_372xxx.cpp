// @flags /O2 /GL-
// Havok (third-party physics library): the island split's grouping of bodies
// (a union-find over a parent table: a root holds minus its group's size),
// the table of 64 pair handlers the world dispatches overlapping pairs
// through, and the sort that orders the pairs first.
//
//   0x3720a0  grouping_3720a0 ctor
//   0x3720c0  the grouping's join (find with path compression, union by
//             index: the smaller root wins)
//   0x372180  the grouping's flatten (every parent and scratch entry made a
//             root)
//   0x372200  grouping_3720a0::function_372200 (each body alone, then join
//             the pairs)
//   0x372390  grouping_3720a0::function_372390 (lists the groups)
//   0x3728b0  helper_3728b0 dtor (the empty handler back to its base vtable)
//   0x3728c0  dispatcher_3728c0::function_3728c0 (pairs begun and ended
//             handed to the handler their two objects' types pick)
//   0x372a40  slot_table_372a40 ctor
//   0x372a80  the pairs' quicksort
//   0x372b50  function_372b50 (each pair ordered, then the pairs sorted)
#include <xmmintrin.h>
#include "placed_shape_312800.h"
#include "joined_groups_3720a0.h"
#include "slot_table_372a40.h"

// ---------------------------------------------------------------------------
// the grouping

// the root of `node`, every node on the way made to point at it
static __forceinline long grouping_find_3720c0(long *parents, long node)
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

// joins the groups of `a` and `b` under the smaller root, the sizes added;
// returns that root
static long grouping_join_3720c0(grouping_3720a0 *grouping, long a, long b)
{
	long root_a = grouping_find_3720c0((long *)grouping->parents->data, a);
	long root_b = grouping_find_3720c0((long *)grouping->parents->data, b);
	if (root_a == root_b)
	{
		return root_a;
	}
	long *parents = (long *)grouping->parents->data;
	if (root_a < root_b)
	{
		parents[root_a] += parents[root_b];
		parents[root_b] = root_a;
		return root_a;
	}
	parents[root_b] += parents[root_a];
	parents[root_a] = root_b;
	return root_b;
}

// every parent entry and every scratch entry made the root it leads to
static void grouping_flatten_372180(grouping_3720a0 *grouping)
{
	long *parents = (long *)grouping->parents->data;
	long *end = parents + grouping->parents->size;
	for (long *entry = parents; entry != end; entry++)
	{
		if (*entry >= 0)
		{
			while (parents[*entry] >= 0)
			{
				*entry = parents[*entry];
			}
		}
	}
	long *scratch = (long *)grouping->scratch->data;
	long *scratch_end = scratch + grouping->scratch->size;
	for (long *entry = scratch; entry != scratch_end; entry++)
	{
		while (parents[*entry] >= 0)
		{
			*entry = parents[*entry];
		}
	}
}

// room for at least `count` elements, at least twice the old room
static __forceinline void ensure_capacity_372200(array_2d91d0 *array, long count, long element_size)
{
	long capacity = array->capacity_and_flags & 0x7fffffff;
	if (capacity < count)
	{
		capacity *= 2;
		function_2d9160(array, count >= capacity ? count : capacity, element_size);
	}
}

// @retail 0x3720a0
grouping_3720a0::grouping_3720a0(array_2d91d0 *parents, array_2d91d0 *scratch)
{
	this->parents = parents;
	this->scratch = scratch;
}

// @retail 0x372200
void grouping_3720a0::function_372200(array_2d91d0 const *pairs, long count)
{
	array_2d91d0 *table = parents;
	ensure_capacity_372200(table, count, 4);
	table->size = count;
	for (long i = 0; i < count; i++)
	{
		((long *)parents->data)[i] = -1;
	}
	scratch->size = 0;
	ensure_capacity_372200(scratch, pairs->size, 4);
	array_2d91d0 *joined = scratch;
	long pair_count = pairs->size;
	ensure_capacity_372200(joined, pair_count, 4);
	joined->size = pair_count;
	for (long i = 0; i < pairs->size; i++)
	{
		pair_30d870 const *pair = &((pair_30d870 const *)pairs->data)[i];
		long *slot = &((long *)scratch->data)[i];
		long first = pair->first;
		long second = pair->second;
		*slot = first < second ? grouping_join_3720c0(this, first, second)
			: grouping_join_3720c0(this, second, first);
	}
	grouping_flatten_372180(this);
	group_count = 0;
}

// each root starts a group (its parent entry then holds the group's index),
// each other body joins its root's group; both tables are emptied after
// @retail 0x372390
void grouping_3720a0::function_372390(groups_372300 *groups)
{
	for (long i = 0; i < parents->size; i++)
	{
		long *table = (long *)parents->data;
		if (table[i] < 0)
		{
			long size = groups->size + 1;
			ensure_capacity_372200((array_2d91d0 *)groups, size, sizeof(members_372300));
			groups->size = size;
			members_372300 *group = &groups->data[size - 1];
			long *members = (long *)g_memory_2d88a0->allocate(-((long *)parents->data)[i] * 4, 1);
			group->data = members;
			// the group's size; the third word counts the members listed
			group->size = -((long *)parents->data)[i];
			members[0] = i;
			group->capacity_and_flags = 1;
			((long *)parents->data)[i] = groups->size - 1;
		}
		else
		{
			members_372300 *group = &groups->data[table[table[i]]];
			group->data[group->capacity_and_flags] = i;
			group->capacity_and_flags++;
		}
	}
	// (retail runs the arrays' resize to 0, whose growth never happens)
	parents->size = 0;
	scratch->size = 0;
}

// ---------------------------------------------------------------------------
// the dispatcher

// what a pair's two words point at: the broad phase's handle of an object
// (0x10 into its collidable), with its id and its type
struct handle_3728c0
{
	long field_0;
	unsigned long id;
	long type;
};

// a handler in the dispatcher's table: slot 1 takes a pair that begins,
// slot 2 one that ends
class pair_handler_3728c0
{
public:
	virtual void slot_0() {}
	virtual void add_pair(pair_30d870 const *pair) {}
	virtual void remove_pair(pair_30d870 const *pair) {}
};

// the listener's slot 0: whether the two collidables may collide (an hkBool
// returned through its first argument)
class pair_filter_3728c0
{
public:
	virtual void is_enabled(bool *result, void const *a, void const *b) {}
};

static __forceinline handle_3728c0 *pair_first_3728c0(pair_30d870 const *pair)
{
	return (handle_3728c0 *)pair->first;
}

static __forceinline handle_3728c0 *pair_second_3728c0(pair_30d870 const *pair)
{
	return (handle_3728c0 *)pair->second;
}

static __forceinline pair_handler_3728c0 *pair_handler_for_3728c0(dispatcher_3728c0 *dispatcher, pair_30d870 const *pair)
{
	long index = pair_first_3728c0(pair)->type * 8 + pair_second_3728c0(pair)->type;
	return ((pair_handler_3728c0 **)dispatcher)[index];
}

// the collidable a handle sits in (0 for no handle)
static __forceinline void const *pair_collidable_3728c0(long handle)
{
	return handle ? (void const *)(handle - 0x10) : 0;
}

// a pair that begins, handed on when the listener lets the two collide
static __forceinline void pair_add_3728c0(dispatcher_3728c0 *dispatcher, pair_30d870 const *pair,
	pair_listener_315a10 *listener)
{
	void const *a = pair_collidable_3728c0(pair->first);
	void const *b = pair_collidable_3728c0(pair->second);
	bool enabled;
	((pair_filter_3728c0 *)listener)->is_enabled(&enabled, a, b);
	if (enabled)
	{
		pair_handler_for_3728c0(dispatcher, pair)->add_pair(pair);
	}
}

// both lists sorted (0x372b50), they are walked together: a pair in both is
// skipped, otherwise the smaller one is handed on first
// @retail 0x3728c0
void dispatcher_3728c0::function_3728c0(pair_30d870 const *added, long added_count, pair_30d870 const *removed,
	long removed_count, pair_listener_315a10 *listener)
{
	pair_30d870 const *added_end = added + added_count;
	pair_30d870 const *removed_end = removed + removed_count;
	if (added != added_end)
	{
		while (removed != removed_end)
		{
			unsigned long added_first = pair_first_3728c0(added)->id;
			unsigned long removed_first = pair_first_3728c0(removed)->id;
			if (removed_first == added_first
				&& pair_second_3728c0(removed)->id == pair_second_3728c0(added)->id)
			{
				added++;
				removed++;
			}
			else if (removed_first < added_first
				|| (removed_first == added_first
					&& pair_second_3728c0(removed)->id < pair_second_3728c0(added)->id))
			{
				pair_handler_for_3728c0(this, removed)->remove_pair(removed);
				removed++;
			}
			else
			{
				pair_add_3728c0(this, added, listener);
				added++;
			}
			if (added == added_end)
			{
				break;
			}
		}
	}
	while (added < added_end)
	{
		pair_add_3728c0(this, added, listener);
		added++;
	}
	while (removed < removed_end)
	{
		pair_handler_for_3728c0(this, removed)->remove_pair(removed);
		removed++;
	}
}

// ---------------------------------------------------------------------------
// the table's empty handler, and the table

class helper_3728b0
{
public:
	~helper_3728b0();
};

// the empty handler at 0x100 given its base's vtable back
// @retail 0x3728b0
helper_3728b0::~helper_3728b0()
{
	*(unsigned *)((char *)this + 0x100) = 0x41387c;
}

// every slot pointing at the empty handler after them (vtable 0x43e534,
// whose slots 1 and 2 do nothing)
// @retail 0x372a40
slot_table_372a40::slot_table_372a40()
{
	*(unsigned *)&empty = 0x43e534;
	for (int i = 0; i < 64; i++)
	{
		slots[i] = &empty;
	}
}

// ---------------------------------------------------------------------------
// the pairs' sort: by the first object's id, then the second's

static __forceinline bool pair_less_372a80(pair_30d870 const *a, pair_30d870 const *b)
{
	unsigned long a_first = pair_first_3728c0(a)->id;
	unsigned long b_first = pair_first_3728c0(b)->id;
	return a_first < b_first
		|| (a_first == b_first && pair_second_3728c0(a)->id < pair_second_3728c0(b)->id);
}

// sorts pairs[low..high]; the last argument (the comparison, an empty
// object) is passed on unused
// @retail 0x372a80
void __cdecl function_372a80(pair_30d870 *pairs, long low, long high, char compare)
{
	for (;;)
	{
		long i = low;
		long j = high;
		pair_30d870 pivot = pairs[(low + high) >> 1];
		do
		{
			while (pair_less_372a80(&pairs[i], &pivot))
			{
				i++;
			}
			while (pair_less_372a80(&pivot, &pairs[j]))
			{
				j--;
			}
			if (j < i)
			{
				break;
			}
			if (j != i)
			{
				pair_30d870 swap = pairs[j];
				pairs[j] = pairs[i];
				pairs[i] = swap;
			}
			j--;
			i++;
		} while (i <= j);
		if (low < j)
		{
			function_372a80(pairs, low, j, compare);
		}
		if (i >= high)
		{
			return;
		}
		low = i;
	}
}

// @retail 0x372b50
void __cdecl function_372b50(pair_30d870 *pairs, long count)
{
	if (count == 0)
	{
		return;
	}
	for (long i = 0; i < count; i++)
	{
		long second = pairs[i].second;
		long first = pairs[i].first;
		if (((handle_3728c0 *)first)->id > ((handle_3728c0 *)second)->id)
		{
			pairs[i].first = second;
			pairs[i].second = first;
		}
	}
	if (count > 1)
	{
		function_372a80(pairs, 0, count - 1, 0);
	}
}
