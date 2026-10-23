// @flags /O2 /GL-
// Havok (third-party physics library): merges two sorted runs of one axis's
// interval endpoints back into place, telling each object where its
// endpoints now sit.
#include "reference_counted_2d88a0.h"

// an array of longs whose storage comes from the allocator's stack area
// (src/unknown_2e18e0.cpp)
struct array_longs_2e1ab0
{
	long *data;
	long size;
	unsigned long capacity_and_flags;

	array_longs_2e1ab0() : data(0), size(0), capacity_and_flags(0x80000000) {}
	~array_longs_2e1ab0()
	{
		if ((capacity_and_flags & 0x80000000) == 0)
		{
			g_memory_2d88a0->release_block(data, (capacity_and_flags & 0x7fffffff) * sizeof(long), 0x12);
		}
	}
};

struct longs_2e1ab0 : array_longs_2e1ab0
{
	long *stack_block;
	long stack_capacity;

	longs_2e1ab0(long capacity);
	~longs_2e1ab0();
};

// one end of an object's interval on an axis: its position, whose low bit
// says whether it is the upper end, and the object's index
struct endpoint_2e2190
{
	unsigned short value;
	unsigned short object;
};

// where each axis's lower and upper endpoint index sits in an object's
// 16-byte record, in bytes
static const long g_endpoint_offsets_461ef0[6] = {8, 10, 0, 4, 2, 6};

// the endpoint at out, recorded in its object as sitting at its index
inline void record_endpoint_2e2190(endpoint_2e2190 const *out, endpoint_2e2190 const *endpoints, char *objects, long axis)
{
	long const index = out - endpoints;
	long const offset = g_endpoint_offsets_461ef0[axis * 2 + (out->value & 1)];
	*(unsigned short *)(objects + out->object * 16 + offset) = (unsigned short)index;
}

// the indices of the endpoints to remove
struct removed_2e2370
{
	unsigned short *indices;
	long count;
};

// each object's new index, by its old one
struct renumbering_2e2370
{
	unsigned short *new_index;
};

void __cdecl function_2d9160(void *array, long capacity, long element_size);
// the order the sort compares by, an empty object passed by value
struct order_2e01c0
{
};

void __cdecl function_2e01c0(unsigned short *entries, int low, int high, order_2e01c0 order);

class endpoint_list_2e2190
{
public:
	endpoint_2e2190 *endpoints;
	long size;
	unsigned long capacity_and_flags;

	void function_2e2190(char *objects, long first_count, long second_count, long axis);
	void function_2e2370(char *objects, removed_2e2370 *removed, long axis, renumbering_2e2370 const *renumbering);
};

// the first run (after the sentinel at the start) is copied aside, then the
// two runs are merged into the list
// @retail 0x2e2190
void endpoint_list_2e2190::function_2e2190(char *objects, long first_count, long second_count, long axis)
{
	longs_2e1ab0 first_copy(first_count);
	first_copy.size = first_count;
	for (long index = 0; index < first_count; index++)
	{
		first_copy.data[index] = ((long *)endpoints)[index];
	}
	endpoint_2e2190 const *first = (endpoint_2e2190 const *)first_copy.data + 1;
	endpoint_2e2190 const *second = endpoints + first_count;
	endpoint_2e2190 *out = endpoints + 1;
	endpoint_2e2190 const *first_end = (endpoint_2e2190 const *)first_copy.data + first_count;
	endpoint_2e2190 const *second_end = endpoints + first_count + second_count;
	while (first->value < second->value)
	{
		first++;
		out++;
	}
	while (first < first_end)
	{
		if (second >= second_end)
		{
			while (first < first_end)
			{
				*out = *first;
				record_endpoint_2e2190(out, endpoints, objects, axis);
				first++;
				out++;
			}
			break;
		}
		while (first->value < second->value && first < first_end)
		{
			*out = *first;
			record_endpoint_2e2190(out, endpoints, objects, axis);
			first++;
			out++;
		}
		while (first->value >= second->value && second < second_end)
		{
			*out = *second;
			record_endpoint_2e2190(out, endpoints, objects, axis);
			second++;
			out++;
		}
	}
	while (second < second_end)
	{
		*out = *second;
		record_endpoint_2e2190(out, endpoints, objects, axis);
		second++;
		out++;
	}
}

// the endpoints at the removed indices taken out, the rest moved down, each
// renumbered to its object's new index, and the objects told where their
// endpoints now sit
// @retail 0x2e2370
void endpoint_list_2e2190::function_2e2370(char *objects, removed_2e2370 *removed, long axis, renumbering_2e2370 const *renumbering)
{
	order_2e01c0 order = {};
	if (removed->count > 1)
	{
		function_2e01c0(removed->indices, 0, removed->count - 1, order);
	}
	long const removed_count = removed->count;
	long out = removed->indices[0];
	long const count = size;
	long next = 1;
	long index;
	for (index = 1; index < out; index++)
	{
		unsigned short *object = &endpoints[index].object;
		*object = renumbering->new_index[*object];
	}
	while (index < count)
	{
		long end;
		if (next < removed_count)
		{
			end = removed->indices[next];
			next++;
		}
		else
		{
			end = count;
		}
		for (index++; index < end; index++, out++)
		{
			endpoints[out] = endpoints[index];
			endpoint_2e2190 *endpoint = &endpoints[out];
			endpoint->object = renumbering->new_index[endpoint->object];
			long const offset = g_endpoint_offsets_461ef0[axis * 2 + (endpoint->value & 1)];
			*(unsigned short *)(objects + endpoint->object * 16 + offset) = (unsigned short)out;
		}
	}
	long capacity = capacity_and_flags & 0x7fffffff;
	if (capacity < out)
	{
		capacity *= 2;
		function_2d9160(this, out >= capacity ? out : capacity, sizeof(endpoint_2e2190));
	}
	size = out;
}
