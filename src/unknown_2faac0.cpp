// @flags /O2 /GL-
// Havok (third-party physics library): registering the handlers for pairs of
// shape types in the handler table.
#include "agent_table_2e5d50.h"

struct body_2ea940;
class bounded_2f9bb0;
class bounded_2fa070;
struct input_2ea940;
bounded_2f9bb0 *__cdecl function_2f9bb0(long arg_0, long arg_1, long arg_2, long value);
void __cdecl function_2fab70(void const *a, void const *b, void const *input, void *collector);
void __cdecl function_2fabf0(body_2ea940 const *a, body_2ea940 const *b, input_2ea940 const *input, void *collector);
void __cdecl function_2f9790(void const *a, void const *b, void const *input, void *collector, void *second_collector);
bounded_2fa070 *__cdecl function_2fa070(long arg_0, long arg_1, long arg_2, long value);
void __cdecl function_2f98d0(void const *a, void const *b, void const *input, void *collector);
void __cdecl function_2f9950(body_2ea940 const *a, body_2ea940 const *b, input_2ea940 const *input, long value);
void __cdecl function_2f99d0(void const *a, void const *b, void const *input, void *collector, void *second_collector);

// registers handlers for pairs of shape types, each pair both ways round
// where the pair differs
// @retail 0x2faac0
void __cdecl function_2faac0(s_1c3a40 *table)
{
	handlers_2e5d50 handlers;
	handlers.handler[0] = (handler_2e5d50)function_2f9bb0;
	handlers.handler[1] = (handler_2e5d50)function_2fab70;
	handlers.handler[2] = (handler_2e5d50)function_2fabf0;
	handlers.handler[3] = (handler_2e5d50)function_2f9790;
	table->function_2e5d50(&handlers, 1, 20);
	handlers.handler[0] = (handler_2e5d50)function_2fa070;
	handlers.handler[1] = (handler_2e5d50)function_2f98d0;
	handlers.handler[2] = (handler_2e5d50)function_2f9950;
	handlers.handler[3] = (handler_2e5d50)function_2f99d0;
	table->function_2e5d50(&handlers, 20, 1);
}
