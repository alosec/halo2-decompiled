// @flags /O2 /GL- /GX
// Havok (third-party physics library): a three-axis sweep of boxes
// (axis_sweep_2e1090.h): the endpoints of every box a box overlaps, in three
// sorted lists, one per axis.
#include "sweep_search_2e1090.h"
#include "reference_counted_2d88a0.h"
#include <new>

// a list of endpoints over storage it does not own
struct endpoint_list_2e3370
{
	endpoint_2e0f80 *data;
	long size;
	unsigned long capacity_and_flags;

	endpoint_list_2e3370(endpoint_2e0f80 *storage, long capacity) : data(storage), size(0), capacity_and_flags(capacity | 0x80000000) {}

	void append(endpoint_2e0f80 const &endpoint)
	{
		data[size] = endpoint;
		size++;
	}
};

// the three lists, and the storage that follows them
struct overlap_endpoints_2e3370
{
	endpoint_list_2e3370 axes[3];
	endpoint_2e0f80 storage[1];
};

// the endpoints of every box the bounds overlap (not the grid cells'), one
// list per axis, each sorted between a sentinel at each end
// @retail 0x2e3370
void class_2e1090::function_2e3370(bounds_2e0020 const *bounds, overlap_endpoints_2e3370 *lists)
{
	longs_2e1ab0 found(elements.size);
	search_boxes_2e1090(this, *bounds, 0, 0, &found, SEARCH_COLLECT_BOXES_2E1090);

	// two endpoints for each box, and a sentinel at each end
	{
		endpoint_2e0f80 *storage = lists->storage;
		long const capacity = found.size * 2 + 2;
		for (int axis = 0; axis < 3; axis++)
		{
			new (&lists->axes[axis]) endpoint_list_2e3370(storage + axis * capacity, capacity);
		}
	}
	endpoint_2e0f80 sentinel;
	sentinel.value = 0;
	sentinel.id = 0;
	lists->axes[0].append(sentinel);
	lists->axes[1].append(sentinel);
	lists->axes[2].append(sentinel);
	element_2e1090 **kept = (element_2e1090 **)found.data;
	for (long remaining = found.size - 1; remaining >= 0; remaining--)
	{
		lists->axes[0].append(axes[0].data[(*kept)->slots[4]]);
		lists->axes[0].append(axes[0].data[(*kept)->slots[5]]);
		lists->axes[1].append(axes[1].data[(*kept)->slots[0]]);
		lists->axes[1].append(axes[1].data[(*kept)->slots[2]]);
		lists->axes[2].append(axes[2].data[(*kept)->slots[1]]);
		lists->axes[2].append(axes[2].data[(*kept)->slots[3]]);
		kept++;
	}
	endpoint_list_2e3370 *list = lists->axes;
	for (int axis = 3; axis != 0; axis--)
	{
		sort_2e0250((entry_2e0250 *)(list->data + 1), list->size - 1);
		list++;
	}
	sentinel.value = 0xfffc;
	sentinel.id = 0;
	lists->axes[0].append(sentinel);
	lists->axes[1].append(sentinel);
	lists->axes[2].append(sentinel);
}
