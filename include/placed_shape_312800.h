#ifndef PLACED_SHAPE_312800_H
#define PLACED_SHAPE_312800_H
// Havok (third-party physics library): an object over 0x30b340's that keeps
// the objects whose bounds overlap its own (0x315a10 brings them up to date),
// and one placed by a transform, with its bounds (built from a description,
// 0x312800; it fills a description and is set from one, 0x3126b0, 0x312790).
#include "world_object_30b340.h"
#include "description_312430.h"
#include "stack_array_30d740.h"

// the smallest and largest corners of a box
struct bounds_312800
{
	__m128 minimum;
	__m128 maximum;

	// both corners zero, their last lanes very large
	void clear_312800()
	{
		minimum = _mm_setzero_ps();
		maximum = _mm_setzero_ps();
		minimum.m128_f32[3] = 1e30f;
		maximum.m128_f32[3] = 1e30f;
	}
};

// a growable array of pointers, freed when destroyed
struct pointers_312800
{
	void **data;
	long size;
	long capacity_and_flags;

	pointers_312800() : data(0), size(0), capacity_and_flags(0x80000000) {}
	~pointers_312800()
	{
		if ((capacity_and_flags & 0x80000000) == 0)
		{
			g_memory_2d88a0->release_block(data, (capacity_and_flags & 0x7fffffff) * sizeof(void *), 0x12);
		}
	}
};

// what these objects reach through their world: the broad phase (its slot 5
// brings an object's overlaps up to date with new bounds), the tolerance
// the bounds are grown by, what is told of overlaps that begin or end, and
// the room for the pairs that do
class pair_finder_315a10
{
public:
	virtual void slot_0() {}
	// adds an object, giving the pairs it begins
	virtual void slot_1(void *handle, bounds_312800 const *bounds, pairs_30d870 *added) {}
	virtual void slot_2() {}
	// removes an object, giving the pairs it ends
	virtual void slot_3(void *handle, pairs_30d870 *removed) {}
	virtual void slot_4() {}
	virtual void update_bounds(void **handles, bounds_312800 const *bounds, long count,
		pairs_30d870 *added, pairs_30d870 *removed) {}
};

class pair_listener_315a10
{
public:
	virtual void slot_0() {}
};

// a reference-counted object that is told of the pairs through its second
// base
class pair_listening_315a10 : public referenced_2d88a0, public pair_listener_315a10
{
};

// what the dispatcher hands the pairs to while it runs
struct pair_handler_3173f0
{
	void const *table;
};

class dispatcher_3728c0
{
public:
	char unknown0[0x24];
	pair_handler_3173f0 *handler;
	char unknown28[0x100 - 0x28];
	// the dispatcher's own handler, put back when a caller is done
	pair_handler_3173f0 own_handler;

	void function_3728c0(pair_30d870 const *added, long added_count, pair_30d870 const *removed,
		long removed_count, pair_listener_315a10 *listener);
};

struct tolerance_312590
{
	char unknown0[8];
	float value;
};

class overlapped_312800;
ELEMENTS_2D91D0(objects_30e4b0, overlapped_312800 *);

struct world_312590
{
	char unknown0[0xc4];
	pair_finder_315a10 *pair_finder;
	char unknownc8[0xcc - 0xc8];
	tolerance_312590 *tolerance;
	pair_listening_315a10 *pair_listener;
	char unknownd4[0x120 - 0xd4];
	// the objects added to the world (0x30e4b0, 0x30e5e0)
	objects_30e4b0 objects;
	char unknown12c[0x130 - 0x12c];
	long next_id;
	char unknown134[0x138 - 0x134];
	dispatcher_3728c0 *dispatcher;
	char unknown13c[0x234 - 0x13c];
	long pair_capacity_234;
	long pair_capacity;
};

// the object's shape and what follows it, at 0xc; the broad phase's handle
// for the object sits 0x10 into it
struct collidable_head_315a10
{
	referenced_2d88a0 *shape;
	long field_4;
	long field_8;
	long field_c;
};

struct handle_315a10
{
	long field_0;
};

struct collidable_315a10 : collidable_head_315a10, handle_315a10
{
};

// orders each pair by the second words of the two objects, then the pairs
void __cdecl function_372b50(pair_30d870 *pairs, long count);

class overlapped_312800 : public class_30b340
{
public:
	overlapped_312800(referenced_2d88a0 *shape, long value)
		: class_30b340(shape, value)
	{
		list.owner = this;
	}

	pointers_312800 field_3c;
	pointers_312800 field_48;

	// brings the pairs of overlapping objects up to date with new bounds
	void function_315a10(bounds_312800 const *bounds);
};

// the shape the object holds: its slot 5 gives what the object keeps at 0x54,
// its slot 6 its bounds under a transform, grown by a tolerance
class shape_312800 : public referenced_2d88a0
{
public:
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual void *slot_5() { return 0; }
	virtual void get_bounds(transform_312630 const &transform, float tolerance, bounds_312800 *bounds) const {}
};

class placed_shape_312800 : public overlapped_312800
{
public:
	placed_shape_312800(description_315ba0 const *description);

	void *field_54;
	bounds_312800 bounds;
	transform_312630 transform;

	// fills the description from the object
	void function_3126b0(description_315ba0 *description) const;
	void function_312760(bounds_312800 *bounds_out) const;
	// sets the object from the description
	void function_312790(description_315ba0 const *description);
	// gives the object another shape, its bounds brought up to date
	void function_30b780(shape_312800 *shape);
};

// a description with bounds, zero when built (0x312310)
struct c_contact_query_bounds_info : description_315b80
{
	c_contact_query_bounds_info();

	bounds_312800 bounds;
};

// a description with a transform (0x30b6a0)
struct c_contact_query_transform_info : description_315ba0
{
	c_contact_query_transform_info();
};

// the object placed by a transform with an array of its own, empty when
// built (0x30bb50)
class c_contact_query_transform_volume : public placed_shape_312800
{
public:
	c_contact_query_transform_volume(c_contact_query_transform_info const *info);
	// out of line (0x30bb90), as include/contact_volume_30bb50.h declares it
	virtual ~c_contact_query_transform_volume();

	REFERENCED_NEW_2D88A0(0x2c)

	pointers_312800 bodies;
};

// an object over the same base with bounds of its own, set from a
// description (0x30aff0) or directly (0x30b080)
class bounded_30aff0 : public overlapped_312800
{
public:
	void *field_54;
	bounds_312800 bounds;
};

#endif
