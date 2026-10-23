// @flags /O2 /GL-
// Havok (third-party physics library): passing queries on to a collector
// with the child's transform composed into the description's transform.
#include "unknown_182d90.h"
#include "pair_agents_2eab70.h"
#include <stddef.h>

typedef char assert_2ecc80_widths[(sizeof(void *) == 4 && sizeof(long) == 4) ? 1 : -1];

struct shape_2ecc80
{
	char field_0[0xc];
	long child;
	s_extent_transform transform;
};

struct vector_2ecc80
{
	__m128 value;

	void operator=(vector_2ecc80 const &other) { value = other.value; }
};

struct placement_2ecc80
{
	vector_2ecc80 field_0;
	vector_2ecc80 field_10;
	s_extent_transform transform;
};

struct description_2ecc80
{
	shape_2ecc80 const *shape;
	long field_4;
	placement_2ecc80 const *placement;
	description_2ecc80 const *parent;

	description_2ecc80() {}
	description_2ecc80(description_2ecc80 const *parent_, placement_2ecc80 const *placement_, shape_2ecc80 const *outer)
	{
		parent = parent_;
		placement = placement_;
		shape_2ecc80 const *inner = (shape_2ecc80 const *)outer->child;
		field_4 = parent->field_4;
		shape = inner;
	}
};

class collector_2ecc80
{
public:
	virtual void slot0() = 0;
	virtual void slot1(description_2ecc80 const *description, long a, long b, long c) = 0;
	virtual void slot2(description_2ecc80 const *description, long a, long b, long c) = 0;
	virtual void slot3(description_2ecc80 const *description, long arg_1, long arg_2, long arg_3, long arg_4) = 0;
	virtual void slot4(description_2ecc80 const *description, long arg_1, long arg_2, long arg_3) = 0;
	virtual void slot5() = 0;
	virtual void slot6() = 0;
	virtual void slot7(description_2ecc80 const *description, long a, long b) = 0;
};

class class_2ecc80
{
public:
	char field_0[0xc];
	collector_2ecc80 *collector;

	void function_2ecc80(description_2ecc80 const *description, long arg_1, long arg_2, long arg_3);
	void function_2ecd00(description_2ecc80 const *description, long arg_1, long arg_2, long arg_3, long arg_4);
};

typedef char assert_description_2ecc80_size[(sizeof(description_2ecc80) == 0x10) ? 1 : -1];
typedef char assert_class_2ecc80_collector[(offsetof(class_2ecc80, collector) == 0xc) ? 1 : -1];
typedef char assert_class_2ece10_collector[(offsetof(class_2ece10, collector) == 0xc) ? 1 : -1];
typedef char assert_class_2ecf20_collector[(offsetof(class_2ecf20, collector) == 0xc) ? 1 : -1];
typedef char assert_class_2ed030_collector[(offsetof(class_2ed030, collector) == 0xc) ? 1 : -1];

// @retail 0x2ecc80
void class_2ecc80::function_2ecc80(description_2ecc80 const *description, long arg_1, long arg_2, long arg_3)
{
	shape_2ecc80 const *shape = description->shape;
	placement_2ecc80 placement;
	placement.field_0 = description->placement->field_0;
	placement.field_10 = description->placement->field_10;
	// Retail reloads the placement pointer for composition after the bounds copy.
	placement.transform.compose(&(*(placement_2ecc80 const *const volatile *)&description->placement)->transform, &shape->transform);
	collector_2ecc80 *target = collector;
	description_2ecc80 child(description, &placement, shape);
	target->slot4(&child, arg_1, arg_2, arg_3);
}

// @retail 0x2ecd00
void class_2ecc80::function_2ecd00(description_2ecc80 const *description, long arg_1, long arg_2, long arg_3, long arg_4)
{
	shape_2ecc80 const *shape = description->shape;
	placement_2ecc80 placement;
	placement.field_0 = description->placement->field_0;
	placement.field_10 = description->placement->field_10;
	// Retail reloads the placement pointer for composition after the bounds copy.
	placement.transform.compose(&(*(placement_2ecc80 const *const volatile *)&description->placement)->transform, &shape->transform);
	collector_2ecc80 *target = collector;
	description_2ecc80 child(description, &placement, shape);
	target->slot3(&child, arg_1, arg_2, arg_3, arg_4);
}

