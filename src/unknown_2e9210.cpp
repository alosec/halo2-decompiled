// @flags /O2 /GL- /GX
// Havok (third-party physics library): bringing the objects kept for a
// wrapped collection's children up to date, with the two sides the other way
// round: a new object for each newly accepted child, the kept object told
// about each child still accepted, and the object of each child no longer
// accepted let go and replaced by the empty one. Then sorting keys, and the
// array of 12-byte elements whose storage comes from the stack area.
#include "bounded_2e9530.h"
#include "child_refresh_2e9210.h"

// score 0 (was 54): Frame agrees; call and result-store ordering differs. Workspace
// order 54; retained child 56; field order 58; both 60; input order 58;
// input with child 60, with field 62, with both 64.
// @retail 0x2e9210
void bounded_2e9530::function_2e9210(typed_2e9210 **other, description_2e9210 const *description, context_2e9210 const *context)
{
	child_buffer_2e9210 buffer;
	child_description_2e9210 child;
	collection_2e9210 const *shape = (collection_2e9210 const *)description->shape->inner;
	for (long i = 0; i < elements.size; i++)
	{
		typed_2e9210 const *element_part = shape->child(elements.data[i].value_0, &buffer);
		child.parent = description;
		child.field_8 = description->field_8;
		child.index = elements.data[i].value_0;
		child.key = (long)element_part;
		if (context->filter->accept(context, other, description, shape, elements.data[i].value_0).value)
		{
			kept_child_2e9210 *agent = (kept_child_2e9210 *)elements.data[i].value_8;
			if ((object_4805a4 *)agent == function_301a40())
			{
				long arg_3 = value;
				dispatcher_2e9210 *table = context->dispatcher;
				long first = (*other)->type();
				long second = ((typed_2e9210 *)child.key)->type();
				handler_2e9210 handler = table->handlers[first][second];
				element_2e9530 *destination = &elements.data[i];
				destination->value_8 = handler(other, &child, context, arg_3);
			}
			else
			{
				agent->refresh(other, &child, context);
			}
		}
		else
		{
			kept_child_2e9210 *agent = (kept_child_2e9210 *)elements.data[i].value_8;
			if ((object_4805a4 *)agent != function_301a40())
			{
				agent->release();
				element_2e9530 *destination = &elements.data[i];
				destination->value_8 = (long)function_301a40();
			}
		}
	}
}

// @retail 0x2e9360
void __cdecl function_2e9360(unsigned long *keys, int low, int high, key_order_2e9360 order)
{
	int j = high;
	int i = low;
	unsigned long pivot = keys[(low + high) >> 1];
	do
	{
		while (keys[i] < pivot)
		{
			i++;
		}
		while (pivot < keys[j])
		{
			j--;
		}
		if (j >= i)
		{
			if (j != i)
			{
				__declspec(align(16)) unsigned long swap = keys[j];
				keys[j] = keys[i];
				keys[i] = swap;
			}
			j--;
			i++;
		}
	} while (i <= j);
	if (low < j)
	{
		function_2e9360(keys, low, j, order);
	}
	if (i < high)
	{
		function_2e9360(keys, i, high, order);
	}
}

// a block from the top of the allocator's stack area, or from its slot 9 when
// the area is too small
inline void *stack_allocate_2e93e0(long size)
{
	memory_2d88a0 *memory = g_memory_2d88a0;
	long free = memory->stack_free;
	if (size > free)
	{
		return memory->allocate_stack_overflow(size);
	}
	char *block = memory->stack_cursor;
	memory->stack_cursor = block + size;
	memory->stack_free = free - size;
	return block;
}

// @retail 0x2e93e0
stack_elements_2e93e0::stack_elements_2e93e0(long capacity)
{
	void *block = stack_allocate_2e93e0((capacity * sizeof(element_2e9530) + 15) & ~15);
	data = (element_2e9530 *)block;
	capacity_and_flags = capacity | 0x80000000;
	stack_block = (element_2e9530 *)block;
	stack_capacity = capacity;
}

// @retail 0x2e9480
stack_elements_2e93e0::~stack_elements_2e93e0()
{
	g_memory_2d88a0->release_stack(stack_block, (stack_capacity * sizeof(element_2e9530) + 15) & ~15);
}
