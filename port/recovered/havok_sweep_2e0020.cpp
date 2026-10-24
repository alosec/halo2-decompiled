// @flags /O2 /GL- /GX
// Havok (third-party physics library): two methods of the three-axis sweep
// (axis_sweep_2e1090.h):
//   0x2e0020  a box's bounds back from its quantized endpoints
//   0x2e2f70  the pairs a batch of boxes makes (or made) with every other
//             box: the batch's lower x endpoints are sorted, and one walk
//             along the x axis keeps the boxes open at each of them; each
//             batch box is tested on y and z against the boxes already open
//             at its lower end (once for a pair of batch boxes) and those
//             whose lower ends fall inside it. A pair with a grid cell adds
//             the box to (or takes it from) the cell's list instead.
#include "sweep_batches_2e50d0.h"
#include "sweep_search_2e1090.h"

// sorts the entries from low to high (both included) by key
// (src/unknown_2dfec0.cpp)
void __cdecl function_2dff20(entry_2dfec0 *entries, int low, int high, long context);

// the corners come back from the endpoints' values, each divided by the
// axis's scale (1.0f / scale, the 1.0f at 0x45dbc0) and moved back by the
// lower offset; the fourth words are 0 before the offset
// @retail 0x2e0020
void class_2e1090::function_2e0020(element_2e1090 const *element, bounds_2e0020 *bounds)
{
	float const *scale_words = (float const *)&scale;
	float const *offset = (float const *)&lower_offset;
	float const inverse_x = 1.0f / scale_words[0];
	float const inverse_y = 1.0f / scale_words[1];
	float const inverse_z = 1.0f / scale_words[2];

	long const lower_x = axes[0].data[element->slots[4]].value;
	long const lower_y = axes[1].data[element->slots[0]].value;
	long const lower_z = axes[2].data[element->slots[1]].value;
	long const upper_x = axes[0].data[element->slots[5]].value;
	long const upper_y = axes[1].data[element->slots[2]].value;
	long const upper_z = axes[2].data[element->slots[3]].value;

	float *lower = (float *)&bounds->lower;
	lower[0] = (float)lower_x * inverse_x - offset[0];
	lower[1] = (float)lower_y * inverse_y - offset[1];
	lower[2] = (float)lower_z * inverse_z - offset[2];
	lower[3] = 0.0f - offset[3];

	float *upper = (float *)&bounds->upper;
	upper[0] = (float)upper_x * inverse_x - offset[0];
	upper[1] = (float)upper_y * inverse_y - offset[1];
	upper[2] = (float)upper_z * inverse_z - offset[2];
	upper[3] = 0.0f - offset[3];
}

// the endpoint's value word: bit 0 set for an upper end; in the list of open
// boxes bit 1 marks a box the walk opened (one to test against), clear for a
// batch box opened by hand (already tested against what follows it)
static long const upper_end_2e2f70 = 1;
static long const walked_2e2f70 = 2;

static __forceinline long pack_2e2f70(endpoint_2e0f80 const &end)
{
	return (long)((unsigned long)end.value | ((unsigned long)end.id << 16));
}

static __forceinline unsigned short id_2e2f70(long packed)
{
	return (unsigned short)((unsigned long)packed >> 16);
}

// a pair of the batch box (by its index and element) with the box `other`:
// reported when they meet on y and z
static __forceinline void pair_2e2f70(class_2e1090 *sweep, unsigned short index, element_2e1090 const *box,
	element_2e1090 const *other, pairs_2e1090 *pairs, pair_change_2e2f70 change)
{
	if (apart_yz_2e1090(*box, *other) != 0)
	{
		return;
	}
	long const owner = other->owner;
	if ((owner & 1) == 0)
	{
		if (pairs->size == (pairs->capacity_and_flags & 0x7fffffff))
		{
			function_2d91d0(pairs, sizeof(pair_2e1090));
		}
		pair_2e1090 &pair = pairs->data[pairs->size];
		pair.first = (long *)box->owner;
		pair.second = owner;
		pairs->size++;
		return;
	}
	cell_2e1090 *cell = (cell_2e1090 *)((char *)sweep->cells + (owner & ~1));
	if (change.starting)
	{
		if (cell->boxes.size == (cell->boxes.capacity_and_flags & 0x7fffffff))
		{
			function_2d91d0(&cell->boxes, sizeof(unsigned short));
		}
		cell->boxes.data[cell->boxes.size] = index;
		cell->boxes.size++;
	}
	else
	{
		long found = 0;
		for (; found < cell->boxes.size; found++)
		{
			if (cell->boxes.data[found] == index)
			{
				break;
			}
		}
		if (found == cell->boxes.size)
		{
			// retail goes on with -1 (writing the slot before the list)
			found = -1;
		}
		cell->boxes.size--;
		cell->boxes.data[found] = cell->boxes.data[cell->boxes.size];
	}
}

// @retail 0x2e2f70
void sweep_batches_2e50d0::function_2e2f70(endpoint_2e0f80 *endpoints, long count, pairs_2e1090 *pairs, pair_change_2e2f70 change)
{
	if (count > 1)
	{
		function_2dff20((entry_2dfec0 *)endpoints, 0, count - 1, 0);
	}
	// the open boxes' endpoints, each packed as a dword (value in the low
	// word, the box's index in the high one)
	longs_2e1ab0 open_list(0x100);
	long cursor = 1;
	for (long k = 0; k < count; k++)
	{
		unsigned short const index = endpoints[k].id;
		element_2e1090 const *box = &elements.data[index];
		long own_lower = pack_2e2f70(axes[0].data[box->slots[4]]);

		// open and close the boxes the walk passes on its way to this box's
		// lower end
		while (cursor != box->slots[4])
		{
			long const end = pack_2e2f70(axes[0].data[cursor]);
			if ((end & upper_end_2e2f70) == 0)
			{
				if (open_list.size == (long)(open_list.capacity_and_flags & 0x7fffffff))
				{
					function_2d91d0(&open_list, sizeof(long));
				}
				open_list.data[open_list.size] = end | walked_2e2f70;
				open_list.size++;
			}
			else
			{
				// retail searches without a bound: the box is open
				long found = 0;
				while (id_2e2f70(open_list.data[found]) != id_2e2f70(end))
				{
					found++;
				}
				open_list.size--;
				open_list.data[found] = open_list.data[open_list.size];
			}
			cursor++;
		}

		// the boxes open here, but for batch boxes, which met this one in
		// their own forward pass
		long const open_count = open_list.size;
		for (long i = 0; i < open_count; i++)
		{
			if ((open_list.data[i] & walked_2e2f70) != 0)
			{
				pair_2e2f70(this, index, box, &elements.data[id_2e2f70(open_list.data[i])], pairs, change);
			}
		}

		// the boxes whose lower ends fall inside this one on x
		cursor++;
		for (long slot = cursor; slot < box->slots[5]; slot++)
		{
			endpoint_2e0f80 const &end = axes[0].data[slot];
			if ((end.value & upper_end_2e2f70) == 0)
			{
				pair_2e2f70(this, index, box, &elements.data[end.id], pairs, change);
			}
		}

		if (open_list.size == (long)(open_list.capacity_and_flags & 0x7fffffff))
		{
			function_2d91d0(&open_list, sizeof(long));
		}
		open_list.data[open_list.size] = own_lower & ~(long)walked_2e2f70;
		open_list.size++;
	}
}
