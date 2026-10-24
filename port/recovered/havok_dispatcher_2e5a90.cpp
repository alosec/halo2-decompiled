// @flags /O2 /GL-
// Havok (third-party physics library): the collision dispatcher's 32x32 table
// of handlers for pairs of shape types (reset by 0x2e5990), and how it is
// filled:
//   0x2e5a90  installs four handlers for a pair of types, and for every type
//             registered as a subtype of either; a type of -1 means every
//             type, and each cell keeps how specialized its handlers are
//   0x2e5ce0  merges a base type's mask of types into a subtype's, and into
//             the subtypes of that subtype
//   0x2e5e70  registers a type as a subtype of another, rebuilding the table
//             from the registrations first when it was already built
//   0x2e6190  the built-in subtype relations of the shape types
//   0x2e6cd0, 0x2e7d10, 0x2e9030  register the handlers of three agents for a
//             type against every type, both ways round
//   0x2e90f0  runs a query between two sides through the handler for their
//             shapes' types
#include "growable_array_2d91d0.h"

// the four handlers registered together for a pair of shape types (as
// include/agent_table_2e5d50.h declares them)
typedef void (__cdecl *handler_2e5d50)(void);

struct handlers_2e5d50
{
	handler_2e5d50 handler[4];
};

// the table's reset (src/unknown_2e5990.cpp)
struct table_2e5990
{
	void reset();
};

// a cell's pair of types (those its handlers were registered for) and how
// specialized they are; 100 after a reset
struct cell_2e5a90
{
	signed char first_type;
	signed char second_type;
	signed char weight;
};

// a type registered as a subtype of another
struct subtype_2e5e70
{
	long type;
	long base;
};

struct registration_2e5e70
{
	long first_type;
	long second_type;
	unsigned char flag;
	handlers_2e5d50 handlers;
};

struct subtypes_2e5e70
{
	subtype_2e5e70 *data;
	long size;
	long capacity_and_flags;
};

struct registrations_2e5e70
{
	registration_2e5e70 *data;
	long size;
	long capacity_and_flags;
};

typedef void (__cdecl *query_2e5a90)(void const *a, void const *b, void const *input, void *collector);

struct s_1c3a40
{
	char unknown0[0x10c];
	unsigned long type_masks[32];
	query_2e5a90 entry[32][32];
	handler_2e5d50 first[32][32];
	handler_2e5d50 second[32][32];
	handler_2e5d50 third[32][32];
	cell_2e5a90 cell[32][32];
	unsigned char built;
	unsigned char field_4d8d;
	char unknown4d8e[2];
	subtypes_2e5e70 subtypes;
	registrations_2e5e70 registrations;

	void function_2e5a90(handlers_2e5d50 const *handlers, long first_type, long second_type, long first_type_again,
		long second_type_again, bool arg_5);
	void install_2e5a90(handlers_2e5d50 const *handlers, long first_type, long second_type, long registered_first,
		long registered_second, long depth);
	void function_2e5ce0(long type, long base, long depth);
	void function_2e5d50(handlers_2e5d50 const *handlers, long first_type, long second_type);
	void function_2e5e70(long type, long base);
};

typedef char assert_s_1c3a40_cells[(__builtin_offsetof(s_1c3a40, cell) == 0x418c) ? 1 : -1];
typedef char assert_s_1c3a40_built[(__builtin_offsetof(s_1c3a40, built) == 0x4d8c) ? 1 : -1];
typedef char assert_s_1c3a40_subtypes[(__builtin_offsetof(s_1c3a40, subtypes) == 0x4d90) ? 1 : -1];
typedef char assert_s_1c3a40_registrations[(__builtin_offsetof(s_1c3a40, registrations) == 0x4d9c) ? 1 : -1];
typedef char assert_registration_2e5e70[(sizeof(registration_2e5e70) == 0x1c) ? 1 : -1];

// retail passes the depth in the flag's stack slot as a whole dword and
// recurses with depth + 1, so the recursion goes through a helper taking a long
// @retail 0x2e5a90
void s_1c3a40::function_2e5a90(handlers_2e5d50 const *handlers, long first_type, long second_type,
	long first_type_again, long second_type_again, bool arg_5)
{
	install_2e5a90(handlers, first_type, second_type, first_type_again, second_type_again, arg_5 ? 1 : 0);
}

