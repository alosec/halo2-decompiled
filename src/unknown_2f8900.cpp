// @flags /O2 /GL-
// Havok (third-party physics library): registering the handlers for pairs of
// shape types in the handler table.
#include "agent_table_2e5d50.h"

struct description_2ed170;
struct dispatcher_2ed170;
class swapped_triangle_terms_2f7b20;
struct triangle_2f7b20;
class triangle_terms_2f7b20;
class typed_2ed170;
swapped_triangle_terms_2f7b20 *__cdecl function_2f7bb0(triangle_2f7b20 const **description, typed_2ed170 **other, dispatcher_2ed170 **dispatcher, long arg_3);
void __cdecl function_2f8680(void const *a, void const *b, void const *input, void *collector);
void __cdecl function_2f8740(void const *a, void const *b, void const *input, void *collector);
void __cdecl function_2f87a0(void const *a, void const *b, void const *input, void *collector, void *second_collector);
triangle_terms_2f7b20 *__cdecl function_2f7b20(description_2ed170 const *description, triangle_2f7b20 const **other, dispatcher_2ed170 **dispatcher, long arg_3);
void __cdecl function_2f84a0(void const *a, void const *b, void const *input, void *collector);
void __cdecl function_2f8110(void const *a, void const *b, void const *input, void *collector);
void __cdecl function_2fe830(void const *a, void const *b, void const *input, void *collector, void *second_collector);

// registers handlers for pairs of shape types, each pair both ways round
// where the pair differs
// @retail 0x2f8900
void __cdecl function_2f8900(s_1c3a40 *table)
{
	handlers_2e5d50 handlers;
	handlers.handler[0] = (handler_2e5d50)function_2f7bb0;
	handlers.handler[1] = (handler_2e5d50)function_2f8680;
	handlers.handler[2] = (handler_2e5d50)function_2f8740;
	handlers.handler[3] = (handler_2e5d50)function_2f87a0;
	table->function_2e5d50(&handlers, 5, 9);
	handlers.handler[0] = (handler_2e5d50)function_2f7b20;
	handlers.handler[1] = (handler_2e5d50)function_2f84a0;
	handlers.handler[2] = (handler_2e5d50)function_2f8110;
	handlers.handler[3] = (handler_2e5d50)function_2fe830;
	table->function_2e5d50(&handlers, 9, 5);
}
