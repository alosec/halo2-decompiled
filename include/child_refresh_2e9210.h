#ifndef CHILD_REFRESH_2E9210_H
#define CHILD_REFRESH_2E9210_H
// Havok (third-party physics library): what the object holding an array of
// 12-byte elements (0x2e9530) works with when it brings the objects it keeps
// for a shape's children up to date (0x2e9210): the shape is a wrapper whose
// collection hands out children by key, a filter decides which children are
// wanted, and a table picks the handler that makes a child's object.
#include <xmmintrin.h>

struct object_4805a4;
object_4805a4 *__cdecl function_301a40();

// room for a child shape the collection builds on request
struct child_buffer_2e9210
{
	__m128 storage[16];
};

struct typed_2e9210
{
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual long type() { return 0; }
};

// a shape holding children it hands out by key
struct collection_2e9210
{
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
	virtual void slot_10() {}
	virtual void slot_11() {}
	virtual void slot_12() {}
	virtual typed_2e9210 const *child(long key, child_buffer_2e9210 *buffer) const { return 0; }
};

// a shape that wraps another
struct wrapper_2e9210
{
	char unknown0[0xc];
	void const *inner;
};

struct description_2e9210
{
	wrapper_2e9210 const *shape;
	long field_4;
	long field_8;
	description_2e9210 const *parent;
};

struct child_description_2e9210
{
	long key;
	long index;
	long field_8;
	description_2e9210 const *parent;
};

struct context_2e9210;

typedef long (__cdecl *handler_2e9210)(typed_2e9210 **other, child_description_2e9210 const *child, context_2e9210 const *context, long arg_3);

struct dispatcher_2e9210
{
	char unknown0[0x18c];
	handler_2e9210 handlers[1][32];
};

struct bool_result_2e9210
{
	bool value;
};

struct filter_2e9210
{
	virtual bool_result_2e9210 accept(context_2e9210 const *context, typed_2e9210 **other, description_2e9210 const *parent, collection_2e9210 const *shape, long key) = 0;
};

struct context_2e9210
{
	dispatcher_2e9210 *dispatcher;
	filter_2e9210 *filter;
};

// a child's object: let go through its sixth slot, brought up to date through
// its eighth
struct kept_child_2e9210
{
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual void release() {}
	virtual void slot_6() {}
	virtual void refresh(typed_2e9210 **other, child_description_2e9210 const *child, context_2e9210 const *context) {}
};

#endif
