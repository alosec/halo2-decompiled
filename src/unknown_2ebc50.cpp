// @flags /O2 /GL- /GX
// Havok (third-party physics library): an object holding one listed object
// per part of a container: telling every listed object, then destroying
// itself through the first virtual slot; passing a query on to the listed
// object of every part; and passing a query on to a function chosen by the
// two kinds involved, for every part of a container that a filter accepts.
// Each query is bracketed by two marks in a timing stream.

#include "timer_mark_2ebc80.h"

class shape_2ebc80;

// one side of a query: a shape, the key of the part concerned, a value
// passed on to parts, and the side it is a part of
struct body_2ebc80
{
	shape_2ebc80 const *shape;
	long key;
	long field_8;
	body_2ebc80 const *parent;
};

class shape_2ebc80
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	// the shape's kind
	virtual long kind() const { return 0; }
	virtual void slot_6() {}
	virtual void slot_7() {}
	virtual void slot_8() {}
	virtual void slot_9() {}
	virtual void slot_10() {}
	// a container's part keys, ending with -1
	virtual long first_key() const { return -1; }
	virtual long next_key(long key) const { return -1; }
	// a container's part, built in the buffer given when it must be
	virtual shape_2ebc80 const *part(long key, char *buffer) const { return 0; }
};

// the two-byte answer of a filter, returned through a hidden pointer
class answer_2ebe40
{
public:
	answer_2ebe40() {}

	char value;
};

struct input_2ebe40;

// the filter a query's input holds: whether one part of a container may meet
// the other side
class filter_2ebe40
{
public:
	virtual answer_2ebe40 accepts(input_2ebe40 const *input, body_2ebc80 const *other, body_2ebc80 const *container_body, shape_2ebc80 const *container, long key) const = 0;
};

struct collector_2ebc80;

typedef void (__cdecl *pair_query_2ec200)(body_2ebc80 const *a, body_2ebc80 const *b, input_2ebe40 const *input, collector_2ebc80 *collector);
typedef void (__cdecl *pair_query_2ebe40)(body_2ebc80 const *a, body_2ebc80 const *b, input_2ebe40 const *input, void *collector);
typedef void (__cdecl *pair_query_2ec020)(body_2ebc80 const *a, body_2ebc80 const *b, input_2ebe40 const *input, void *arg_3, void *arg_4);

// per-kind flags, then three tables of query functions indexed by the kinds
// of the two sides
struct dispatcher_2ebe40
{
	char unknown0[0x118c];
	pair_query_2ec200 queries_118c[32][32];
	pair_query_2ebe40 queries_218c[32][32];
	pair_query_2ec020 queries_318c[32][32];
};

// a query's input: the dispatcher and the filter
struct input_2ebe40
{
	dispatcher_2ebe40 *dispatcher;
	filter_2ebe40 *filter;
};

// what a query fills in; a nonzero byte at +4 ends it early
struct collector_2ebc80
{
	long field_0;
	bool early_out;
};

class class_2ebc50_object
{
public:
	virtual void slot0() = 0;
	virtual void slot1(body_2ebc80 const *body, void const *arg_1, void const *arg_2, collector_2ebc80 *collector) = 0;
	virtual void slot2(body_2ebc80 const *body, void const *arg_1, void const *arg_2, void *arg_3) = 0;
	virtual void slot3(body_2ebc80 const *body, void const *arg_1, void const *arg_2, void *arg_3, void *arg_4) = 0;
	virtual void slot4(body_2ebc80 const *body, void const *arg_1, void const *arg_2, void *arg_3) = 0;
	virtual void slot5() = 0;
};

struct s_2ebc50_entry
{
	long field_0;
	class_2ebc50_object *object;
};

class class_2ebc50
{
public:
	virtual void slot0(int flags) = 0;

	long field_4;
	long field_8;
	s_2ebc50_entry *entries;
	int count;

	void function_2ebc50();
	void function_2ebc80(body_2ebc80 const *body, void const *arg_1, void const *arg_2, void *arg_3);
	void function_2ebd60(body_2ebc80 const *body, void const *arg_1, void const *arg_2, void *arg_3);
	void function_2ebf50(body_2ebc80 const *body, void const *arg_1, void const *arg_2, void *arg_3, void *arg_4);
	void function_2ec130(body_2ebc80 const *body, void const *arg_1, void const *arg_2, collector_2ebc80 *collector);
};

