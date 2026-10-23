#ifndef AGENT_TABLE_2E5D50_H
#define AGENT_TABLE_2E5D50_H
// Havok (third-party physics library): the table of handlers for pairs of
// shape types (0x2e5990 resets it). Registering a set of four handlers for a
// pair drops any earlier registration of the same pair, records the new one
// (with the flag 0x2e5a10 sets) and then installs it (0x2e5a90).
#include "growable_array_2d91d0.h"

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

ELEMENTS_2D91D0_ALIGNED(registrations_2e5d50, registration_2e5d50);

struct s_1c3a40
{
	char unknown0[0x4d8d];
	unsigned char field_4d8d;
	char unknown4d8e[0x4d9c - 0x4d8e];
	registrations_2e5d50 registrations;

	void function_2e5a90(handlers_2e5d50 const *handlers, long first_type, long second_type, long first_type_again,
		long second_type_again, bool arg_5);
	void function_2e5d50(handlers_2e5d50 const *handlers, long first_type, long second_type);
};

#endif
