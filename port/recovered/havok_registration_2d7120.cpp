// @flags /O2 /GL-
// Havok (third-party physics library): the names the registration manager
// (g_48011c, built by 0x2d73a0 in havok_base_system.cpp) keeps for objects,
// and four small pieces near it:
//   0x2d7120  an object's name copied into a buffer of a given size; the
//             name's length, or 0 when the object has none (reached through
//             0x2d7210 when the object's flag 0x8000 says it is named)
//   0x2d7180  gives an object a name (a copy is kept), or drops its name for
//             a null one (reached through 0x2d7240, which then sets or
//             clears the object's flag)
//   0x2d70f0  gives an open-addressing table's storage back (the same code
//             as 0x2d94b0)
//   0x2d7100, 0x2d7860  give an object's block back with the size it keeps
//             at +4, tag 0x13 and 0x15 (operator delete of two classes)
//   0x2d7540  the destructor of an array of 4-byte elements
//
// The manager: {vtable, size, count, then at +8 its table (class_2d9240,
// keys then values, 0 for an empty key) from objects to their names}.
//
// src/unknown_2d7210.cpp declares 0x2d7180's second argument a bool; retail
// passes the name through (every direct caller passes 0, to drop it). The
// bool form defined here for that declaration passes its value on as the
// name, as retail does.
#include "reference_counted_2d88a0.h"
#include "hash_table_2d9240.h"
#include "string_helpers_2d8c90.h"
#include "unknown_11c920.h"

// ---------------------------------------------------------------------------
// recovered code this file calls, declared as src declares it

// a copy of a string in a block from the allocator (src/unknown_2d8e10.cpp)
char *__cdecl function_2d8e10(const char *string);

// finding a key's slot (mask + 1 when it is absent), emptying a slot and
// giving the storage back are class_2d9240's (hash_table_2d9240.h:
// src/unknown_2d9370.cpp, src/unknown_2d94b0.cpp)

// the object a name belongs to (src/unknown_2d7210.cpp)
class c_physics_runtime_base;

// ---------------------------------------------------------------------------

class c_physics_registration_manager
{
public:
	void const *vtable;
	unsigned short memory_size;
	unsigned short reference_count;
	class_2d9240 names;

	long function_2d7120(c_physics_runtime_base *object, long first, long second);
	void set_name(c_physics_runtime_base *object, char const *name);
	void function_2d7180(c_physics_runtime_base *object, bool enable);
};

typedef char assert_c_physics_registration_manager[(sizeof(c_physics_registration_manager) == 0x14) ? 1 : -1];

// `first` is the buffer, `second` its size
// @retail 0x2d7120
long c_physics_registration_manager::function_2d7120(c_physics_runtime_base *object, long first, long second)
{
	int index = (int)names.find((unsigned long)object);
	if (!names.is_valid(index).value)
	{
		return 0;
	}
	char const *name = (char const *)names.word_at(index);
	function_2d8d60((char *)first, name, (unsigned int)second);
	return (long)string_length_2d8d80(name);
}

// @retail 0x2d7180
void c_physics_registration_manager::set_name(c_physics_runtime_base *object, char const *name)
{
	class_2d9240 *table = &names;
	int index = (int)table->find((unsigned long)object);
	if (names.is_valid(index).value)
	{
		g_memory_2d88a0->release((void *)names.word_at(index));
		if (name)
		{
			names.store_word_at(index, (unsigned long)function_2d8e10(name));
		}
		else
		{
			table->function_2d93b0((unsigned long)index);
		}
	}
	else if (name)
	{
		names.function_2d92f0((unsigned long)object, (unsigned long)function_2d8e10(name));
	}
}

// the form src/unknown_2d7210.cpp calls
void c_physics_registration_manager::function_2d7180(c_physics_runtime_base *object, bool enable)
{
	set_name(object, (char const *)(unsigned long)enable);
}

// ---------------------------------------------------------------------------

class table_2d70f0
{
public:
	void *slots;
	long field_4;
	long mask;

	void release();
};

// @retail 0x2d70f0
void table_2d70f0::release()
{
	((class_2d9240 *)this)->~class_2d9240();
}

struct sized_2d7100
{
	void const *vtable;
	unsigned short memory_size;
	unsigned short reference_count;
};

// @retail 0x2d7100
void __cdecl function_2d7100(sized_2d7100 *object)
{
	g_memory_2d88a0->release_block(object, object->memory_size, 0x13);
}

// @retail 0x2d7860
void __cdecl function_2d7860(sized_2d7100 *object)
{
	g_memory_2d88a0->release_block(object, object->memory_size, 0x15);
}

struct array_2d7540
{
	void *data;
	long size;
	long capacity_and_flags;

	void destroy();
};

// @retail 0x2d7540
void array_2d7540::destroy()
{
	if (capacity_and_flags >= 0)
	{
		g_memory_2d88a0->release_block(data, (capacity_and_flags & 0x7fffffff) * 4, 0x12);
	}
}

// ---------------------------------------------------------------------------
// an object's name, as main's c_a declares its slots (unknown_1efac0.h; main
// defines them with its own registry in src/unknown_2d7120.cpp, which the
// whole image leaves out: port/whole_image_exclude.txt). The retail tables
// reach 0x2d7210 and 0x2d7240 through src/unknown_2d7210.cpp; these serve
// the natively built tables.
#include "unknown_1efac0.h"

extern c_physics_registration_manager *g_48011c;

unsigned int c_a::v1(char *buffer, unsigned int size)
{
	if (flags & 0x8000)
	{
		return (unsigned int)g_48011c->function_2d7120((c_physics_runtime_base *)this, (long)buffer, (long)size);
	}
	return 0;
}

void c_a::v2(char const *name)
{
	g_48011c->set_name((c_physics_runtime_base *)this, name);
	if (name)
	{
		flags |= 0x8000;
	}
	else
	{
		flags &= 0x7fff;
	}
}
