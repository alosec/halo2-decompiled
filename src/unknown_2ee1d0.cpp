// @flags /O2 /GL- /GX
// Havok (third-party physics library): makes a new list of child objects for
// a pair of shapes given the other way round.
#include "child_list_2edd00.h"
#include "agent_table_2e5d50.h"

// the list with the two shapes the other way round
class swapped_list_2ee1d0 : public list_2eda30
{
public:
	swapped_list_2ee1d0(typed_2edd00 **other, description_2edd00 const *description, context_2edd00 const *context, long arg_3) :
		list_2eda30(description, other, context, arg_3)
	{
	}
};

// @retail 0x2ee1d0
sized_2edd00 *__cdecl function_2ee1d0(typed_2edd00 **other, description_2edd00 const *description, context_2edd00 const *context, long arg_3)
{
	return new swapped_list_2ee1d0(other, description, context, arg_3);
}

struct body_2ebc80;
struct collector_2ebc80;
struct context_2edd00;
struct description_2edd00;
struct input_2ebe40;
struct query_2ec580;
class sized_2edd00;
struct typed_2edd00;
sized_2edd00 *__cdecl function_2ee010(description_2edd00 const *description, typed_2edd00 **other, context_2edd00 const *context, long arg_3);
void __cdecl function_2ec200(body_2ebc80 const *a, body_2ebc80 const *b, input_2ebe40 const *input, collector_2ebc80 *collector);
void __cdecl function_2ebe40(body_2ebc80 const *a, body_2ebc80 const *b, input_2ebe40 const *input, void *collector);
void __cdecl function_2ec020(body_2ebc80 const *a, body_2ebc80 const *b, input_2ebe40 const *input, void *arg_3, void *arg_4);
sized_2edd00 *__cdecl function_2ee1d0(typed_2edd00 **other, description_2edd00 const *description, context_2edd00 const *context, long arg_3);
void __cdecl function_2ec310(body_2ebc80 const *b, body_2ebc80 const *a, input_2ebe40 const *input, collector_2ebc80 *collector);
void __cdecl function_2ec370(body_2ebc80 const *b, body_2ebc80 const *a, input_2ebe40 const *input, void *collector);
void __cdecl function_2ec580(body_2ebc80 const *b, body_2ebc80 const *a, query_2ec580 const *query, void *collector, void *second_collector);

// registers handlers for pairs of shape types, each pair both ways round
// where the pair differs
// @retail 0x2ee240
void __cdecl function_2ee240(s_1c3a40 *table)
{
	handlers_2e5d50 handlers;
	handlers.handler[0] = (handler_2e5d50)function_2ee010;
	handlers.handler[1] = (handler_2e5d50)function_2ec200;
	handlers.handler[2] = (handler_2e5d50)function_2ebe40;
	handlers.handler[3] = (handler_2e5d50)function_2ec020;
	table->function_2e5d50(&handlers, 10, -1);
	handlers.handler[0] = (handler_2e5d50)function_2ee1d0;
	handlers.handler[1] = (handler_2e5d50)function_2ec310;
	handlers.handler[2] = (handler_2e5d50)function_2ec370;
	handlers.handler[3] = (handler_2e5d50)function_2ec580;
	table->function_2e5d50(&handlers, -1, 10);
}