// @retail 0x2ebc50
void class_2ebc50::function_2ebc50()
{
	for (int i = 0; i < count; i++)
	{
		entries[i].object->slot5();
	}
	slot0(1);
}

// @retail 0x2ebc80
void class_2ebc50::function_2ebc80(body_2ebc80 const *body, void const *arg_1, void const *arg_2, void *arg_3)
{
	timer_add_2ebc80("TtShapeCollection");
	__declspec(align(16)) char buffer[0x100];
	shape_2ebc80 const *container = body->shape;
	s_2ebc50_entry const *entry = entries;
	body_2ebc80 part;
	part.parent = body;
	part.field_8 = body->field_8;
	for (int i = count - 1; i >= 0; i--)
	{
		long const key = entry->field_0;
		part.shape = container->part(key, buffer);
		part.key = key;
		entry->object->slot4(&part, arg_1, arg_2, arg_3);
		entry++;
	}
	timer_add_2ebc80("Et");
}

// @retail 0x2ebd60
void class_2ebc50::function_2ebd60(body_2ebc80 const *body, void const *arg_1, void const *arg_2, void *arg_3)
{
	timer_add_2ebc80("TtShapeCollection");
	__declspec(align(16)) char buffer[0x100];
	shape_2ebc80 const *container = body->shape;
	s_2ebc50_entry const *entry = entries;
	body_2ebc80 part;
	part.parent = body;
	part.field_8 = body->field_8;
	for (int i = count - 1; i >= 0; i--)
	{
		long const key = entry->field_0;
		part.shape = container->part(key, buffer);
		part.key = key;
		entry->object->slot2(&part, arg_1, arg_2, arg_3);
		entry++;
	}
	timer_add_2ebc80("Et");
}

// every part of a container the filter accepts, passed with the other side
// to the query function for the two kinds
// @retail 0x2ebe40
void __cdecl function_2ebe40(body_2ebc80 const *a, body_2ebc80 const *b, input_2ebe40 const *input, void *collector)
{
	timer_add_2ebc80("TtShapeCollection");
	__declspec(align(16)) char buffer[0x100];
	body_2ebc80 part;
	part.field_8 = a->field_8;
	shape_2ebc80 const *container = a->shape;
	part.parent = a;
	long const kind_b = b->shape->kind();
	for (long key = container->first_key(); key != -1; key = container->next_key(key))
	{
		if (input->filter->accepts(input, b, a, container, key).value)
		{
			part.shape = container->part(key, buffer);
			part.key = key;
			input->dispatcher->queries_218c[part.shape->kind()][kind_b](&part, b, input, collector);
		}
	}
	timer_add_2ebc80("Et");
}

// as 0x2ebc80, but retail never steps past the first entry, so the first
// part is queried count times
// @retail 0x2ebf50
void class_2ebc50::function_2ebf50(body_2ebc80 const *body, void const *arg_1, void const *arg_2, void *arg_3, void *arg_4)
{
	timer_add_2ebc80("TtShapeCollection");
	__declspec(align(16)) char buffer[0x100];
	shape_2ebc80 const *container = body->shape;
	s_2ebc50_entry const *entry = entries;
	body_2ebc80 part;
	part.parent = body;
	part.field_8 = body->field_8;
	for (int i = count - 1; i >= 0; i--)
	{
		long const key = entry->field_0;
		part.shape = container->part(key, buffer);
		part.key = key;
		entry->object->slot3(&part, arg_1, arg_2, arg_3, arg_4);
	}
	timer_add_2ebc80("Et");
}

