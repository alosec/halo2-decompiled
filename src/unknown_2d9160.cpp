// @flags /O2 /GL-
// Havok (third-party physics library): reallocating its growable arrays.
#include "growable_array_2d91d0.h"

// @retail 0x2d9160
void __cdecl function_2d9160(void *array, long capacity, long element_size)
{
	array_2d91d0 *self = (array_2d91d0 *)array;
	void *data = g_memory_2d88a0->allocate_block(capacity * element_size, 0x12);
	function_2d8db0(data, self->data, self->size * element_size);
	if ((self->capacity_and_flags & 0x80000000) == 0)
	{
		g_memory_2d88a0->release_block(self->data, (self->capacity_and_flags & 0x7fffffff) * element_size, 0x12);
	}
	self->data = data;
	self->capacity_and_flags = capacity;
}

// @retail 0x2d91d0
void __cdecl function_2d91d0(void *array, long element_size)
{
	array_2d91d0 *self = (array_2d91d0 *)array;
	long capacity = self->size * 2;
	if (self->size == 0)
	{
		capacity = 1;
	}
	void *data = g_memory_2d88a0->allocate_block(capacity * element_size, 0x12);
	function_2d8db0(data, self->data, self->size * element_size);
	if ((self->capacity_and_flags & 0x80000000) == 0)
	{
		g_memory_2d88a0->release_block(self->data, (self->capacity_and_flags & 0x7fffffff) * element_size, 0x12);
	}
	self->data = data;
	self->capacity_and_flags = capacity;
}
