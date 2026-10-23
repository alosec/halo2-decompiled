// @flags /O2 /GL- /GX
// Havok (third-party physics library): makes a new object for a pair of
// shapes given the other way round, whose second holds a number of parts.
#include "parts_agent_2f8ff0.h"
#include "agent_table_2e5d50.h"

// the object with the two shapes the other way round
class swapped_parts_agent_2f9660 : public parts_agent_2f8ff0
{
public:
	swapped_parts_agent_2f9660(typed_2ed170 **other, parts_description_2f8ff0 const *description, dispatcher_2ed170 **dispatcher, long arg_3) :
		parts_agent_2f8ff0(description, other, dispatcher, arg_3)
	{
	}
};

// @retail 0x2f9660
sized_2ed170 *__cdecl function_2f9660(typed_2ed170 **other, parts_description_2f8ff0 const *description, dispatcher_2ed170 **dispatcher, long arg_3)
{
	return new swapped_parts_agent_2f9660(other, description, dispatcher, arg_3);
}

struct dispatcher_2ed170;
struct parts_description_2f8ff0;
class sized_2ed170;
class typed_2ed170;
sized_2ed170 *__cdecl function_2f9660(typed_2ed170 **other, parts_description_2f8ff0 const *description, dispatcher_2ed170 **dispatcher, long arg_3);
void function_175f40();
void __cdecl function_2f8e90(void const *a, void const *b, void const *input, void *collector);
void __cdecl function_2f8ef0(void const *a, void const *b, void const *input, void *collector, void *second_collector);
sized_2ed170 *__cdecl function_2f93e0(parts_description_2f8ff0 const *description, typed_2ed170 **other, dispatcher_2ed170 **dispatcher, long arg_3);
void __cdecl function_2f8c20(void const *a, void const *b, void const *input, void *collector);
void __cdecl function_2fe830(void const *a, void const *b, void const *input, void *collector, void *second_collector);

// registers handlers for pairs of shape types, each pair both ways round
// where the pair differs
// @retail 0x2f96d0
void __cdecl function_2f96d0(s_1c3a40 *table)
{
	handlers_2e5d50 handlers;
	handlers.handler[0] = (handler_2e5d50)function_2f9660;
	handlers.handler[1] = (handler_2e5d50)function_175f40;
	handlers.handler[2] = (handler_2e5d50)function_2f8e90;
	handlers.handler[3] = (handler_2e5d50)function_2f8ef0;
	table->function_2e5d50(&handlers, 1, 12);
	handlers.handler[0] = (handler_2e5d50)function_2f93e0;
	handlers.handler[1] = (handler_2e5d50)function_175f40;
	handlers.handler[2] = (handler_2e5d50)function_2f8c20;
	handlers.handler[3] = (handler_2e5d50)function_2fe830;
	table->function_2e5d50(&handlers, 12, 1);
}
