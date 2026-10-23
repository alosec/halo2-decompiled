// @flags /O2 /GL- /GX
// Havok (third-party physics library): the constructor of a list of child
// objects for a shape that hands out its children by key: one object per
// child the filter accepts and the handler a table picks by the two shapes'
// types makes, kept with the child's key.
#include "keyed_list_2ec680.h"

// @retail 0x2ec680
keyed_list_2ec680::keyed_list_2ec680(description_2edd00 const *description, typed_2edd00 **other, context_2edd00 const *context, long arg_3) :
	sized_2edd00(arg_3)
{
	child_description_2edd00 child;
	child_buffer_2ec680 buffer;
	child.parent = description;
	child.field_8 = description->field_8;
	collection_2ec680 const *shape = (collection_2ec680 const *)description->shape;
	long count = shape->child_count();
	children.ensure_capacity(count);
	long key = shape->first_key();
	for (long i = 0; i < count; i++)
	{
		child.key = (long)shape->child(key, &buffer);
		child.index = key;
		if (context->filter->accept(context, other, description, (shape_2edd00 const *)shape, key).value)
		{
			dispatcher_2edd00 *table = context->dispatcher;
			long first = ((typed_2edd00 *)child.key)->type();
			long second = (*other)->type();
			handler_2edd00 handler = table->handlers[first][second];
			child_2edd00 *made = handler(&child, other, context, arg_3);
			if (made)
			{
				keyed_child_2ec680 &slot = children.next_slot();
				slot.child = made;
				slot.key = key;
			}
		}
		key = shape->next_key(key);
	}
}
