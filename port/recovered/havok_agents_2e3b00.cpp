// @flags /O2 /GL- /GX
// Havok (third-party physics library): three virtual methods reached through
// bound vtables:
//   0x2e3b00  the three-axis sweep (vtable 0x412600, slot 6) re-sorts its
//             boxes by their lower x endpoint, so the boxes' order in memory
//             follows the x axis, and renumbers everything that names a box
//   0x2e90b0  an agent holding a list of child agents (slot 5 of the
//             agent tables at 0x412a98, 0x412adc, 0x412b08, 0x412f60 and
//             0x412fb0): cleans up each child, then deletes itself
//   0x312760  an object placed by a transform (slot 6 of vtables 0x41382c
//             and 0x4144fc): its shape's bounds under its transform, grown
//             by the world's collision tolerance
#include <xmmintrin.h>
#include "reference_counted_2d88a0.h"
#include "growable_array_2d91d0.h"
#include "axis_sweep_2e1090.h"
#include "bounded_2e9530.h"
#include "placed_shape_312800.h"

// ---------------------------------------------------------------------------
// recovered code this file calls, declared as src declares it

// the arrays whose storage comes from the allocator's stack area
// (src/unknown_2e18e0.cpp, which stamps them out with these macros)
#define ARRAY_2E1850(name, T) \
	struct name \
	{ \
		T *data; \
		long size; \
		unsigned long capacity_and_flags; \
		name() : data(0), size(0), capacity_and_flags(0x80000000) {} \
		~name() \
		{ \
			if ((capacity_and_flags & 0x80000000) == 0) \
			{ \
				g_memory_2d88a0->release_block(data, (capacity_and_flags & 0x7fffffff) * sizeof(T), 0x12); \
			} \
		} \
	};

#define STACK_ARRAY_2E1850(name, base, T) \
	struct name : base \
	{ \
		T *stack_block; \
		long stack_capacity; \
		name(long capacity); \
		~name(); \
	};

ARRAY_2E1850(array_vectors_2e1980, __m128)
// 16-byte elements (0x2e1980, destroyed by 0x2e1a10)
STACK_ARRAY_2E1850(vectors_2e1980, array_vectors_2e1980, __m128)
ARRAY_2E1850(array_longs_2e1ab0, long)
// 4-byte elements (0x2e1ab0, destroyed by 0x2e1b40)
STACK_ARRAY_2E1850(longs_2e1ab0, array_longs_2e1ab0, long)

// a key and the index it came from (src/unknown_2e0120.cpp)
struct entry_2e0250
{
	unsigned short key;
	unsigned short value;
};

// sorts the entries from low to high (both included) by key
// (src/unknown_2e0120.cpp)
void __cdecl function_2e02d0(entry_2e0250 *entries, int low, int high, long context);

// ---------------------------------------------------------------------------
// 0x2e3b00

class sweep_2e3b00 : public class_2e1090
{
public:
	void sort_boxes_by_x();
};

// Box 0 stays first; the rest are ordered by the value of their lower x
// endpoint. Then each box's owner is told its new index (a grid cell keeps it
// as a word among the cells, any other owner as a word it points at), and the
// cells' box lists and the three axes' endpoints are renumbered.
// @retail 0x2e3b00
void sweep_2e3b00::sort_boxes_by_x()
{
	long count = elements.size;
	vectors_2e1980 saved(count);
	saved.size = count;
	longs_2e1ab0 order(count);
	order.size = count;
	longs_2e1ab0 moved_to(count);
	moved_to.size = count;

	element_2e1090 *saved_boxes = (element_2e1090 *)saved.data;
	entry_2e0250 *entries = (entry_2e0250 *)order.data;
	for (long i = 0; i < elements.size; i++)
	{
		saved_boxes[i] = elements[i];
		entries[i].key = axes[0].data[elements[i].slots[4]].value;
		entries[i].value = (unsigned short)i;
	}

	long last = order.size - 1;
	if (last > 1)
	{
		function_2e02d0(entries + 1, 0, last - 1, 0);
	}

	for (long i = 0; i < elements.size; i++)
	{
		unsigned short from = entries[i].value;
		moved_to.data[from] = i;
		elements[i] = saved_boxes[from];
	}

	for (long i = 1; i < elements.size; i++)
	{
		long owner = elements[i].owner;
		if ((owner & 1) == 0)
		{
			*(long *)owner = i;
		}
		else
		{
			*(unsigned short *)((char *)cells + (owner & ~1)) = (unsigned short)i;
		}
	}

	for (long cell = 0; cell < cells_used; cell++)
	{
		shorts_2e1090 &boxes = cells[cell].boxes;
		for (long j = boxes.size - 1; j >= 0; j--)
		{
			boxes.data[j] = (unsigned short)moved_to.data[boxes.data[j]];
		}
	}

	for (int axis = 0; axis < 3; axis++)
	{
		endpoints_2e0f80 &endpoints = axes[axis];
		for (long k = 0; k < endpoints.size; k++)
		{
			endpoints.data[k].id = (unsigned short)moved_to.data[endpoints.data[k].id];
		}
	}
}

// ---------------------------------------------------------------------------
// 0x2e90b0

// a child agent: its slot 5 cleans it up (hkCollisionAgent::cleanup)
class agent_2e90b0
{
public:
	virtual void slot_0() = 0;
	virtual void slot_1() = 0;
	virtual void slot_2() = 0;
	virtual void slot_3() = 0;
	virtual void slot_4() = 0;
	virtual void cleanup() = 0;
};

// the agent over bounded_2e9530's layout: the 12-byte elements hold a child
// agent in their third word
class agent_list_2e90b0 : public bounded_2e9530
{
public:
	void cleanup_children();
};

// @retail 0x2e90b0
void agent_list_2e90b0::cleanup_children()
{
	element_2e9530 *element = elements.data;
	element_2e9530 *end = elements.data + elements.size;
	for (; element != end; element++)
	{
		agent_2e90b0 *child = (agent_2e90b0 *)element->value_8;
		if (child)
		{
			child->cleanup();
		}
	}
	delete (counted_2e9530 *)this;
}

// ---------------------------------------------------------------------------
// 0x312760

class placed_shape_312760 : public placed_shape_312800
{
public:
	void calculate_bounds(bounds_312800 *out) const;
};

// @retail 0x312760
void placed_shape_312760::calculate_bounds(bounds_312800 *out) const
{
	world_312590 const *world = (world_312590 const *)field_8;
	((shape_312800 const *)object)->get_bounds(transform, world->tolerance->value, out);
}
