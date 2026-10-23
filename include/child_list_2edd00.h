#ifndef CHILD_LIST_2EDD00_H
#define CHILD_LIST_2EDD00_H
// Havok (third-party physics library): a list of child objects for a shape
// holding children (constructor 0x2edd00) and the types it reads.
#include "growable_array_2d91d0.h"

struct key_entry_2edd00
{
	long key;
	long value;
};

class shape_2edd00
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual void slot_5() {}
	virtual void slot_6() {}
	virtual void slot_7() {}
	virtual void slot_8() {}
	virtual void slot_9() {}
	virtual long child_count() const { return 0; }

	long field_4;
	long field_8;
	key_entry_2edd00 *entries;
};

struct description_2edd00
{
	shape_2edd00 const *shape;
	long field_4;
	long field_8;
};

struct child_description_2edd00
{
	long key;
	long index;
	long field_8;
	description_2edd00 const *parent;
};

struct typed_2edd00
{
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual long type() { return 0; }
};

class child_2edd00;
struct context_2edd00;

typedef child_2edd00 *(__cdecl *handler_2edd00)(child_description_2edd00 const *child, typed_2edd00 **other, context_2edd00 const *context, long arg_3);

struct dispatcher_2edd00
{
	char unknown0[0x18c];
	handler_2edd00 handlers[1][32];
};

struct bool_result_2edd00
{
	bool value;
};

class filter_2edd00
{
public:
	virtual bool_result_2edd00 accept(context_2edd00 const *context, typed_2edd00 **other, description_2edd00 const *parent, shape_2edd00 const *shape, long index) = 0;
};

struct context_2edd00
{
	dispatcher_2edd00 *dispatcher;
	filter_2edd00 *filter;
};

// an array that starts out in the room for four children held inside it
struct children_2edd00
{
	ELEMENTS_2D91D0_MEMBERS(child_2edd00 *)
	child_2edd00 *embedded[4];

	children_2edd00()
	{
		data = embedded;
		size = 0;
		capacity_and_flags = 0x80000004;
	}
	~children_2edd00()
	{
		if ((capacity_and_flags & 0x80000000) == 0)
		{
			g_memory_2d88a0->release_block(data, (capacity_and_flags & 0x7fffffff) * sizeof(child_2edd00 *), 0x12);
		}
	}
};

// sized by its allocator, but with no reference count of its own
class sized_2edd00
{
public:
	REFERENCED_NEW_2D88A0(0x1a)

	unsigned short memory_size;
	unsigned short reference_count;
	long field_8;

	sized_2edd00(long field_8_) : field_8(field_8_) {}
	virtual ~sized_2edd00() {}
};

class list_2eda30 : public sized_2edd00
{
public:
	children_2edd00 children;

	list_2eda30(description_2edd00 const *description, typed_2edd00 **other, context_2edd00 const *context, long arg_3);
};

#endif
