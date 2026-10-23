#ifndef PAIR_AGENT_2ED170_H
#define PAIR_AGENT_2ED170_H
// Havok (third-party physics library): an object created for a pair of
// shapes, which moves the first's placement into world space and keeps what
// the handler a table picks by the two shapes' types returns.
#include "reference_counted_2d88a0.h"
#include "unknown_182d90.h"

class typed_2ed170
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual long type() { return 0; }
};

struct shape_2ed170
{
	char field_0[0xc];
	typed_2ed170 *child;
	s_extent_transform transform;
};

struct placement_2ed170
{
	__m128 field_0;
	__m128 field_10;
	s_extent_transform transform;

	placement_2ed170(placement_2ed170 const &other)
	{
		field_0 = other.field_0;
		field_10 = other.field_10;
		transform.rows[0] = other.transform.rows[0];
		transform.rows[1] = other.transform.rows[1];
		transform.rows[2] = other.transform.rows[2];
		transform.rows[3] = other.transform.rows[3];
	}
};

struct description_2ed170
{
	shape_2ed170 const *shape;
	long field_4;
	placement_2ed170 const *placement;
	description_2ed170 const *parent;

	description_2ed170(description_2ed170 const *parent_, placement_2ed170 const *placement_, shape_2ed170 const *shape_)
	{
		placement = placement_;
		parent = parent_;
		field_4 = parent->field_4;
		shape = shape_;
	}
};

typedef long (__cdecl *handler_2ed170)(description_2ed170 const *description, typed_2ed170 **other, void *dispatcher, long arg_3);

struct dispatcher_2ed170
{
	char field_0[0x18c];
	handler_2ed170 handlers[1][32];
};

// sized by its allocator, but with no reference count of its own
class sized_2ed170
{
public:
	REFERENCED_NEW_2D88A0(0x1a)

	unsigned short memory_size;
	unsigned short reference_count;
	long field_8;

	sized_2ed170(long field_8_) : field_8(field_8_) {}
	virtual ~sized_2ed170() {}
};

class pair_agent_2ed170 : public sized_2ed170
{
public:
	long result;

	__forceinline pair_agent_2ed170(description_2ed170 const *description, typed_2ed170 **other, dispatcher_2ed170 **dispatcher, long arg_3) :
		sized_2ed170(arg_3)
	{
		shape_2ed170 const *shape = description->shape;
		typed_2ed170 *inner_shape = shape->child;
		placement_2ed170 placement(*description->placement);
		placement.transform.compose(&description->placement->transform, &shape->transform);
		description_2ed170 child(description, &placement, (shape_2ed170 const *)inner_shape);
		dispatcher_2ed170 *table = *dispatcher;
		long first = inner_shape->type();
		long second = (*other)->type();
		handler_2ed170 handler = table->handlers[first][second];
		result = handler(&child, other, dispatcher, arg_3);
	}
};

// the same with the two shapes the other way round
class swapped_pair_agent_2ed170 : public pair_agent_2ed170
{
public:
	__forceinline swapped_pair_agent_2ed170(typed_2ed170 **other, description_2ed170 const *description, dispatcher_2ed170 **dispatcher, long arg_3) :
		pair_agent_2ed170(description, other, dispatcher, arg_3)
	{
	}
};

#endif
