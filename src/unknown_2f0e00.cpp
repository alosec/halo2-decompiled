// @flags /O2 /GL-
// Havok (third-party physics library): registering the handlers for pairs of
// shape types in the handler table.
#include "agent_table_2e5d50.h"

struct body_2f09b0;
class collector_2f09b0;
class indexed_2f00a0;
class indexed_2f00d0;
indexed_2f00d0 *__cdecl function_2f00d0(long arg_0, long arg_1, long arg_2, long value);
void __cdecl function_2f0b80(body_2f09b0 const *capsule_body, body_2f09b0 const *sphere_body, void const *input, collector_2f09b0 *collector);
void __cdecl function_2f0c40(void const *a, void const *b, void const *input, void *collector);
void __cdecl function_2f0ca0(void const *a, void const *b, void const *input, void *collector, void *second_collector);
indexed_2f00a0 *__cdecl function_2f00a0(long arg_0, long arg_1, long arg_2, long value);
void __cdecl function_2f09b0(body_2f09b0 const *a, body_2f09b0 const *b, void const *input, collector_2f09b0 *collector);
void __cdecl function_2f0700(void const *a, void const *b, void const *input, void *collector);
void __cdecl function_2fe830(void const *a, void const *b, void const *input, void *collector, void *second_collector);

// registers handlers for pairs of shape types, each pair both ways round
// where the pair differs
// @retail 0x2f0e00
void __cdecl function_2f0e00(s_1c3a40 *table)
{
	handlers_2e5d50 handlers;
	handlers.handler[0] = (handler_2e5d50)function_2f00d0;
	handlers.handler[1] = (handler_2e5d50)function_2f0b80;
	handlers.handler[2] = (handler_2e5d50)function_2f0c40;
	handlers.handler[3] = (handler_2e5d50)function_2f0ca0;
	table->function_2e5d50(&handlers, 7, 4);
	handlers.handler[0] = (handler_2e5d50)function_2f00a0;
	handlers.handler[1] = (handler_2e5d50)function_2f09b0;
	handlers.handler[2] = (handler_2e5d50)function_2f0700;
	handlers.handler[3] = (handler_2e5d50)function_2fe830;
	table->function_2e5d50(&handlers, 4, 7);
}
