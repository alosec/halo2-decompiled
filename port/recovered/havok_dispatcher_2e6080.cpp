// @flags /O2 /GL-
// Havok (third-party physics library): the destructor of the collision
// dispatcher-like object with vtable 0x412704 (its deleting destructor
// 0x2e6160 is generated). It releases the 8 x 8 table of referenced objects
// it keeps from 0xc, frees its two arrays and leaves the object with the
// reference-counted base's vtable, 0x452c30.
#include "reference_counted_2d88a0.h"

struct array_2e6080
{
	void *data;
	long size;
	unsigned long capacity_and_flags;
};

struct dispatcher_2e6080
{
	void const *vtable;
	unsigned short memory_size;
	unsigned short reference_count;
	char unknown8[4];
	referenced_2d88a0 *table[8][8];
	char unknown10c[0x4d90 - 0x10c];
	array_2e6080 pairs;   // 8-byte elements
	array_2e6080 records; // 0x1c-byte elements
};

typedef char assert_dispatcher_2e6080_records[__builtin_offsetof(dispatcher_2e6080, records) == 0x4d9c ? 1 : -1];

// @retail 0x2e6080
extern "C" void dispatcher_2e6080_destroy(dispatcher_2e6080 *self)
{
	self->vtable = (void const *)0x412704;
	for (long i = 0; i < 8; ++i)
	{
		for (long j = 0; j < 8; ++j)
		{
			referenced_2d88a0 *object = self->table[i][j];
			if (object && --object->reference_count == 0)
			{
				typedef void (*t_delete)(void *, unsigned long);
				((t_delete)(*(void ***)object)[0])(object, 1);
			}
		}
	}
	if ((long)self->records.capacity_and_flags >= 0)
	{
		g_memory_2d88a0->release_block(self->records.data,
			(self->records.capacity_and_flags & 0x7fffffff) * 0x1c, 0x12);
	}
	if ((long)self->pairs.capacity_and_flags >= 0)
	{
		g_memory_2d88a0->release_block(self->pairs.data,
			(self->pairs.capacity_and_flags & 0x7fffffff) * 8, 0x12);
	}
	self->vtable = (void const *)0x452c30;
}