class typed_2ecd80
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual long type() { return 0; }
};

typedef void (__cdecl *handler_2ecd80)(description_2ecc80 const *description, typed_2ecd80 **other, void *dispatcher, long arg_3, long arg_4);

struct dispatcher_2ecd80
{
	char field_0[0x318c];
	// Retail 0x2e5990 initializes 32 rows of 32 four-byte entries.
	handler_2ecd80 handlers[32][32];
};

typedef char assert_dispatcher_2ecd80_offset[(offsetof(dispatcher_2ecd80, handlers) == 0x318c) ? 1 : -1];
typedef char assert_dispatcher_2ecd80_row[(sizeof(((dispatcher_2ecd80 *)0)->handlers[0]) == 0x80) ? 1 : -1];
typedef char assert_dispatcher_2ecd80_table[(sizeof(((dispatcher_2ecd80 *)0)->handlers) == 0x1000) ? 1 : -1];

// passing a pair on to the handler a table picks by the two shapes' types
// @retail 0x2ecd80
void __cdecl function_2ecd80(description_2ecc80 const *description, typed_2ecd80 **other, dispatcher_2ecd80 **dispatcher, long arg_3, long arg_4)
{
	shape_2ecc80 const *shape = description->shape;
	placement_2ecc80 placement;
	placement.field_0 = description->placement->field_0;
	placement.field_10 = description->placement->field_10;
	// Retail reads the placement pointer again for composition after copying
	// the two vectors; keep this read separate from those bounds reads.
	placement.transform.compose(&(*(placement_2ecc80 const *const volatile *)&description->placement)->transform, &shape->transform);
	description_2ecc80 child(description, &placement, shape);
	long first = ((typed_2ecd80 *)child.shape)->type();
	long second = (*other)->type();
	(*dispatcher)->handlers[first][second](&child, other, dispatcher, arg_3, arg_4);
}

// @retail 0x2ece10
void class_2ece10::function_2ece10(void const *a, void const *b, void const *input, void *out)
{
	description_2ecc80 const *description = (description_2ecc80 const *)a;
	shape_2ecc80 const *shape = description->shape;
	placement_2ecc80 placement;
	placement.field_0 = description->placement->field_0;
	placement.field_10 = description->placement->field_10;
	// Retail reloads the placement pointer for composition after the bounds copy.
	placement.transform.compose(&(*(placement_2ecc80 const *const volatile *)&description->placement)->transform, &shape->transform);
	collector_2ecc80 *target = collector;
	description_2ecc80 child(description, &placement, shape);
	target->slot2(&child, (long)b, (long)input, (long)out);
}

// selects the pair handler after composing the wrapper's child placement
struct dispatcher_2ece90
{
	char field_0[0x218c];
	typedef void (__cdecl *handler)(description_2ecc80 const *description, typed_2ecd80 **other, void const *input, void *out);
	// Retail 0x2e5990 initializes 32 rows of 32 four-byte entries.
	handler handlers[32][32];
};

typedef char assert_dispatcher_2ece90_offset[(offsetof(dispatcher_2ece90, handlers) == 0x218c) ? 1 : -1];
typedef char assert_dispatcher_2ece90_row[(sizeof(((dispatcher_2ece90 *)0)->handlers[0]) == 0x80) ? 1 : -1];
typedef char assert_dispatcher_2ece90_table[(sizeof(((dispatcher_2ece90 *)0)->handlers) == 0x1000) ? 1 : -1];