// @retail 0x2ec020
void __cdecl function_2ec020(body_2ebc80 const *a, body_2ebc80 const *b, input_2ebe40 const *input, void *arg_3, void *arg_4)
{
	timer_add_2ebc80("TtShapeCollection");
	__declspec(align(16)) char buffer[0x100];
	body_2ebc80 part;
	part.field_8 = a->field_8;
	shape_2ebc80 const *container = a->shape;
	part.parent = a;
	long const kind_b = b->shape->kind();
	for (long key = container->first_key(); key != -1; key = container->next_key(key))
	{
		if (input->filter->accepts(input, b, a, container, key).value)
		{
			part.shape = container->part(key, buffer);
			part.key = key;
			input->dispatcher->queries_318c[part.shape->kind()][kind_b](&part, b, input, arg_3, arg_4);
		}
	}
	timer_add_2ebc80("Et");
}

// as 0x2ebc80, stopping once the collector says so
// @retail 0x2ec130
void class_2ebc50::function_2ec130(body_2ebc80 const *body, void const *arg_1, void const *arg_2, collector_2ebc80 *collector)
{
	timer_add_2ebc80("TtShapeCollection");
	__declspec(align(16)) char buffer[0x100];
	shape_2ebc80 const *container = body->shape;
	s_2ebc50_entry const *entry = entries;
	body_2ebc80 part;
	part.parent = body;
	part.field_8 = body->field_8;
	for (int i = count - 1; i >= 0; i--)
	{
		long const key = entry->field_0;
		part.shape = container->part(key, buffer);
		part.key = key;
		entry->object->slot1(&part, arg_1, arg_2, collector);
		if (collector->early_out)
		{
			break;
		}
		entry++;
	}
	timer_add_2ebc80("Et");
}

// as 0x2ebe40, stopping once the collector says so
// @retail 0x2ec200
void __cdecl function_2ec200(body_2ebc80 const *a, body_2ebc80 const *b, input_2ebe40 const *input, collector_2ebc80 *collector)
{
	timer_add_2ebc80("TtShapeCollection");
	__declspec(align(16)) char buffer[0x100];
	body_2ebc80 part;
	part.field_8 = a->field_8;
	shape_2ebc80 const *container = a->shape;
	part.parent = a;
	long const kind_b = b->shape->kind();
	for (long key = container->first_key(); key != -1; key = container->next_key(key))
	{
		if (input->filter->accepts(input, b, a, container, key).value)
		{
			part.shape = container->part(key, buffer);
			part.key = key;
			input->dispatcher->queries_118c[part.shape->kind()][kind_b](&part, b, input, collector);
			if (collector->early_out)
			{
				break;
			}
		}
	}
	timer_add_2ebc80("Et");
}

// a collector that stops a query early once its flag is set
class stopping_collector_2ec310
{
public:
	stopping_collector_2ec310() : early_out(false) {}
	virtual ~stopping_collector_2ec310() {}

	bool early_out;
};

// passes each pair it is given on to another collector with the two sides
// swapped back
class swapped_collector_2ec310 : public stopping_collector_2ec310
{
public:
	swapped_collector_2ec310(collector_2ebc80 *target) : target(target) {}
	virtual ~swapped_collector_2ec310() {}
	virtual void add_pair(long first, long second) {}

	collector_2ebc80 *target;
};

// a collector that keeps the nearest result, starting from the largest float
class nearest_collector_2ec370
{
public:
	nearest_collector_2ec370() : distance(3.40282347e+38f) {}
	virtual ~nearest_collector_2ec370() {}

	float distance;
};

// passes each hit it is given on to another collector as seen from the
// other side
class flipped_collector_2ec370 : public nearest_collector_2ec370
{
public:
	flipped_collector_2ec370(void *target) : target(target) {}
	virtual ~flipped_collector_2ec370() {}
	virtual void add_hit(void const *hit) {}

	void *target;
};

// 0x2ec200 with the two sides given the other way round
// @retail 0x2ec310
void __cdecl function_2ec310(body_2ebc80 const *b, body_2ebc80 const *a, input_2ebe40 const *input, collector_2ebc80 *collector)
{
	swapped_collector_2ec310 swapped(collector);
	function_2ec200(a, b, input, (collector_2ebc80 *)&swapped);
}

// 0x2ebe40 with the two sides given the other way round
// @retail 0x2ec370
void __cdecl function_2ec370(body_2ebc80 const *b, body_2ebc80 const *a, input_2ebe40 const *input, void *collector)
{
	flipped_collector_2ec370 flipped(collector);
	function_2ebe40(a, b, input, &flipped);
}
