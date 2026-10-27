#ifndef CHILD_QUERY_2F9950_H
#define CHILD_QUERY_2F9950_H
// Havok (third-party physics library): one side of a query passed on as the
// child shape of a wrapping shape, at the same key and placement, with the
// wrapping side as its parent; and a collector of the nearest result.

struct input_2ea940;

// a shape that wraps a child shape
struct wrapper_2f9950
{
	char field_0[0xc];
	void const *child;
};

// one side of a query: a shape, the key of the part concerned, where it sits,
// and the side it is a part of
struct body_2ea940
{
	void const *shape;
	long key;
	void const *placement;
	body_2ea940 const *parent;

	body_2ea940() {}
	// the wrapped child of a side, in the same place
	body_2ea940(body_2ea940 const *parent_, void const *placement_, wrapper_2f9950 const *wrapper)
	{
		placement = placement_;
		parent = parent_;
		void const *child = wrapper->child;
		key = parent->key;
		shape = child;
	}
};

void __cdecl function_2ea940(body_2ea940 const *a, body_2ea940 const *b, input_2ea940 const *input, void *collector);

// a collector that starts with no result: the largest float as its distance
class nearest_2f9950
{
public:
	nearest_2f9950() : distance(3.40282347e+38f) {}
	virtual ~nearest_2f9950() {}

	float distance;
};

// a hit passed on seen from the other side (0x3018f0, unknown_3018f0.cpp):
// the same layout as the collector below, the collector it passes to at +8
struct hit_3018f0;
class flipped_3018f0
{
public:
	long field_0;
	float early_out;
	void *collector;

	void add(hit_3018f0 const *hit);
};

// the collector of 0x2f9950's query (retail vtable 0x412970): the query runs
// with its sides the other way round, so each hit is turned back (0x3018f0)
// and passed on to the collector given as the value
class nearest_value_2f9950 : public nearest_2f9950
{
public:
	nearest_value_2f9950(long value_) : value(value_) {}
	virtual void add(hit_3018f0 const *hit) { ((flipped_3018f0 *)this)->add(hit); }

	long value;
};

#endif