void s_1c3a40::install_2e5a90(handlers_2e5d50 const *handlers, long first_type, long second_type,
	long registered_first, long registered_second, long depth)
{
	built = 1;
	// the subtypes of either type get the same handlers, one level less
	// specialized
	for (long i = 0; i < subtypes.size; i++)
	{
		subtype_2e5e70 const &subtype = subtypes.data[i];
		if (subtype.base == first_type)
		{
			install_2e5a90(handlers, subtype.type, second_type, registered_first, registered_second, depth + 1);
		}
		else if (subtype.base == second_type)
		{
			install_2e5a90(handlers, first_type, subtype.type, registered_first, registered_second, depth + 1);
		}
	}

	long first_begin = first_type;
	long first_end = first_type + 1;
	long second_begin = second_type;
	long second_end = second_type + 1;
	long weight = depth;
	if (first_type == -1)
	{
		weight++;
		first_begin = 1;
		first_end = 0x20;
	}
	if (second_type == -1)
	{
		weight++;
		second_begin = 1;
		second_end = 0x20;
	}

	for (long i = first_begin; i < first_end; i++)
	{
		for (long j = second_begin; j < second_end; j++)
		{
			entry[i][j] = (query_2e5a90)handlers->handler[0];
			first[i][j] = handlers->handler[1];
			second[i][j] = handlers->handler[2];
			third[i][j] = handlers->handler[3];
			// retail here (with the flag at 0x4d8d set, and the cell already
			// holding more specialized handlers) formats the warning at
			// 0x412640 ("Agent handling types <%s-%s> would override more
			// specialized agent <%s-%s> ...", the names from 0x2e6220) into a
			// local buffer through 0x2d8ca0 and never reports it; nothing
			// else is affected, so it is left out
			cell[i][j].first_type = (signed char)registered_first;
			cell[i][j].second_type = (signed char)registered_second;
			cell[i][j].weight = (signed char)weight;
		}
	}
}

// @retail 0x2e5ce0
void s_1c3a40::function_2e5ce0(long type, long base, long depth)
{
	type_masks[type] |= type_masks[base];
	for (long i = 0; i < subtypes.size; i++)
	{
		if (subtypes.data[i].base == type)
		{
			function_2e5ce0(subtypes.data[i].type, type, depth + 1);
		}
	}
}

// @retail 0x2e5e70
void s_1c3a40::function_2e5e70(long type, long base)
{
	if (built)
	{
		// rebuild the table from the registrations kept so far, each with
		// the flag it was registered under
		((table_2e5990 *)this)->reset();
		unsigned char flag = field_4d8d;
		for (long i = 0; i < registrations.size; i++)
		{
			registration_2e5e70 *registration = &registrations.data[i];
			field_4d8d = registration->flag;
			function_2e5d50(&registration->handlers, registration->first_type, registration->second_type);
		}
		field_4d8d = flag;
	}

	for (long i = 0; i < subtypes.size; i++)
	{
		if (subtypes.data[i].type == type && subtypes.data[i].base == base)
		{
			subtypes.size--;
			for (long j = i; j < subtypes.size; j++)
			{
				subtypes.data[j] = subtypes.data[j + 1];
			}
			i--;
		}
	}
	function_2e5ce0(type, base, 0);
	if (subtypes.size == (subtypes.capacity_and_flags & 0x7fffffff))
	{
		function_2d91d0(&subtypes, sizeof(subtype_2e5e70));
	}
	subtype_2e5e70 &slot = subtypes.data[subtypes.size++];
	slot.type = type;
	slot.base = base;
}

// @retail 0x2e6190
void __cdecl function_2e6190(s_1c3a40 *dispatcher)
{
	dispatcher->function_2e5e70(4, 1);
	dispatcher->function_2e5e70(5, 1);
	dispatcher->function_2e5e70(6, 1);
	dispatcher->function_2e5e70(7, 1);
	dispatcher->function_2e5e70(8, 1);
	dispatcher->function_2e5e70(0x17, 1);
	dispatcher->function_2e5e70(0xb, 2);
	dispatcher->function_2e5e70(0xa, 2);
	dispatcher->function_2e5e70(0x13, 3);
	dispatcher->function_2e5e70(1, 0x10);
	dispatcher->function_2e5e70(0x12, 0xd);
	dispatcher->function_2e5e70(0xe, 0xd);
}

