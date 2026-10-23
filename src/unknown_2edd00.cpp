// @flags /O2 /GL- /GX
// Havok (third-party physics library): the constructor of a list of child
// objects for a shape holding children: one object per child, made by the
// handler a table picks by the two shapes' types when the filter accepts the
// child, else the shared empty object.
#include "child_list_2edd00.h"

struct object_4805a4;
object_4805a4 *__cdecl function_301a40();

// @retail 0x2edd00
list_2eda30::list_2eda30(description_2edd00 const *description, typed_2edd00 **other, context_2edd00 const *context, long arg_3) :
	sized_2edd00(arg_3)
{
	child_description_2edd00 child;
	child.parent = description;
	child.field_8 = description->field_8;
	shape_2edd00 const *shape = description->shape;
	children.resize(shape->child_count());
	for (long i = 0; i < children.size; i++)
	{
		child.key = shape->entries[i].key;
		child.index = i;
		if (context->filter->accept(context, other, description, shape, i).value)
		{
			dispatcher_2edd00 *table = context->dispatcher;
			long first = ((typed_2edd00 *)child.key)->type();
			long second = (*other)->type();
			handler_2edd00 handler = table->handlers[first][second];
			child_2edd00 **slot = &children.data[i];
			*slot = handler(&child, other, context, arg_3);
		}
		else
		{
			child_2edd00 **slot = &children.data[i];
			*slot = (child_2edd00 *)function_301a40();
		}
	}
}
