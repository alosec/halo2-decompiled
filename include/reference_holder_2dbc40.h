#ifndef REFERENCE_HOLDER_2DBC40_H
#define REFERENCE_HOLDER_2DBC40_H
// Havok (third-party physics library): an object that holds a reference to
// another and lets go of it when it is destroyed (0x2dbc40), freed with
// tag 0x22 by its deleting destructor (0x2dbf90).
#include "unknown_0a9f20.h"
#include "reference_counted_2d88a0.h"

class class_2dbc40 : public c_reference_a9f20
{
public:
	char field_8[0x30 - 8];
	referenced_2d88a0 *field_30;

	virtual ~class_2dbc40();

	static void operator delete(void *object)
	{
		g_memory_2d88a0->release_block(object, ((c_sized_reference *)object)->flags, 0x22);
	}
};

#endif