// the agents' handlers nobody has recovered yet, reached by retail address
extern "C" void __cdecl h2_retail_2e6c60(void);
extern "C" void __cdecl h2_retail_2e6780(void);
extern "C" void __cdecl h2_retail_2e67e0(void);
extern "C" void __cdecl h2_retail_2e6a20(void);
extern "C" void __cdecl h2_retail_2e66e0(void);
extern "C" void __cdecl h2_retail_2e64a0(void);
extern "C" void __cdecl h2_retail_2e6560(void);
extern "C" void __cdecl h2_retail_2e6620(void);
extern "C" void __cdecl h2_retail_2e7ca0(void);
extern "C" void __cdecl h2_retail_2e7850(void);
extern "C" void __cdecl h2_retail_2e78b0(void);
extern "C" void __cdecl h2_retail_2e7ab0(void);
extern "C" void __cdecl h2_retail_2e6e30(void);
extern "C" void __cdecl h2_retail_2e7710(void);
extern "C" void __cdecl h2_retail_2e7510(void);
extern "C" void __cdecl h2_retail_2e71f0(void);
extern "C" void __cdecl h2_retail_2e8fc0(void);
extern "C" void __cdecl h2_retail_2e8880(void);
extern "C" void __cdecl h2_retail_2e88e0(void);
extern "C" void __cdecl h2_retail_2e8940(void);
extern "C" void __cdecl h2_retail_2e8e00(void);
extern "C" void __cdecl h2_retail_2e86e0(void);
extern "C" void __cdecl h2_retail_2e80a0(void);
extern "C" void __cdecl h2_retail_2e83c0(void);

// the handlers of the agent for type 0x14 against every type, both ways round
// @retail 0x2e6cd0
void __cdecl function_2e6cd0(s_1c3a40 *dispatcher)
{
	handlers_2e5d50 handlers;
	handlers.handler[0] = h2_retail_2e6c60;
	handlers.handler[1] = h2_retail_2e6780;
	handlers.handler[2] = h2_retail_2e67e0;
	handlers.handler[3] = h2_retail_2e6a20;
	dispatcher->function_2e5d50(&handlers, -1, 0x14);
	handlers.handler[0] = h2_retail_2e66e0;
	handlers.handler[1] = h2_retail_2e64a0;
	handlers.handler[2] = h2_retail_2e6560;
	handlers.handler[3] = h2_retail_2e6620;
	dispatcher->function_2e5d50(&handlers, 0x14, -1);
}

// the same for type 0x11
// @retail 0x2e7d10
void __cdecl function_2e7d10(s_1c3a40 *dispatcher)
{
	handlers_2e5d50 handlers;
	handlers.handler[0] = h2_retail_2e7ca0;
	handlers.handler[1] = h2_retail_2e7850;
	handlers.handler[2] = h2_retail_2e78b0;
	handlers.handler[3] = h2_retail_2e7ab0;
	dispatcher->function_2e5d50(&handlers, -1, 0x11);
	handlers.handler[0] = h2_retail_2e6e30;
	handlers.handler[1] = h2_retail_2e7710;
	handlers.handler[2] = h2_retail_2e7510;
	handlers.handler[3] = h2_retail_2e71f0;
	dispatcher->function_2e5d50(&handlers, 0x11, -1);
}

// the same for type 9
// @retail 0x2e9030
void __cdecl function_2e9030(s_1c3a40 *dispatcher)
{
	handlers_2e5d50 handlers;
	handlers.handler[0] = h2_retail_2e8fc0;
	handlers.handler[1] = h2_retail_2e8880;
	handlers.handler[2] = h2_retail_2e88e0;
	handlers.handler[3] = h2_retail_2e8940;
	dispatcher->function_2e5d50(&handlers, -1, 9);
	handlers.handler[0] = h2_retail_2e8e00;
	handlers.handler[1] = h2_retail_2e86e0;
	handlers.handler[2] = h2_retail_2e80a0;
	handlers.handler[3] = h2_retail_2e83c0;
	dispatcher->function_2e5d50(&handlers, 9, -1);
}

// a shape, whose sixth virtual method gives its type
class typed_2e90f0
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual long type() const { return 0; }
};

// a side of a query starts with its shape; the query's input with the
// dispatcher
struct side_2e90f0
{
	typed_2e90f0 const *shape;
};

struct input_2e90f0
{
	s_1c3a40 *dispatcher;
};

// @retail 0x2e90f0
void __cdecl function_2e90f0(void const *a, void const *b, void const *input, void *collector)
{
	s_1c3a40 *dispatcher = ((input_2e90f0 const *)input)->dispatcher;
	long first_type = ((side_2e90f0 const *)a)->shape->type();
	long second_type = ((side_2e90f0 const *)b)->shape->type();
	dispatcher->entry[first_type][second_type](a, b, input, collector);
}
