#ifndef REFERENCE_NAME_REGISTRY_H
#define REFERENCE_NAME_REGISTRY_H

#include "unknown_1efac0.h"
#include "hash_table_2d9240.h"
#include "sized_reference_a9ef0.h"

// Distinct registry member table: retail's EH cleanup uses its implicit
// forwarding destructor at 0x2d70f0, rather than the base destructor directly.
struct lane_a_name_table : class_2d9240
{
	lane_a_name_table();
};

// Polymorphic reference header (8 bytes), followed by the name table.
// Retail constructor 0x2d7270 and destructor 0x2d72f0 establish this layout.
class c_reference_name_registry : public c_sized_reference_a9ef0
{
public:
	c_reference_name_registry();
	virtual ~c_reference_name_registry();

	static void *operator new(unsigned int size)
	{
		c_reference_name_registry *object = (c_reference_name_registry *)
			g_memory_2d88a0->allocate_block(size, 0x13);
		object->memory_size = (unsigned short)size;
		return object;
	}
	lane_a_name_table names;

	static void __cdecl operator delete(void *object)
	{
		g_memory_2d88a0->release_block(object,
			((c_reference_name_registry *)object)->memory_size, 0x13);
	}

	unsigned int copy_name(c_a const *object, char *buffer, unsigned int size);
	void set_name(c_a const *object, char const *name);
};

extern c_reference_name_registry *g_48011c;

#endif
