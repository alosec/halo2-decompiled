// @flags /O2 /GL- /GX
// Reference-object names are owned by the registry, keyed by object address.
#include "reference_name_registry.h"
#include "string_helpers_2d8c90.h"
#include "unknown_146a20.h"

char *__cdecl function_2d8e10(char const *string);

c_reference_name_registry *g_48011c;

// @retail 0x2d7270
c_reference_name_registry::c_reference_name_registry()
{
}

// @retail 0x2d73a0
c_reference_name_registry *__cdecl function_2d73a0()
{
	return new c_reference_name_registry;
}

// @retail 0x2d7100
void __cdecl function_2d7100(void *object)
{
	g_memory_2d88a0->release_block(object,
		((c_reference_name_registry *)object)->memory_size, 0x13);
}

// @retail 0x2d70f0 destructor lane_a_name_table

// @retail 0x2d72c0 deleting c_reference_name_registry

// @retail 0x2d72f0
c_reference_name_registry::~c_reference_name_registry()
{
	int index = names.first();
	for (;;)
	{
		bool_2d9240 valid = names.is_valid(index);
		if (!valid.value)
		{
			break;
		}
		void *name = (void *)names.word_at(index);
		((c_havok_abstract_allocator *)g_480118)->deallocate(name);
		index = names.next(index);
	}
}

// @retail 0x2d7120
unsigned int c_reference_name_registry::copy_name(c_a const *object, char *buffer, unsigned int size)
{
	unsigned long index = names.find((unsigned long)object);
	bool_2d9240 valid = names.is_valid(index);
	if (valid.value)
	{
		char const *name = (char const *)names.word_at(index);
		function_2d8d60(buffer, name, size);
		return string_length_2d8d80(name);
	}
	return 0;
}

// @retail 0x2d7180
void c_reference_name_registry::set_name(c_a const *object, char const *name)
{
	unsigned long index = names.find((unsigned long)object);
	bool_2d9240 valid = names.is_valid(index);
	if (valid.value)
	{
		void *old_name = (void *)names.word_at(index);
		((c_havok_abstract_allocator *)g_480118)->deallocate(old_name);
		if (name)
		{
			names.store_word_at(index, (unsigned long)function_2d8e10(name));
		}
		else
		{
			names.function_2d93b0(index);
		}
	}
	else if (name)
	{
		names.function_2d92f0((unsigned long)object, (unsigned long)function_2d8e10(name));
	}
}

// @retail 0x2d7210
unsigned int c_a::v1(char *buffer, unsigned int size)
{
	if (flags & 0x8000)
	{
		return g_48011c->copy_name(this, buffer, size);
	}
	return 0;
}

// @retail 0x2d7240
void c_a::v2(char const *name)
{
	g_48011c->set_name(this, name);
	if (name)
	{
		flags |= 0x8000;
	}
	else
	{
		flags &= 0x7fff;
	}
}
