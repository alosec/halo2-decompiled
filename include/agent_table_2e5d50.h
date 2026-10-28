#ifndef AGENT_TABLE_2E5D50_H
#define AGENT_TABLE_2E5D50_H
// Havok (third-party physics library): the table of handlers for pairs of
// shape types (0x2e5990 resets it). Registering a set of four handlers for a
// pair drops any earlier registration of the same pair, records the new one
// (with the flag 0x2e5a10 sets) and then installs it (0x2e5a90).
#include "growable_array_2d91d0.h"
#include "sized_reference_a9ef0.h"
#include "physics_allocator.h"

struct s_2e5a10
{
 unsigned char field_0;
 s_2e5a10(bool value) : field_0(value) {}
 s_2e5a10(s_2e5a10 const &other) : field_0(other.field_0) {}
};

typedef void (__cdecl *handler_2e5d50)(void);

// the four handlers registered together for a pair of shape types
struct handlers_2e5d50
{
	handler_2e5d50 handler[4];
};

struct registration_2e5d50
{
	long first_type;
	long second_type;
	unsigned char flag;
	handlers_2e5d50 handlers;
};

struct pair_record_2e5a90
{
	char first_type;
	char second_type;
	char level;
};

ELEMENTS_2D91D0_ALIGNED(registrations_2e5d50, registration_2e5d50);

// the registrations, owning their storage unless its top bit says otherwise
struct owned_registrations_2e5d50 : registrations_2e5d50
{
	~owned_registrations_2e5d50()
	{
		if ((capacity_and_flags & 0x80000000) == 0)
		{
			registration_2e5d50 *storage = data;
			havok_allocator()->release(storage, (capacity_and_flags & 0x7fffffff) * sizeof(registration_2e5d50), 0x12);
		}
	}
};

// a shape type handled as another one where it has no handlers of its own
// (0x2e5e70 records them)
struct alternate_type_2e5d50
{
	long type;
	long alternate;
};

ELEMENTS_2D91D0(alternate_types_2e5d50, alternate_type_2e5d50);

struct owned_alternate_types_2e5d50 : alternate_types_2e5d50
{
	~owned_alternate_types_2e5d50()
	{
		if ((capacity_and_flags & 0x80000000) == 0)
			havok_allocator()->release(data, (capacity_and_flags & 0x7fffffff) * sizeof(alternate_type_2e5d50), 0x12);
	}
};

// the collision dispatcher (retail table 0x412704 over the reference base
// 0x452c30): an 8 by 8 table of reference-counted objects for pairs of
// response types at +0xc, the handler tables, the alternate shape types and
// the registrations
struct s_1c3a40 : c_sized_reference_a9ef0
{
	static void operator delete(void *object)
	{
		g_memory_2d88a0->release_block(object, ((s_1c3a40 *)object)->memory_size, 0x22);
	}
	virtual ~s_1c3a40();

	long unknown8;
	c_sized_reference_a9ef0 *response_objects[8][8];
	char unknown10c[0x18c - 0x10c];
	handler_2e5d50 handler_tables[4][32][32];
	pair_record_2e5a90 pair_records[32][32];
	unsigned char field_4d8c;
	unsigned char field_4d8d;
	char unknown4d8e[0x4d90 - 0x4d8e];
	owned_alternate_types_2e5d50 alternate_types;
	owned_registrations_2e5d50 registrations;

	void function_2e5a10(s_2e5a10 arg_0);
	void function_2e5a90(handlers_2e5d50 const *handlers, long first_type, long second_type, long first_type_again,
		long second_type_again, long level);
	void function_2e5d50(handlers_2e5d50 const *handlers, long first_type, long second_type);
	// records that shape type `type` is handled as `alternate`
	void function_2e5e70(long type, long alternate);
};

#endif
