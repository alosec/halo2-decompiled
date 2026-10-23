// @flags /O2 /GL- /GX
// Havok (third-party physics library): creating the object kept for a pair
// of shapes given the other way round.
#include "pair_agent_2ed170.h"
#include "agent_table_2e5d50.h"

// @retail 0x2ed5d0
pair_agent_2ed170 *__cdecl function_2ed5d0(typed_2ed170 **other, description_2ed170 const *description, dispatcher_2ed170 **dispatcher, long arg_3)
{
	return new swapped_pair_agent_2ed170(other, description, dispatcher, arg_3);
}

struct description_2ecc80;
struct description_2ed170;
struct dispatcher_2ecd80;
struct dispatcher_2ed170;
class pair_agent_2ed170;
struct query_2ed2a0;
class typed_2ecd80;
class typed_2ed170;
pair_agent_2ed170 *__cdecl function_2ed5d0(typed_2ed170 **other, description_2ed170 const *description, dispatcher_2ed170 **dispatcher, long arg_3);
void __cdecl function_2ed0b0(void const *a, void const *b, void const *input, void *collector);
void __cdecl function_2ed110(void const *a, void const *b, void const *input, void *collector);
void __cdecl function_2ed2a0(typed_2ecd80 **other, description_2ecc80 const *description, query_2ed2a0 const *query, void *collector, void *second_collector);
pair_agent_2ed170 *__cdecl function_2ed170(description_2ed170 const *description, typed_2ed170 **other, dispatcher_2ed170 **dispatcher, long arg_3);
void __cdecl function_2ecfa0(void const *a, void const *b, void const *input, void *collector);
void __cdecl function_2ece90(void const *a, void const *b, void const *input, void *collector);
void __cdecl function_2ecd80(description_2ecc80 const *description, typed_2ecd80 **other, dispatcher_2ecd80 **dispatcher, long arg_3, long arg_4);

// registers handlers for pairs of shape types, each pair both ways round
// where the pair differs
// @retail 0x2ed700
void __cdecl function_2ed700(s_1c3a40 *table)
{
	handlers_2e5d50 handlers;
	handlers.handler[0] = (handler_2e5d50)function_2ed5d0;
	handlers.handler[1] = (handler_2e5d50)function_2ed0b0;
	handlers.handler[2] = (handler_2e5d50)function_2ed110;
	handlers.handler[3] = (handler_2e5d50)function_2ed2a0;
	table->function_2e5d50(&handlers, -1, 21);
	handlers.handler[0] = (handler_2e5d50)function_2ed170;
	handlers.handler[1] = (handler_2e5d50)function_2ecfa0;
	handlers.handler[2] = (handler_2e5d50)function_2ece90;
	handlers.handler[3] = (handler_2e5d50)function_2ecd80;
	table->function_2e5d50(&handlers, 21, -1);
}