// @retail 0x2ece90
void __cdecl function_2ece90(void const *a, void const *b, void const *input, void *out)
{
	description_2ecc80 const *description = (description_2ecc80 const *)a;
	typed_2ecd80 **other = (typed_2ecd80 **)b;
	dispatcher_2ece90 **dispatcher = (dispatcher_2ece90 **)input;
	shape_2ecc80 const *shape = description->shape;
	placement_2ecc80 placement;
	placement.field_0 = description->placement->field_0;
	placement.field_10 = description->placement->field_10;
	// Retail reloads the placement pointer for composition after the bounds copy.
	placement.transform.compose(&(*(placement_2ecc80 const *const volatile *)&description->placement)->transform, &shape->transform);
	description_2ecc80 child(description, &placement, shape);
	long first = ((typed_2ecd80 *)child.shape)->type();
	long second = (*other)->type();
	(*dispatcher)->handlers[first][second](&child, other, input, out);
}

// @retail 0x2ecf20
void class_2ecf20::function_2ecf20(void const *a, void const *b, void const *input, void *out)
{
	description_2ecc80 const *description = (description_2ecc80 const *)a;
	shape_2ecc80 const *shape = description->shape;
	placement_2ecc80 placement;
	placement.field_0 = description->placement->field_0;
	placement.field_10 = description->placement->field_10;
	// Retail reloads the placement pointer for composition after the bounds copy.
	placement.transform.compose(&(*(placement_2ecc80 const *const volatile *)&description->placement)->transform, &shape->transform);
	collector_2ecc80 *target = collector;
	description_2ecc80 child(description, &placement, shape);
	target->slot1(&child, (long)b, (long)input, (long)out);
}

// selects the pair handler after composing the wrapper's child placement
struct dispatcher_2ecfa0
{
	char field_0[0x118c];
	typedef void (__cdecl *handler)(description_2ecc80 const *description, typed_2ecd80 **other, void const *input, void *out);
	// Retail 0x2e5990 initializes 32 rows of 32 four-byte entries.
	handler handlers[32][32];
};

typedef char assert_dispatcher_2ecfa0_offset[(offsetof(dispatcher_2ecfa0, handlers) == 0x118c) ? 1 : -1];
typedef char assert_dispatcher_2ecfa0_row[(sizeof(((dispatcher_2ecfa0 *)0)->handlers[0]) == 0x80) ? 1 : -1];
typedef char assert_dispatcher_2ecfa0_table[(sizeof(((dispatcher_2ecfa0 *)0)->handlers) == 0x1000) ? 1 : -1];

// @retail 0x2ecfa0
void __cdecl function_2ecfa0(void const *a, void const *b, void const *input, void *out)
{
	description_2ecc80 const *description = (description_2ecc80 const *)a;
	typed_2ecd80 **other = (typed_2ecd80 **)b;
	dispatcher_2ecfa0 **dispatcher = (dispatcher_2ecfa0 **)input;
	shape_2ecc80 const *shape = description->shape;
	placement_2ecc80 placement;
	placement.field_0 = description->placement->field_0;
	placement.field_10 = description->placement->field_10;
	// Retail reloads the placement pointer for composition after the bounds copy.
	placement.transform.compose(&(*(placement_2ecc80 const *const volatile *)&description->placement)->transform, &shape->transform);
	description_2ecc80 child(description, &placement, shape);
	long first = ((typed_2ecd80 *)child.shape)->type();
	long second = (*other)->type();
	(*dispatcher)->handlers[first][second](&child, other, input, out);
}

// @retail 0x2ed030
void class_2ed030::function_2ed030(void const *a, void const *b, void *out)
{
	description_2ecc80 const *description = (description_2ecc80 const *)a;
	shape_2ecc80 const *shape = description->shape;
	placement_2ecc80 placement;
	placement.field_0 = description->placement->field_0;
	placement.field_10 = description->placement->field_10;
	// Retail reloads the placement pointer for composition after the bounds copy.
	placement.transform.compose(&(*(placement_2ecc80 const *const volatile *)&description->placement)->transform, &shape->transform);
	collector_2ecc80 *target = collector;
	description_2ecc80 child(description, &placement, shape);
	target->slot7(&child, (long)b, (long)out);
}
