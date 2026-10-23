// @flags /O2 /GL-
// Havok (third-party physics library): a list of child objects that passes a
// query on to each child, with a description of the child (its key from the
// parent's table, its index, and the parent's description).

#include <stddef.h>

struct key_entry_2eda30
{
	long key;
	long value;
};

struct shape_2eda30
{
	char unknown0[0xc];
	key_entry_2eda30 *entries;
};

struct description_2eda30
{
	shape_2eda30 *shape;
	long field_4;
	long field_8;
};

struct child_description_2eda30
{
	long key;
	long index;
	long field_8;
	description_2eda30 const *parent;
};

struct collector_2edb60
{
	long field_0;
	bool done;
};

class child_2eda30
{
public:
	virtual void slot_0() {}
	virtual void query_1(child_description_2eda30 const *child, long arg_1, long arg_2, collector_2edb60 *collector) {}
	virtual void query_2(child_description_2eda30 const *child, long arg_1, long arg_2, long arg_3) {}
	virtual void query_3(child_description_2eda30 const *child, long arg_1, long arg_2, long arg_3, long arg_4) {}
	virtual void query_4(child_description_2eda30 const *child, long arg_1, long arg_2, long arg_3) {}
	virtual void release() {}
	virtual void slot_6() {}
	virtual void update(child_description_2eda30 const *child, struct typed_2edbd0 **other, struct context_2edbd0 const *context) {}
};

struct object_4805a4;
object_4805a4 *__cdecl function_301a40();

struct typed_2edbd0
{
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual long type() { return 0; }
};

typedef child_2eda30 *(__cdecl *handler_2edbd0)(child_description_2eda30 const *child, typed_2edbd0 **other, context_2edbd0 const *context, long arg_3);

struct dispatcher_2edbd0
{
	char unknown0[0x18c];
	// Retail 0x2e5990 initializes 32 rows of 32 four-byte entries.
	handler_2edbd0 handlers[32][32];
};

typedef char assert_dispatcher_2edbd0_offset[(offsetof(dispatcher_2edbd0, handlers) == 0x18c) ? 1 : -1];
typedef char assert_dispatcher_2edbd0_row[(sizeof(((dispatcher_2edbd0 *)0)->handlers[0]) == 0x80) ? 1 : -1];
typedef char assert_dispatcher_2edbd0_table[(sizeof(((dispatcher_2edbd0 *)0)->handlers) == 0x1000) ? 1 : -1];

struct bool_result_2edbd0
{
	bool value;

	bool_result_2edbd0(bool value_)
	{
		value = value_;
	}
};

class filter_2edbd0
{
public:
	virtual bool_result_2edbd0 accept(context_2edbd0 const *context, typed_2edbd0 **other, description_2eda30 const *parent, shape_2eda30 const *shape, long index) = 0;
};

struct context_2edbd0
{
	dispatcher_2edbd0 *dispatcher;
	filter_2edbd0 *filter;
};

class list_2eda30
{
public:
	char unknown0[8];
	long field_8;
	child_2eda30 **children;
	long count;

	void function_2eda30(description_2eda30 const *description, long arg_1, long arg_2, long arg_3);
	void function_2eda90(description_2eda30 const *description, long arg_1, long arg_2, long arg_3);
	void function_2edaf0(description_2eda30 const *description, long arg_1, long arg_2, long arg_3, long arg_4);
	void function_2edb60(description_2eda30 const *description, long arg_1, long arg_2, collector_2edb60 *collector);
	void function_2edbd0(description_2eda30 const *description, typed_2edbd0 **other, context_2edbd0 const *context);
};

// @retail 0x2eda30
void list_2eda30::function_2eda30(description_2eda30 const *description, long arg_1, long arg_2, long arg_3)
{
	child_description_2eda30 child;
	shape_2eda30 const *shape = description->shape;
	child.parent = description;
	child.field_8 = description->field_8;
	long n = count;
	child_2eda30 **list = children;
	for (long i = 0; i < n; i++)
	{
		child.key = shape->entries[i].key;
		child.index = i;
		(*list)->query_4(&child, arg_1, arg_2, arg_3);
		list++;
	}
}

// @retail 0x2eda90
void list_2eda30::function_2eda90(description_2eda30 const *description, long arg_1, long arg_2, long arg_3)
{
	child_description_2eda30 child;
	shape_2eda30 const *shape = description->shape;
	child.parent = description;
	child.field_8 = description->field_8;
	long n = count;
	child_2eda30 **list = children;
	for (long i = 0; i < n; i++)
	{
		child.key = shape->entries[i].key;
		child.index = i;
		(*list)->query_2(&child, arg_1, arg_2, arg_3);
		list++;
	}
}

// @retail 0x2edaf0
void list_2eda30::function_2edaf0(description_2eda30 const *description, long arg_1, long arg_2, long arg_3, long arg_4)
{
	child_description_2eda30 child;
	shape_2eda30 const *shape = description->shape;
	child.parent = description;
	child.field_8 = description->field_8;
	long n = count;
	child_2eda30 **list = children;
	for (long i = 0; i < n; i++)
	{
		child.key = shape->entries[i].key;
		child.index = i;
		(*list)->query_3(&child, arg_1, arg_2, arg_3, arg_4);
		list++;
	}
}

// @retail 0x2edb60
void list_2eda30::function_2edb60(description_2eda30 const *description, long arg_1, long arg_2, collector_2edb60 *collector)
{
	child_description_2eda30 child;
	shape_2eda30 const *shape = description->shape;
	child.parent = description;
	child.field_8 = description->field_8;
	for (long i = 0; i < count; i++)
	{
		child.key = shape->entries[i].key;
		child.index = i;
		children[i]->query_1(&child, arg_1, arg_2, collector);
		if (collector->done)
		{
			break;
		}
	}
}

// keeps one child object for each child the filter accepts, made by the
// handler a table picks by the two shapes' types, and lets go of the rest
// @retail 0x2edbd0
void list_2eda30::function_2edbd0(description_2eda30 const *description, typed_2edbd0 **other, context_2edbd0 const *context)
{
	child_description_2eda30 child;
	shape_2eda30 const *shape = description->shape;
	child.parent = description;
	child.field_8 = description->field_8;
	for (long i = 0; i < count; i++)
	{
		child.key = shape->entries[i].key;
		child.index = i;
		if (context->filter->accept(context, other, description, shape, i).value)
		{
			child_2eda30 *agent = children[i];
			if ((object_4805a4 *)agent == function_301a40())
			{
				long arg_3 = field_8;
				dispatcher_2edbd0 *table = context->dispatcher;
				long first = ((typed_2edbd0 *)child.key)->type();
				long second = (*other)->type();
				handler_2edbd0 handler = table->handlers[first][second];
				child_2eda30 **slot = &children[i];
				*slot = handler(&child, other, context, arg_3);
			}
			else
			{
				agent->update(&child, other, context);
			}
		}
		else
		{
			child_2eda30 *agent = children[i];
			if ((object_4805a4 *)agent != function_301a40())
			{
				agent->release();
				child_2eda30 **slot = &children[i];
				*slot = (child_2eda30 *)function_301a40();
			}
		}
	}
}
